#pragma once
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLShaders.h"

class FireShader
{
public:
    // Variables
    GLuint shaderProgramObject;

    GLuint modelMatrixUniform = 0;
    GLuint viewMatrixUniform = 0;
    GLuint projectionMatrixUniform = 0;
    GLuint scrollSpeedsUniform = 0;
    GLuint frameTimeUniform = 0;
    GLuint scalesUniform = 0;
    GLuint distortion1Uniform = 0;
    GLuint distortion2Uniform = 0;
    GLuint distortion3Uniform = 0;
    GLuint distortionScaleUniform = 0;
    GLuint distortionBiasUniform = 0;
    GLuint fireTextureUniform = 0;
    GLuint noiseTextureUniform = 0;
    GLuint alphaTextureUniform = 0;
    GLuint texture_fire = 0;
    GLuint texture_noise = 0;
    GLuint texture_alpha = 0;
    GLuint alphaUniform = 0;

    // Member Functions
    BOOL initialize()
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\fire\\fire.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\fire\\fire.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        // prelinked binding
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "inputPosition");
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_NORMAL, "inputNormal");
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_TEXTURE0, "inputTexCoord");

        // link
        BOOL bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

        if (bShaderLinkStatus == FALSE)
            return FALSE;

        // post link - getting
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "modelMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "viewMatrix");
        projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "projectionMatrix");
        frameTimeUniform = glGetUniformLocation(shaderProgramObject, "frameTime");
        scrollSpeedsUniform = glGetUniformLocation(shaderProgramObject, "scrollSpeeds");
        scalesUniform = glGetUniformLocation(shaderProgramObject, "scales");
        fireTextureUniform = glGetUniformLocation(shaderProgramObject, "fireTexture");
        noiseTextureUniform = glGetUniformLocation(shaderProgramObject, "noiseTexture");
        alphaTextureUniform = glGetUniformLocation(shaderProgramObject, "alphaTexture");
        distortion1Uniform = glGetUniformLocation(shaderProgramObject, "distortion1");
        distortion2Uniform = glGetUniformLocation(shaderProgramObject, "distortion2");
        distortion3Uniform = glGetUniformLocation(shaderProgramObject, "distortion3");
        distortionScaleUniform = glGetUniformLocation(shaderProgramObject, "distortionScale");
        distortionBiasUniform = glGetUniformLocation(shaderProgramObject, "distortionBias");
        alphaUniform = glGetUniformLocation(shaderProgramObject, "u_Alpha");

        return TRUE;
    }

    void uninitialize(void)
    {
        UninitializeShaders(shaderProgramObject);
    }
};
