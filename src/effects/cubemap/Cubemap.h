#pragma once

#include "../../utils/common.h"
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLLog.h"
#include "../../shaders/cubemap/CubemapShader.h"

class CubeMap
{
public:
    CubemapShader *cubemap = NULL;
    Cube *cube = NULL;
    GLuint vao_cube;
    GLuint vbo_cube_position;
    unsigned int cubemapTexture;
    int isBarasat = 0;
    float alpha = 0.0f;

    BOOL initialize(const char *faces_path[])
    {
        // code
        cubemap = new CubemapShader();
        if (cubemap->initialize() == FALSE)
        {
            PrintLog("Failed to initialize CubeMap\n");
            return FALSE;
        }

        const GLfloat cubeVertices[] =
            {
                // positions          // normals
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
                0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
                0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
                0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
                -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

                -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
                -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

                -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

                0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

                -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
                -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

                -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f};

        // vao
        glGenVertexArrays(1, &vao_cube);
        glBindVertexArray(vao_cube);
        // vbo for position
        glGenBuffers(1, &vbo_cube_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              6 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);

        glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              6 * sizeof(float),
                              (void *)(3 * sizeof(float)));

        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);

        glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind position vbo

        glBindVertexArray(0); // unbind vao

        cubemapTexture = loadCubemap(faces_path);

        return TRUE;
    }

    void display(void)
    {
        // Draw as a pure background: no depth test, no depth writes, no culling.
        // The skybox must be drawn FIRST in the scene so everything else paints on top.
        GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);
        GLboolean cullWasEnabled = glIsEnabled(GL_CULL_FACE);
        GLboolean savedDepthMask = GL_TRUE;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &savedDepthMask);

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);

        // Keep skybox centred on the camera by removing view translation.
        vmath::mat4 skyView = viewMatrix;
        skyView[3][0] = 0.0f;
        skyView[3][1] = 0.0f;
        skyView[3][2] = 0.0f;

        glUseProgram(cubemap->shaderProgramObject);

        glUniformMatrix4fv(cubemap->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(cubemap->viewMatrixUniform, 1, GL_FALSE, skyView);
        glUniformMatrix4fv(cubemap->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
        glUniform1i(cubemap->isBarsatUniform, isBarasat);
        glUniform1f(cubemap->alphaUniform, Cubemap_Alpha);
        if (cubemap->skyBoxUniform >= 0)
            glUniform1i(cubemap->skyBoxUniform, 0);

        glBindVertexArray(vao_cube);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glUseProgram(0);

        glDepthMask(savedDepthMask);
        if (depthWasEnabled)
            glEnable(GL_DEPTH_TEST);
        if (cullWasEnabled)
            glEnable(GL_CULL_FACE);
    }

    void uninitialize(void)
    {
        // code
    }
};
