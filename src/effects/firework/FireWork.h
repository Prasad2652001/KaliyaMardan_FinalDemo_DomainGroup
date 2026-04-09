#pragma once

#include "../../utils/common.h"
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLLog.h"
#include "../../shaders/firework/FireWorkShader.h"
#include <ctime>

// Source Reference: https://codepen.io/MillerTime/pen/zYxYYqw
class FireWork
{
public:
    FireWorkShader *firework = NULL;
    GLuint vao;
    GLuint vbo;
    const int particleCount = 30000;
    float t = 0.0;

    BOOL initialize()
    {
        // code
        firework = new FireWorkShader();
        if (firework->initialize() == FALSE)
        {
            PrintLog("Failed to initialize FireWork\n");
            return FALSE;
        }

        // particles
        std::vector<float> offsets(particleCount * 3);

        // Initialize random number generator
        srand(static_cast<unsigned int>(time(0)));

        // Initialize particle offsets
        /*
        for (int i = 0; i < particleCount; ++i)
        {
            // Generate random offset values
            float offsetX = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f; // Random X between -1 and 1
            float offsetY = static_cast<float>(rand()) / RAND_MAX * 0.9f + 0.1f; // Random Y between 0.1 and 1.0
            float offsetTime = static_cast<float>(rand()) / RAND_MAX;            // Random time between 0.0 and 1.0

            // Store the offset values in the vector
            offsets[i * 3] = offsetX;
            offsets[i * 3 + 1] = offsetY;
            offsets[i * 3 + 2] = offsetTime;
        }
        */
        for (int i = 0; i < particleCount; i++)
        {
            int startIndex = i * 3;

            // Random angle around the cone's central axis
            float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;

            // Random radius for spread (cone's circular base)
            float radius = static_cast<float>(rand()) / RAND_MAX;

            // Random height factor along the cone (0.0 to 1.0)
            float heightFactor = static_cast<float>(rand()) / RAND_MAX;

            // Offset calculation
            offsets[startIndex] = radius * cos(angle) * (1.0f - heightFactor);     // X
            offsets[startIndex + 1] = radius * sin(angle) * (1.0f - heightFactor); // Y
            offsets[startIndex + 2] = heightFactor;                                // Z
        }

        // vao - vertex array object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // vbo for position - vertex buffer object
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, particleCount * 3 * sizeof(float), offsets.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, false, 0, 0); // xyz i.e. 3
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // unbind vao
        glBindVertexArray(0);

        return TRUE;
    }

    void display(void)
    {
        // code
        glEnable(GL_PROGRAM_POINT_SIZE); // Allow programmable point sizes
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(firework->shaderProgramObject);

        // push above mvp into vertex shaders mvp uniform
        glUniformMatrix4fv(firework->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(firework->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(firework->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
        glUniform1f(firework->u_pointSize, 1.0);
        glUniform1f(firework->u_time, t);

        t += gDeltaTime;

        // Render the square model using the firework shader.
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, particleCount);
        glBindVertexArray(0);

        glUseProgram(0);

        glDisable(GL_BLEND);
        glDisable(GL_PROGRAM_POINT_SIZE); // Allow programmable point sizes
    }

    void uninitialize(void)
    {
        // code
        // particles
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
    }
};
