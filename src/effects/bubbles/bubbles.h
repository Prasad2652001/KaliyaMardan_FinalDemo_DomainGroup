#pragma once

#include "../../utils/common.h"
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLLog.h"
#include "../../utils/camera/Camera.h"
#include "../../shaders/bubbles/BubbleShader.h"

class Bubbles
{
private:
public:
    Camera camera;
    BubbleShader* bubbleShader;

    GLuint vao;
    GLuint vbo_position;
    GLuint vbo_seed;
    GLuint vbo_velocity;

    // OpenCL style placeholders kept for template similarity
    GLuint vbo_gpu_position;
    GLuint vbo_gpu_seed;
    GLuint vbo_gpu_velocity;

    GLuint projectionMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint modelMatrixUniform;

    int maxParticles;

    // same array style as Rain.h : 4 floats per particle
    float posBuffer[280000] = { 0 };
    float seedBuffer[280000] = { 0 };
    float veloBuffer[280000] = { 0 };

    float dataBuffer[4 * 50000] = { 0 };

    vec3 eyePos = vec3(0.0f, 0.0f, 4.0f);

    float dt = 0.0f;
    float elapsedTime = 0.0f;

    GLuint texture_bubble;

    // testing quad
    GLuint vao_quad;
    GLuint vbo_quad_position;
    GLuint vbo_quad_texcoord;

    // for light
    GLfloat lightAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightPosition[4] = { 100.0f, 100.0f, 100.0f, 1.0f };
    GLfloat materialAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat materialDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat materialSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat materialShininess = 50.0f;

    GLfloat renderBubbles = true;
    GLfloat alpha = 1.0f;

    // emitter controls
    vec3 emitterPosition = vec3(0.0f, 0.0f, 0.0f);
    vec3 modelOffset = vec3(0.0f, 0.0f, 0.0f);

    float clusterScale = 0.25f;
    float riseFactor = 0.45f;
    float spreadFactor = 0.08f;
    float bubbleSizeFactor = 1.0f;
    float alphaFactor = 0.8f;
    float wobbleFactor = 1.0f;

    // 0 = jump burst, 1 = nose bubbles
    int bubbleMode = 0;

    void setEyePosition(vec3 eye)
    {
        eyePos = eye;
    }

    Bubbles(int particleSize)
    {
        maxParticles = particleSize;

        if ((4 * maxParticles) > 280000)
        {
            maxParticles = 70000;
        }
    }

    void setDeltaTime(float delta)
    {
        dt = delta;
    }

    void setEmitterPosition(vec3 position)
    {
        emitterPosition = position;
    }

    void setModelOffset(vec3 position)
    {
        modelOffset = position;
    }

    void setJumpBubbles(vec3 position)
    {
        bubbleMode = 0;
        emitterPosition = position;
        clusterScale = 0.45f;
        riseFactor = 1.15f;
        spreadFactor = 0.22f;
        bubbleSizeFactor = 1.2f;
        alphaFactor = 0.9f;
        wobbleFactor = 1.2f;
    }
    /*void setJumpBubbles(vec3 position)
    {
        bubbleMode = 0;
        emitterPosition = position;
        clusterScale = 250.0f;
        riseFactor = 180.0f;
        spreadFactor = 90.0f;
        bubbleSizeFactor = 1.5f;
        alphaFactor = 0.95f;
        wobbleFactor = 1.2f;
    }*/

    void setNoseBubbles(vec3 position)
    {
        bubbleMode = 1;
        emitterPosition = position;
        clusterScale = 0.045f;
        riseFactor = 0.35f;
        spreadFactor = 0.035f;
        bubbleSizeFactor = 0.55f;
        alphaFactor = 0.7f;
        wobbleFactor = 0.55f;
    }

    BOOL initialize(GLuint textureNumber)
    {
        GLfloat position[] =
        {
             1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f
        };

        GLfloat coord[] =
        {
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f
        };

        glGenVertexArrays(1, &vao_quad);
        glBindVertexArray(vao_quad);
        {
            glGenBuffers(1, &vbo_quad_position);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_position);
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
                glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
                glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glGenBuffers(1, &vbo_quad_texcoord);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_texcoord);
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(coord), coord, GL_STATIC_DRAW);
                glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, 0);
                glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);

        // initializing Bubble Shader
        bubbleShader = new BubbleShader();
        bubbleShader->initializeBubbleShader();
        PrintLog("bubble shader initialized\n");

        // VAO and VBO Related Code
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
        glBufferData(GL_ARRAY_BUFFER, 4 * maxParticles * sizeof(float), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_seed);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_seed);
        glBufferData(GL_ARRAY_BUFFER, 4 * maxParticles * sizeof(float), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_SEED, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_SEED);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_velocity);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_velocity);
        glBufferData(GL_ARRAY_BUFFER, 4 * maxParticles * sizeof(float), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_VELOCITY, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_VELOCITY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        // loading texture
        if (textureNumber == 1)
        {
            if (LoadPNGImage(&texture_bubble, "./assets/textures/Bubbles/bubble.png") == FALSE)
            {
                PrintLog("Failed to load bubble texture\n");
                return FALSE;
            }
        }
        else if (textureNumber == 2)
        {
            if (LoadPNGImage(&texture_bubble, "./assets/textures/Bubbles/bubble-soft.png") == FALSE)
            {
                PrintLog("Failed to load bubble texture\n");
                return FALSE;
            }
        }
        else
        {
            if (LoadPNGImage(&texture_bubble, "./assets/textures/Bubbles/bubble.png") == FALSE)
            {
                PrintLog("Failed to load bubble texture\n");
                return FALSE;
            }
        }

        return TRUE;
    }

    void createBubbleData(void)
    {
        //eyePos = camera.getEye();
        elapsedTime += dt;

        posBuffer[4 * maxParticles * sizeof(float)];
        seedBuffer[4 * maxParticles * sizeof(float)];
        veloBuffer[4 * maxParticles * sizeof(float)];
        dataBuffer[sizeof(posBuffer) * 3];

        float breathPulse = 0.5f + 0.5f * sinf(elapsedTime * 4.5f);

        for (int i = 0; i < maxParticles; i++)
        {
            int index = i * 4;

            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;

            float seedX = randFloat(0.0f, 1.0f);
            float seedY = randFloat(0.0f, 1.0f);
            float seedZ = randFloat(0.0f, 1.0f);

            if (bubbleMode == 0)
            {
                // jump / splash style bubbles
                x = emitterPosition[0] + (randFloat(-1.0f, 1.0f) * clusterScale);
                y = emitterPosition[1] + (randFloat(-0.3f, 0.3f) * clusterScale);
                z = emitterPosition[2] + (randFloat(-1.0f, 1.0f) * clusterScale);

                posBuffer[index + 0] = x;
                posBuffer[index + 1] = y;
                posBuffer[index + 2] = z;
                posBuffer[index + 3] = 1.0f;

                seedBuffer[index + 0] = seedX;
                seedBuffer[index + 1] = seedY;
                seedBuffer[index + 2] = seedZ;
                //seedBuffer[index + 3] = bubbleSizeFactor * randFloat(0.7f, 1.7f);
                seedBuffer[index + 3] = bubbleSizeFactor * randFloat(8.0f, 18.0f);

                veloBuffer[index + 0] = randFloat(-spreadFactor, spreadFactor);
                veloBuffer[index + 1] = randFloat(riseFactor * 0.45f, riseFactor * 1.35f);
                veloBuffer[index + 2] = randFloat(-spreadFactor, spreadFactor);
                veloBuffer[index + 3] = alphaFactor * randFloat(0.55f, 1.0f);
            }
            else
            {
                // nose breathing bubbles
                float pulseScale = 0.25f + (0.75f * breathPulse);

                x = emitterPosition[0] + (randFloat(-1.0f, 1.0f) * clusterScale * pulseScale);
                y = emitterPosition[1] + (randFloat(-0.15f, 0.15f) * clusterScale);
                z = emitterPosition[2] + (randFloat(-1.0f, 1.0f) * clusterScale * pulseScale);

                posBuffer[index + 0] = x;
                posBuffer[index + 1] = y;
                posBuffer[index + 2] = z;
                posBuffer[index + 3] = 1.0f;

                seedBuffer[index + 0] = seedX;
                seedBuffer[index + 1] = seedY;
                seedBuffer[index + 2] = breathPulse;
                seedBuffer[index + 3] = bubbleSizeFactor * randFloat(0.35f, 0.95f);

                veloBuffer[index + 0] = randFloat(-spreadFactor, spreadFactor) * 0.45f;
                veloBuffer[index + 1] = randFloat(riseFactor * 0.8f, riseFactor * 1.2f);
                veloBuffer[index + 2] = randFloat(-spreadFactor, spreadFactor) * 0.45f;
                veloBuffer[index + 3] = alphaFactor * randFloat(0.35f, 0.85f);
            }
        }
    }

    void display(void)
    {
        if (renderBubbles == false)
            return;

        pushMatrix(modelMatrix);
        {
            createBubbleData();

            glUseProgram(bubbleShader->shaderProgramObject);

            glUniformMatrix4fv(bubbleShader->modelMatrixUniform, 1, GL_FALSE, vmath::translate(modelOffset));
            //glUniformMatrix4fv(bubbleShader->viewMatrixUniform, 1, GL_FALSE, mat4::identity());
            glUniformMatrix4fv(bubbleShader->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(bubbleShader->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

            //glUniform3fv(bubbleShader->eyePosUniform, 1, camera.getEye());
            glUniform3fv(bubbleShader->eyePosUniform, 1, eyePos);
            glUniform1f(bubbleShader->dtUniform, elapsedTime);

            // FOR LIGHT
            glUniform3fv(bubbleShader->laUniform, 1, lightAmbient);
            glUniform3fv(bubbleShader->ldUniform, 1, lightDiffuse);
            glUniform3fv(bubbleShader->lsUniform, 1, lightSpecular);
            glUniform4fv(bubbleShader->lightPositionUniform, 1, lightPosition);

            glUniform3fv(bubbleShader->kaUniform, 1, materialAmbient);
            glUniform3fv(bubbleShader->kdUniform, 1, materialDiffuse);
            glUniform3fv(bubbleShader->ksUniform, 1, materialSpecular);
            glUniform1f(bubbleShader->materialShinessUniform, materialShininess);
            glUniform1f(bubbleShader->alphaUniform, alpha);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_bubble);
            glUniform1i(bubbleShader->bubbleTextureSamplerUniform, 0);

            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
            glBufferData(GL_ARRAY_BUFFER, 4 * maxParticles * sizeof(float), posBuffer, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);

            glBindBuffer(GL_ARRAY_BUFFER, vbo_seed);
            glBufferData(GL_ARRAY_BUFFER, 4 * maxParticles * sizeof(float), seedBuffer, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(MATRIX_ATTRIBUTE_SEED, 4, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MATRIX_ATTRIBUTE_SEED);

            glBindBuffer(GL_ARRAY_BUFFER, vbo_velocity);
            glBufferData(GL_ARRAY_BUFFER, 4 * maxParticles * sizeof(float), veloBuffer, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(MATRIX_ATTRIBUTE_VELOCITY, 4, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MATRIX_ATTRIBUTE_VELOCITY);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glDrawArrays(GL_POINTS, 0, maxParticles);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDisable(GL_BLEND);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
        modelMatrix = popMatrix();
    }

    void display1(void)
    {
        glUseProgram(commonShaders->textureShader->shaderProgramObject);

        glUniformMatrix4fv(commonShaders->textureShader->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(commonShaders->textureShader->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(commonShaders->textureShader->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

        glEnable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_bubble);
        glUniform1i(commonShaders->textureShader->textureSamplerUniform, 0);

        glBindVertexArray(vao_quad);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        glUseProgram(0);
    }

    float randomf()
    {
        return (float((double)rand() / ((double)(RAND_MAX)+(double)(1))));
    }

    float randFloat(float min, float max)
    {
        return min + (rand() / (RAND_MAX / (max - min)));
    }

    float randInt(int min, int max)
    {
        return min + (rand() / (RAND_MAX / (max - min)));
    }

    void uninitialize(void)
    {
        if (bubbleShader)
        {
            bubbleShader->uninitialize();
            delete bubbleShader;
            bubbleShader = NULL;
        }

        if (texture_bubble)
        {
            glDeleteTextures(1, &texture_bubble);
            texture_bubble = 0;
        }

        if (vbo_velocity)
        {
            glDeleteBuffers(1, &vbo_velocity);
            vbo_velocity = 0;
        }

        if (vbo_seed)
        {
            glDeleteBuffers(1, &vbo_seed);
            vbo_seed = 0;
        }

        if (vbo_position)
        {
            glDeleteBuffers(1, &vbo_position);
            vbo_position = 0;
        }

        if (vao)
        {
            glDeleteVertexArrays(1, &vao);
            vao = 0;
        }

        if (vbo_quad_texcoord)
        {
            glDeleteBuffers(1, &vbo_quad_texcoord);
            vbo_quad_texcoord = 0;
        }

        if (vbo_quad_position)
        {
            glDeleteBuffers(1, &vbo_quad_position);
            vbo_quad_position = 0;
        }

        if (vao_quad)
        {
            glDeleteVertexArrays(1, &vao_quad);
            vao_quad = 0;
        }
    }
};