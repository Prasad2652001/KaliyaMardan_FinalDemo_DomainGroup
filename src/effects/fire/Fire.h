#pragma once

#include "../../utils/common.h"
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLLog.h"
#include "../../shaders/fire/FireShader.h"

// Source Reference:
// https://www.rastertek.com/dx11win10tut33.html
// https://www.rastertek.com/gl4linuxtut33.html
class Fire
{
public:
    FireShader *fire = NULL;
    Cube *square = NULL;
    GLuint vao_square;
    GLuint vbo_square;
    GLuint texture_fire = 0;
    GLuint texture_noise = 0;
    GLuint texture_alpha = 0;

    BOOL initialize()
    {
        // code
        fire = new FireShader();
        if (fire->initialize() == FALSE)
        {
            PrintLog("Failed to initialize fire\n");
            return FALSE;
        }

        const GLfloat square_pcnt[] = {
            // position		//texture_coordinates		// normal
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,

            1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f};

        // square
        // vao - vertex array object
        glGenVertexArrays(1, &vao_square);
        glBindVertexArray(vao_square);

        // vbo for position - vertex buffer object
        glGenBuffers(1, &vbo_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_square);
        glBufferData(GL_ARRAY_BUFFER, 6 * 8 * sizeof(float), square_pcnt, GL_STATIC_DRAW);
        // position
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(0 * sizeof(float)));
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        // texture_coordinates
        glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
        // normal
        glVertexAttribPointer(MATRIX_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // unbind vao
        glBindVertexArray(0);

        // texturess
        stbi_set_flip_vertically_on_load(TRUE);
        if (loadTextureForFire(&texture_fire, "./assets/textures/fire/fire01.png", false) == FALSE)
        {
            PrintLog("LoadGLTexture Failed for fire01.png...\n");
            return (-6);
        }
        if (loadTextureForFire(&texture_noise, "./assets/textures/fire/noise01.png", true) == FALSE)
        {
            PrintLog("LoadGLTexture Failed for noise01.png...\n");
            return (-6);
        }
        if (loadTextureForFire(&texture_alpha, "./assets/textures/fire/alpha01.png", false) == FALSE)
        {
            PrintLog("LoadGLTexture Failed for alpha01.png...\n");
            return (-6);
        }
        stbi_set_flip_vertically_on_load(FALSE);

        return TRUE;
    }

    void display(void)
    {
        // code
        float scrollSpeeds[3], scales[3], distortion1[2], distortion2[2], distortion3[2];
        float distortionScale, distortionBias;
        static float frameTime = 0.0f;

        // Increment the frame time counter.
        frameTime += 0.001f;
        if (frameTime > 1000.0f)
        {
            frameTime = 0.0f;
        }

        // Set the three scrolling speeds for the three different noise textures.
        scrollSpeeds[0] = 1.3f;
        scrollSpeeds[1] = 2.1f;
        scrollSpeeds[2] = 2.3f;

        // Set the three scales which will be used to create the three different noise octave textures.
        scales[0] = 1.0f;
        scales[1] = 2.0f;
        scales[2] = 3.0f;

        // Set the three different x and y distortion factors for the three different noise textures.
        distortion1[0] = 0.1f;
        distortion1[1] = 0.2f;

        distortion2[0] = 0.1f;
        distortion2[1] = 0.3f;

        distortion3[0] = 0.1f;
        distortion3[1] = 0.1f;

        // The the scale and bias of the texture coordinate sampling perturbation.
        distortionScale = 0.6f;
        distortionBias = 0.6f;

        // Enable alpha blending.
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

        glUseProgram(fire->shaderProgramObject);

        // push above mvp into vertex shaders mvp uniform
        glUniformMatrix4fv(fire->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(fire->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(fire->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
        glUniform1f(fire->frameTimeUniform, frameTime);
        glUniform3fv(fire->scrollSpeedsUniform, 1, scrollSpeeds);
        glUniform3fv(fire->scalesUniform, 1, scales);
        glUniform2fv(fire->distortion1Uniform, 1, distortion1);
        glUniform2fv(fire->distortion2Uniform, 1, distortion2);
        glUniform2fv(fire->distortion3Uniform, 1, distortion3);
        glUniform1f(fire->distortionScaleUniform, distortionScale);
        glUniform1f(fire->distortionBiasUniform, distortionBias);

        // bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_fire);
        glUniform1i(fire->fireTextureUniform, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_noise);
        glUniform1i(fire->noiseTextureUniform, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture_alpha);
        glUniform1i(fire->alphaTextureUniform, 2);

        // Render the square model using the fire shader.
        glBindVertexArray(vao_square);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Disable alpha blending.
        glDisable(GL_BLEND);

        // unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(0);
    }

    void uninitialize(void)
    {
        // code
    }
};
