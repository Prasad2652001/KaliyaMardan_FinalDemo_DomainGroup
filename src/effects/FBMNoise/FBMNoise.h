#pragma once

#include "../../utils/common.h"
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLLog.h"
#include "../../shaders/fbmNoise/FBMNoiseShader.h"
#include <ctime>

class FbmNoise
{
public:
    FBMNoiseShader *fbmNoise = NULL;
    GLuint vao_square;
    GLuint vbo_position_square;
    float t = 0.0;

    BOOL initialize()
    {
        // code
        fbmNoise = new FBMNoiseShader();
        if (fbmNoise -> initialize() == FALSE)
        {
            PrintLog("Failed to initialize fbmNoise\n");
            return FALSE;
        }

        
       const GLfloat square_position[] = {
            
                                            1.0f, 1.0f, 0.0f,
                                            -1.0f, 1.0f, 0.0f,
                                            -1.0f, -1.0f, 0.0f,
                                            1.0f, -1.0f, 0.0f
                                      };

        //-------------------------------------------------------------------------------------------------------------- SQUARE
        // VAO
        glGenVertexArrays(1, &vao_square);
        // Bind With Vao
        glBindVertexArray(vao_square);
        // VBO for position
        glGenBuffers(1, &vbo_position_square);
        // Create Vertex Buffer object for position
        // Bind With VBO of position
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
        // Push the data
        glBufferData(GL_ARRAY_BUFFER, sizeof(square_position), square_position, GL_STATIC_DRAW);
        glVertexAttribPointer(
            MATRIX_ATTRIBUTE_POSITION,
            3,                              //  create 3 vertices  
            GL_FLOAT,
            GL_FALSE,
            0,
            NULL
        );
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return TRUE;
        
    }

    void display(float time , float alpha)
    {
        // code
        glEnable(GL_PROGRAM_POINT_SIZE); // Allow programmable point sizes
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(fbmNoise->shaderProgramObject);

        // push above mvp into vertex shaders mvp uniform
        
        glUniform1f(fbmNoise->timeUniform, time);
        glUniform1f(fbmNoise->alphaUniform , alpha);

        // Render the square model using the fbmNoise shader.
       glBindVertexArray(vao_square);
       
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glBindVertexArray(0);

        //////////////////////////////////////////////////////
        glUseProgram(0);

        glDisable(GL_BLEND);
   
    }

    void uninitialize(void)
    {
        // code
            if (vbo_position_square)
            {
                glDeleteBuffers(1, &vbo_position_square);
                vbo_position_square = 0;
            }
            if (vao_square)
            {
                glDeleteVertexArrays(1, &vao_square);
                vao_square = 0;

            }
    }
};
