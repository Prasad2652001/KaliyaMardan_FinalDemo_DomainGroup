#pragma once
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLShaders.h"

class FireWorkShader
{
public:
    // Variables
    GLuint shaderProgramObject;

    GLuint u_time = 0;
    GLuint u_pointSize = 0;
    GLuint modelMatrixUniform = 0;
    GLuint viewMatrixUniform = 0;
    GLuint projectionMatrixUniform = 0;

    // Member Functions
    BOOL initialize()
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\firework\\firework.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\firework\\firework.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        // prelinked binding
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "aPosition");

        // link
        BOOL bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

        if (bShaderLinkStatus == FALSE)
            return FALSE;

        // post link - getting
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "modelMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "viewMatrix");
        projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "projectionMatrix");
        u_time = glGetUniformLocation(shaderProgramObject, "u_time");
        u_pointSize = glGetUniformLocation(shaderProgramObject, "u_pointSize");

        return TRUE;
    }

    void uninitialize(void)
    {
        UninitializeShaders(shaderProgramObject);
    }
};
