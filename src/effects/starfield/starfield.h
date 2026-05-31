#pragma once

#include "../../utils/common.h"
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLLog.h"
#include "../../shaders/starfield/starFieldShader.h"

class StarField
{
public:
    StarFieldShader *starfieldShader = NULL;

    // Random number generator
    unsigned int seed = 0x13371337;

    float lfTime = 0.1f;

    struct star_t
    {
        GLfloat position[3];
    };
    star_t *starPos = NULL;

    struct Star_color
    {
        GLfloat color[3];
    };
    Star_color *starColor = NULL;

    int NUM_STARS = 2000;

    GLuint vao;
    GLuint vbo_position;
    GLuint vbo_color;

    GLuint starFieldTexture;

    float timeUniform;
    double timerStart;

    int initialize()
    {
        starfieldShader = new StarFieldShader();

        starfieldShader->initialize();
        initVertices();
        initColor();

        // VAO related code
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // VBO For Postition
        glGenBuffers(1, &vbo_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(star_t) * NUM_STARS, starPos, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // VBO For Color
        glGenBuffers(1, &vbo_color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Star_color) * NUM_STARS, starColor, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (LoadPNGImage(&starFieldTexture, "./assets/textures/StarField/star_p.png") == FALSE)
        {
            PrintLog("LoadGLTexture Failed for texture_starField..\n");
            return (-6);
        }

        return (TRUE);
    }

    void display(void)
    {
        lfTime *= 0.1f;
        lfTime -= floor(lfTime);

        // code
        glUseProgram(starfieldShader->shaderProgramObject);
        {
            glEnable(GL_PROGRAM_POINT_SIZE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glUniformMatrix4fv(starfieldShader->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
            glUniformMatrix4fv(starfieldShader->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(starfieldShader->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
            glUniform1f(starfieldShader->timeUniform, lfTime * 0.005f);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, starFieldTexture);

            glEnable(GL_POINT_SPRITE);
            glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
            glUniform1i(starfieldShader->textureSamplerUniform, 0);

            glBindVertexArray(vao);
            glDrawArrays(GL_POINTS, 0, NUM_STARS);
            glBindTexture(GL_TEXTURE_2D, 0);

            glDisable(GL_POINT_SPRITE);
            glDisable(GL_PROGRAM_POINT_SIZE);
            glDisable(GL_BLEND);
            glBindVertexArray(0);
        }
        glUseProgram(0);

        if (lfTime < 1.0f)
        {
            lfTime += 0.01f;
        }
    }

    float random_float()
    {
        float res;
        unsigned int tmp;

        seed *= 16807;

        tmp = seed ^ (seed >> 4) ^ (seed << 15);

        *((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

        return (res - 1.0f);
    }

    void initVertices(void)
    {
        starPos = (star_t *)malloc(sizeof(star_t) * NUM_STARS);

        float minRadius = 10.0f;
        float maxRadius = 300.0f;
        int totalTurns = 10;
        int numArms = 8;
        float armSeparation = 2.0f * M_PI / numArms;
        float armOffsetStrength = 0.8f;

        int i = 0;
        while (i < NUM_STARS)
        {
            float t = powf((float)i / (float)NUM_STARS, 0.7f); // More stars near center
            float angle = t * totalTurns * 2.0f * M_PI;

            int arm = i % numArms;
            angle += arm * armSeparation;
            angle += sin(t * 30.0f) * armOffsetStrength;  // wavy spiral

            float baseRadius = minRadius + t * (maxRadius - minRadius);
            float radiusMultiplier = 1.0f + 0.3f * sin(angle);
            float radius = baseRadius * radiusMultiplier;

            float jitterRadius = (random_float() - 0.5f) * radius * 0.2f;
            float jitterAngle = (random_float() - 0.5f) * 0.3f;
            float jitterZ = (random_float() - 0.5f) * 300.0f; // Increased Z depth from ±50 to ±150

            float finalAngle = angle + jitterAngle;
            float finalRadius = radius + jitterRadius;

            float x = finalRadius * cos(finalAngle);
            float y = finalRadius * sin(finalAngle);
            float z = jitterZ;

            starPos[i].position[0] = x;
            starPos[i].position[1] = y;
            starPos[i].position[2] = z;

            // Occasionally insert star clusters
            if (random_float() < 0.05f && i + 5 < NUM_STARS) // 5% of stars are cluster centers
            {
                for (int j = 1; j <= 5; j++)
                {
                    int idx = i + j;
                    float dx = x + (random_float() - 0.5f) * 5.0f;
                    float dy = y + (random_float() - 0.5f) * 5.0f;
                    float dz = z + (random_float() - 0.5f) * 100.0f; // Increased depth for clusters too

                    starPos[idx].position[0] = dx;
                    starPos[idx].position[1] = dy;
                    starPos[idx].position[2] = dz;
                }
                i += 5;
            }

            i++;
        }
    }

    // normal color blue 
    void initColor(void)
    {
        starColor = (Star_color *)malloc(sizeof(Star_color) * NUM_STARS);

        for (int i = 0; i < NUM_STARS; i++)
        {
            float t = powf((float)i / (float)NUM_STARS, 0.7f); // denser near center
            float centerWeight = 1.0f - t;

            float red = 1.0f * centerWeight + 0.3f * (1.0f - centerWeight);
            float green = 0.9f * centerWeight + 0.6f * (1.0f - centerWeight);
            float blue = 0.6f * centerWeight + 1.0f * (1.0f - centerWeight);

            red += (random_float() - 0.5f) * 0.2f;
            green += (random_float() - 0.5f) * 0.2f;
            blue += (random_float() - 0.5f) * 0.2f;

            red = fminf(fmaxf(red, 0.0f), 1.0f);
            green = fminf(fmaxf(green, 0.0f), 1.0f);
            blue = fminf(fmaxf(blue, 0.0f), 1.0f);

            starColor[i].color[0] = red;
            starColor[i].color[1] = green;
            starColor[i].color[2] = blue;
        }
    }

   
    void uninitialize(void)
    {
        starfieldShader->uninitialize();
        if (starFieldTexture)
        {
            glDeleteTextures(1, &starFieldTexture);
            starFieldTexture = 0;
        }

        if (vbo_position)
        {
            glDeleteBuffers(1, &vbo_position);
            vbo_position = 0;
        }

        if (vbo_color)
        {
            glDeleteBuffers(1, &vbo_color);
            vbo_color = 0;
        }

        if (vao)
        {
            glDeleteVertexArrays(1, &vao);
            vao = 0;
        }
    }
};

