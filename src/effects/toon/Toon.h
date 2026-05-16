#pragma once

#include "../../utils/common.h"
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLLog.h"
#include "../../shaders/toon/ToonShader.h"

#define FBO_WIDTH 1920
#define FBO_HEIGHT 1080

class ToonEffect
{
public:
    ToonShader *toon = NULL;

    GLuint vao;
    GLuint vbo;

    GLfloat lightAmbient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat lightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightPosition[4] = {10.0f, 10.0f, 10.0f, 1.0f};

    GLfloat materialAmbient[4] = {0.0, 0.0, 0.0, 1.0f};
    GLfloat materialDiffuse[4] = {0.5f, 0.2f, 0.7f, 1.0f};
    GLfloat materialSpecular[4] = {0.7f, 0.7f, 0.7f, 1.0f};

    GLfloat materialShininess = 128.0f;

    BOOL initialize()
    {
        // code
        toon = new ToonShader();
        if (toon->initialize() == FALSE)
        {
            PrintLog("Failed to initialize toon shader\n");
            return FALSE;
        }

        PrintLog("Toon Shader initialized successfully\n");

        return TRUE;
    }

    void beginToonShader(void)
    {
        // code
        glUseProgram(toon->shaderProgramObject);

        // M V P matrix
        glUniformMatrix4fv(toon->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(toon->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(toon->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

        // Light
        glUniform3fv(toon->laUniform, 1, lightAmbient);
        glUniform3fv(toon->ldUniform, 1, lightDiffuse);
        glUniform3fv(toon->lsUniform, 1, lightSpecular);

        glUniform3fv(toon->kaUniform, 1, materialAmbient);
        glUniform3fv(toon->kdUniform, 1, materialDiffuse);
        glUniform3fv(toon->ksUniform, 1, materialSpecular);

        glUniform1f(toon->materialShininessUniform, materialShininess);
        glUniform4fv(toon->lightPositionUniform, 1, lightPosition);
        // glUniform1i(toon->keyPressedUniform, 1);
    }

    void endToonShader()
    {
        glUseProgram(0);
    }

    void disableLightingForToon()
    {
        glUseProgram(toon->shaderProgramObject);

        glUniform1i(
            toon->keyPressedUniform,
            0);

        glUseProgram(0);
    }

    void uninitialize(void)
    {
        if (toon)
        {
            toon->uninitialize();
            delete toon;
            toon = NULL;
        }
    }
};
