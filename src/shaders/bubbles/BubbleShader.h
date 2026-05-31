#pragma once
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLShaders.h"

class BubbleShader
{
public:
    // Variables
    GLuint shaderProgramObject;
    GLuint projectionMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint modelMatrixUniform;

    GLuint eyePosUniform;
    GLuint dtUniform;
    GLuint bubbleTextureSamplerUniform;

    // For light
    GLuint lightPositionUniform;
    GLuint laUniform;
    GLuint ldUniform;
    GLuint lsUniform;
    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    GLuint materialShinessUniform;
    GLuint alphaUniform;

    // Member Functions
    BOOL initializeBubbleShader()
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\bubbles\\bubbles.vs", VERTEX);

        // geometry shader
        GLuint geometryShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\bubbles\\bubbles.gs", GEOMETRY);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\bubbles\\bubbles.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, geometryShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        // prelinked binding
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "positionMC");
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_SEED, "vertexSeed");
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_VELOCITY, "vertexVelo");

        // link
        BOOL bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

        if (bShaderLinkStatus == FALSE)
            return FALSE;

        // post link - getting
        projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");

        eyePosUniform = glGetUniformLocation(shaderProgramObject, "eyePosition");
        dtUniform = glGetUniformLocation(shaderProgramObject, "dt");
        bubbleTextureSamplerUniform = glGetUniformLocation(shaderProgramObject, "bubbleTex");

        // for light
        laUniform = glGetUniformLocation(shaderProgramObject, "u_la");
        ldUniform = glGetUniformLocation(shaderProgramObject, "u_ld");
        lsUniform = glGetUniformLocation(shaderProgramObject, "u_ls");
        lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_lightPosition");

        kaUniform = glGetUniformLocation(shaderProgramObject, "u_ka");
        kdUniform = glGetUniformLocation(shaderProgramObject, "u_kd");
        ksUniform = glGetUniformLocation(shaderProgramObject, "u_ks");
        materialShinessUniform = glGetUniformLocation(shaderProgramObject, "u_materialShiness");
        alphaUniform = glGetUniformLocation(shaderProgramObject, "u_alpha");

        return TRUE;
    }

    void uninitialize(void)
    {
        UninitializeShaders(shaderProgramObject);
    }
};