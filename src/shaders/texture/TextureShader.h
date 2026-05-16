#pragma once
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLShaders.h"
#include "../../includes/vmath.h"
using namespace vmath;
class TextureShader
{
public:
    // Variables
    /* SKY RELATED VARIALBES*/
    GLuint shaderProgramObject;

    GLuint projectionMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint modelMatrixUniform;
    GLfloat texRepeatUniform;
    GLuint textureSamplerUniform;
    GLuint alphaValueUniform;
    GLuint iTimeUniform;
    GLuint bShineUniform;
    GLuint vao;
    GLuint vbo_position;
    GLuint vbo_texcoords;
    GLuint vbo_normal;
    GLuint materialShininessUniform;
    GLuint applyToonUniform;
    GLuint applySpecularUniform;
    GLuint lightPositionUniform;

    BOOL initialize(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/texture/texture.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/texture/texture.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        // prelinked binding
        // Binding Position Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // Binding Color Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");
        // Biding the NOrmals
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_NORMAL, "a_normal");

        // link
        BOOL bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

        if (bShaderLinkStatus == FALSE)
            return FALSE;

        // post link - getting
        texRepeatUniform = glGetUniformLocation(shaderProgramObject, "u_texRepeat");
        projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
        textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler");
        alphaValueUniform = glGetUniformLocation(shaderProgramObject, "u_alphaVal");
        iTimeUniform = glGetUniformLocation(shaderProgramObject, "iTime");
        bShineUniform = glGetUniformLocation(shaderProgramObject, "bShine");
        materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_MaterialShininess");
        applyToonUniform = glGetUniformLocation(shaderProgramObject, "u_ApplyToon");
        applySpecularUniform = glGetUniformLocation(shaderProgramObject, "u_ApplySpecular");
        lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_LightPosition");

        // VAO For QUAD

        const GLfloat positions[] =
            {
                // front
                1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f};

        const GLfloat texcoord[] =
            {
                1.0, 0.0,
                0.0, 0.0,
                0.0, 1.0,
                1.0, 1.0};

        const GLfloat normals[] =
            {
                // front
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f
            };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        {
            glGenBuffers(1, &vbo_position);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
            glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
            glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glGenBuffers(1, &vbo_texcoords);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
            glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_STATIC_DRAW);
            glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glGenBuffers(1, &vbo_normal);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
            glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
            glVertexAttribPointer(MATRIX_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MATRIX_ATTRIBUTE_NORMAL);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);

        return TRUE;
    }

    void drawQuadWithTexture(GLuint texture, mat4 modelMatrix, mat4 viewMatrix, mat4 perspectiveProjectionMatrix, float alpha, float repX = 1, float repY = 1, vec4 lightPosition = vec4(0.0f, 0.0f, 100.0f, 1.0f), bool applyToon = true, bool applySpecular = false, float shininess = 64.0f)
    {
        glUseProgram(shaderProgramObject);
        {
            // Uniforms
            glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
            glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
            glUniform2f(texRepeatUniform, repX, repY);
            glUniform1f(alphaValueUniform, alpha);
            glUniform1i(textureSamplerUniform, 0);
            glUniform4fv(lightPositionUniform, 1, lightPosition);
            glUniform1f(materialShininessUniform, shininess);
            glUniform1i(applyToonUniform, applyToon);

            glUniform1i(applySpecularUniform, applySpecular);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 4 Vertices for cube
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_BLEND);
        }
        glUseProgram(0);
    }
    void uninitialize(void)
    {
        UninitializeShaders(shaderProgramObject);
    }
};
