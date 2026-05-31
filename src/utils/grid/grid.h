#pragma once
#include "../../utils/common.h"
#include "../../shaders/grid/GridShader.h"

// Quad

class Grid
{
public:
    GridShader *grid = NULL;

    GLuint vao_grid;
    GLuint vbo_grid_position;

    BOOL initialize()
    {
        const GLfloat GridPosition[] = {
            // Triangle 1
            -100.0f, 0.0f, -100.0f,
            100.0f, 0.0f, -100.0f,
            100.0f, 0.0f, 100.0f,

            // Triangle 2
            -100.0f, 0.0f, -100.0f,
            100.0f, 0.0f, 100.0f,
            -100.0f, 0.0f, 100.0f};

        grid = new GridShader();
        if (grid->initialize() == FALSE)
        {
            PrintLog("Failed to initialize grid shader\n");
            return FALSE;
        }

        // vao Grid
        glGenVertexArrays(1, &vao_grid);
        glBindVertexArray(vao_grid);
        // position
        glGenBuffers(1, &vbo_grid_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_grid_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GridPosition), GridPosition, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        return true;
    }

    void display()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(grid->shaderProgramObject);

        glUniformMatrix4fv(grid->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(grid->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(grid->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

        glBindVertexArray(vao_grid);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glUseProgram(0);

        glDisable(GL_BLEND);
    }

    void uninitialize()
    {
        if (vbo_grid_position)
        {
            glDeleteBuffers(1, &vbo_grid_position);
            vbo_grid_position = 0;
        }

        if (vao_grid)
        {
            glDeleteBuffers(1, &vao_grid);
            vao_grid = 0;
        }

        if (grid)
        {
            grid->uninitialize();
            delete grid;
            grid = NULL;
        }
    }
};
