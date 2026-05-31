#pragma once
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLShaders.h"

class ToonShader
{
public:
        // Variables
        GLuint shaderProgramObject;
        // matrix uniform
        GLuint projectionMatrixUniform;
        GLuint viewMatrixUniform;
        GLuint modelMatrixUniform;

        // Light uniforms
        GLuint laUniform;
        GLuint ldUniform;
        GLuint lsUniform;
        GLuint lightPositionUniform;

        // Material uniforms
        GLuint kaUniform;
        GLuint kdUniform;
        GLuint ksUniform;
        GLuint materialShininessUniform;

        // Toggle lighting
        GLuint keyPressedUniform;

        // Member Functions
        BOOL initialize()
        {
                // vertex Shader
                GLuint vertexShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\toon\\toon.vs", VERTEX);

                // fragment Shader
                GLuint fragmentShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\toon\\toon.fs", FRAGMENT);

                shaderProgramObject = glCreateProgram();
                glAttachShader(shaderProgramObject, vertexShaderObject);
                glAttachShader(shaderProgramObject, fragmentShaderObject);

                // Binding Position Array
                glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "a_position");
                glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_NORMAL, "a_Normal");

                // link
                BOOL bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

                if (bShaderLinkStatus == FALSE)
                        return FALSE;

                projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
                viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
                modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");

                // light

                // Light uniforms
                laUniform = glGetUniformLocation(shaderProgramObject, "u_LA");
                ldUniform = glGetUniformLocation(shaderProgramObject, "u_LD");
                lsUniform = glGetUniformLocation(shaderProgramObject, "u_LS");

                // material uniform
                kaUniform = glGetUniformLocation(shaderProgramObject, "u_KA");
                kdUniform = glGetUniformLocation(shaderProgramObject, "u_KD");
                ksUniform = glGetUniformLocation(shaderProgramObject, "u_KS");

                // Lighting toggle
                // keyPressedUniform = glGetUniformLocation(shaderProgramObject, "u_KeyisPressed");
                materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_MaterialShininess");
                lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_LightPosition");

                return TRUE;
        }

        void uninitialize(void)
        {
                UninitializeShaders(shaderProgramObject);
        }
};
