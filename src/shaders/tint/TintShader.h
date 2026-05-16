#pragma once
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLShaders.h"

class TintShader
{
public:
        // Variables
        GLuint shaderProgramObject;
        GLuint projectionMatrixUniform;
        GLuint viewMatrixUniform;
        GLuint modelMatrixUniform;
        GLuint textureSamplerUniformFBO;

        GLuint tintModeUniform;
        GLuint vignettePowerUniform;
        GLuint tintStrengthUniform;

        // Member Functions
        BOOL initialize()
        {
                // vertex Shader
                GLuint vertexShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\tint\\tint.vs", VERTEX);

                // fragment Shader
                GLuint fragmentShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\tint\\tint.fs", FRAGMENT);

                shaderProgramObject = glCreateProgram();
                glAttachShader(shaderProgramObject, vertexShaderObject);
                glAttachShader(shaderProgramObject, fragmentShaderObject);

                // Binding Position Array
                glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "a_position");
                glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_TEXTURE0, "a_TexCoord");

                // link
                BOOL bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

                if (bShaderLinkStatus == FALSE)
                        return FALSE;

                projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
                viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
                modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
                textureSamplerUniformFBO = glGetUniformLocation(shaderProgramObject, "uTextureSampler");
                tintModeUniform = glGetUniformLocation(shaderProgramObject, "u_TintMode");
                vignettePowerUniform = glGetUniformLocation(shaderProgramObject, "u_VignettePower");
                tintStrengthUniform = glGetUniformLocation(shaderProgramObject, "u_TintStrength");
                return TRUE;
        }

        void uninitialize(void)
        {
                UninitializeShaders(shaderProgramObject);
        }
};
