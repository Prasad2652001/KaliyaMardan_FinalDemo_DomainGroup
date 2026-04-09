#pragma once
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLShaders.h"
#include "../../includes/vmath.h"
using namespace vmath;

class SmokeShader
{
public:
    // Variables
 
    GLuint shaderProgramObject;


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
    GLuint alphaTextureOneUniform = 0;
    GLuint alphaTextureTwoUniform = 0;
    GLuint texture_fire = 0;
    GLuint texture_noise = 0;
    GLuint texture_alpha_one = 0;
    GLuint texture_alpha_two = 0;
    GLuint checkVariableUniform = 0;

    BOOL initialize(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/smoke.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/smoke.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        // prelinked binding
        // Binding Position Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // Binding Color Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");
 
        // link
        BOOL bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

        if (bShaderLinkStatus == FALSE)
            return FALSE;

        // post link - getting
        frameTimeUniform = glGetUniformLocation(shaderProgramObject, "frameTime");
        scrollSpeedsUniform = glGetUniformLocation(shaderProgramObject, "scrollSpeeds");
        scalesUniform = glGetUniformLocation(shaderProgramObject, "scales");
        fireTextureUniform = glGetUniformLocation(shaderProgramObject, "fireTexture");
        noiseTextureUniform = glGetUniformLocation(shaderProgramObject, "noiseTexture");
        alphaTextureOneUniform = glGetUniformLocation(shaderProgramObject, "alphaTextureOne");
        alphaTextureTwoUniform = glGetUniformLocation(shaderProgramObject, "alphaTextureTwo");
        distortion1Uniform = glGetUniformLocation(shaderProgramObject, "distortion1");
        distortion2Uniform = glGetUniformLocation(shaderProgramObject, "distortion2");
        distortion3Uniform = glGetUniformLocation(shaderProgramObject, "distortion3");
        distortionScaleUniform = glGetUniformLocation(shaderProgramObject, "distortionScale");
        distortionBiasUniform = glGetUniformLocation(shaderProgramObject, "distortionBias");
        checkVariableUniform = glGetUniformLocation(shaderProgramObject , "checkVariable");

        return TRUE;
    }

    void uninitialize(void)
    {
        UninitializeShaders(shaderProgramObject);
    }
};
