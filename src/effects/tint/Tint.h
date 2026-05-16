#pragma once

#include "../../utils/common.h"
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLLog.h"
#include "../../shaders/tint/TintShader.h"

#define FBO_WIDTH 1920
#define FBO_HEIGHT 1080

class TintEffect
{
public:
    TintShader *tint = NULL;

    GLuint vao;
    GLuint vbo;
    GLuint fbo = 0;
    GLuint rbo = 0;
    GLuint texture_FBO = 0;

    BOOL initialize()
    {
        // code
        tint = new TintShader();
        if (tint->initialize() == FALSE)
        {
            PrintLog("Failed to initialize Tint shader\n");
            return FALSE;
        }

        const GLfloat square_vertices[] = {
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

        // square
        // vao - vertex array object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(float), square_vertices, GL_STATIC_DRAW);

        // position
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(0 * sizeof(float)));
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        // texture_coordinates
        glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
        // normal
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // unbind vao
        glBindVertexArray(0);

        if (createFBO() == TRUE)
        {
            PrintLog("\n FBO creation Sucessful!! \n");
        }
        else
        {
            PrintLog("\n FBO creation Failed!! \n");
            return FALSE;
        }

        return TRUE;
    }

    BOOL createFBO()
    {

        // variable declarions
        GLint maxRenderBufferSize = 0;

        // code
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);
        if (FBO_WIDTH > maxRenderBufferSize || FBO_HEIGHT > maxRenderBufferSize)
        {
            PrintLog("\n Execiding max size for tint effect FBO \n");
            return FALSE;
        }
        else
        {
            glGenFramebuffers(1, &fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, FBO_WIDTH, FBO_HEIGHT); // GL_DEPTH_COMPONENT16
            glGenTextures(1, &texture_FBO);
            glBindTexture(GL_TEXTURE_2D, texture_FBO);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            // Create this texture for fbo
            glTexImage2D(
                GL_TEXTURE_2D, 0,
                GL_RGBA, FBO_WIDTH, FBO_HEIGHT, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, NULL // last null is as texture,  middle 6 as human eye sensible to greenry
            );                                  // GL_UNSIGNED_SHORT_5_6_5

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_FBO, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                PrintLog("\n fbo creation is incomplete \n");
                return FALSE;
            }
            else
            {
                // glBindTexture(GL_TEXTURE_2D, 0);
                // glBindRenderbuffer(GL_RENDERBUFFER, 0);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                return TRUE;
            }
        }
    }

    void bindTintFBO()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, FBO_WIDTH, FBO_HEIGHT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void unbindTintFBO()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void display(int tintMode = 1, float vignettePower = 1.8f, float tintStrength = 0.85f)
    {
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

        glUseProgram(tint->shaderProgramObject);

        glUniformMatrix4fv(tint->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(tint->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(tint->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

        glUniform1i(tint->tintModeUniform, tintMode);
        glUniform1f(tint->vignettePowerUniform, vignettePower);
        glUniform1f(tint->tintStrengthUniform, tintStrength);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_FBO);
        glUniform1i(tint->textureSamplerUniformFBO, 0);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

    void uninitialize(void)
    {
        if (texture_FBO)
        {
            glDeleteTextures(1, &texture_FBO);
            texture_FBO = 0;
        }

        if (rbo)
        {
            glDeleteRenderbuffers(1, &rbo);
            rbo = 0;
        }

        if (fbo)
        {
            glDeleteFramebuffers(1, &fbo);
            fbo = 0;
        }

        if (vbo)
        {
            glDeleteBuffers(1, &vbo);
            vbo = 0;
        }

        if (vao)
        {
            glDeleteVertexArrays(1, &vao);
            vao = 0;
        }

        if (tint)
        {
            tint->uninitialize();
            delete tint;
            tint = NULL;
        }
    }
};
