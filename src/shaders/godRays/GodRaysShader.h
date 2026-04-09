#pragma once
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLShaders.h"
#include "../../includes/vmath.h"
#include "../color/ColorShader.h"
#include "../../includes/SphereAish.h"

extern GLuint giWindowWidth;
extern GLuint giWindowHeight;
extern mat4 perspectiveProjectionMatrix;

using namespace vmath;
class GodRaysShader
{
public:
    // Variables
    int fboWidth = 1920;
    int fboHeight = 1080;

    // square related  *****DO NOT CHANGE****
    GLuint vao_square = 0;
    GLuint vbo_position_square = 0;
    GLuint vbo_texcoord_square = 0;

    GLuint mvpMatrixUniform_square = 0;
    GLuint sceneSamplerUniform_square = 0;
    GLuint godraysSamplerUniform_square = 0;

    // scene related vao and vbo
    GLuint vao_scene_sphere = 0;
    GLuint vbo_scene_position_sphere = 0;
    GLuint vbo_scene_texcoord_sphere = 0;
    GLuint vbo_scene_normal_sphere = 0;
    GLuint vbo_scene_element_sphere = 0;

    //***** Shaders for Godrays *****
    GLuint shaderProgramObject_Godrays = 0;
    GLuint occlusionTextureSamplerUniform_Godrays = 0;
    GLuint sceneTextureUniform_Godrays = 0;
    GLuint lightPositionUniform_Godrays = 0;

    GLuint decayUniform_godrays = 0;
    GLuint exposureUniform_godrays = 0;
    GLuint densityUniform_godrays = 0;
    GLuint weightUniform_godrays = 0;
    GLuint numSamplesUniform_godrays = 0;

    float sphere_vertices[1146];
    float sphere_normals[1146];
    float sphere_texcoords[764];
    unsigned short sphere_elements[2280];

    // FBO related global variables
    GLuint FBOscene = 0;
    GLuint RBOscene = 0; // render buffer object
    GLuint texture_fbo_scene = 0;
    BOOL bfboSceneResult = FALSE;

    GLuint FBOGodrays = 0;
    GLuint RBOGodrays = 0;
    GLuint texture_fbo_godrays = 0;
    BOOL bfboGodraysResult = FALSE;

    // input variables
    float lightPosX = 0.0f;
    float lightPosY = 0.0f;

    float decay = 0.9f;
    float exposure = 0.2f;
    float weight = 0.6f;
    float density = 0.9f;
    int numSample = 50;

    ColorShader colorShader;

    GLuint numSphereElements = 0;
    SphereAish *sphereAish;

    GodRaysShader(float decay = 0.9f, float exposure = 0.2f, float weight = 0.6f,
                  float density = 0.9f, int numSample = 50)
        : decay(decay), exposure(exposure), weight(weight),
          density(density), numSample(numSample)
    {
        sphereAish = new SphereAish(5.0, 100, 100);
    }

    BOOL initialize(void)
    {
        const GLfloat square_position[] = {
            // front
            1.0f, 1.0f, 0.0f,   // top-right of front
            -1.0f, 1.0f, 0.0f,  // top-left of front
            -1.0f, -1.0f, 0.0f, // bottom-left of front
            1.0f, -1.0f, 0.0f,  // bottom-right of front
        };

        const GLfloat square_texcoord[] =
            {
                // front
                1.0f, 1.0f, // top-right of front
                0.0f, 1.0f, // top-left of front
                0.0f, 0.0f, // bottom-left of front
                1.0f, 0.0f, // bottom-right of front
            };

        // cube
        //  vao
        glGenVertexArrays(1, &vao_square);
        glBindVertexArray(vao_square);

        // vbo for position
        glGenBuffers(1, &vbo_position_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);

        glBufferData(GL_ARRAY_BUFFER, sizeof(square_position), square_position, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vbo for texcoord
        glGenBuffers(1, &vbo_texcoord_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord_square);

        glBufferData(GL_ARRAY_BUFFER, sizeof(square_texcoord), square_texcoord, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        glBindVertexArray(0);

        // tell OpenglGL to enable the texture
        glEnable(GL_TEXTURE_2D);

        // fbo related code
        if (createFBO(fboWidth, fboHeight, &FBOscene, &RBOscene, &texture_fbo_scene) == TRUE)
        {
            bfboSceneResult = initialize_scene(fboWidth, fboHeight);
        }
        if (createFBO(fboWidth, fboHeight, &FBOGodrays, &RBOGodrays, &texture_fbo_godrays) == TRUE)
        {
            bfboGodraysResult = initialize_scene_Godrays(fboWidth, fboHeight);
        }

        return TRUE;
    }

    BOOL initialize_scene(GLint textureWidth, GLint textureHeight)
    {
        colorShader.initialize();

        // vao Sphere
        glGenVertexArrays(1, &vao_scene_sphere);
        glBindVertexArray(vao_scene_sphere);

        // position vbo
        glGenBuffers(1, &vbo_scene_position_sphere);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_scene_position_sphere);

        glBufferData(GL_ARRAY_BUFFER, sphereAish->getNumberOfSphereVertices() * sizeof(float), sphereAish->getSphereVertex(), GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // unbind vao
        glBindVertexArray(0);

        return TRUE;
    }

    BOOL initialize_scene_Godrays(GLint textureWidth, GLint textureHeight)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/godRays/godrays.vert", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/godRays/godrays.frag", FRAGMENT);

        shaderProgramObject_Godrays = glCreateProgram();
        glAttachShader(shaderProgramObject_Godrays, vertexShaderObject);
        glAttachShader(shaderProgramObject_Godrays, fragmentShaderObject);

        // link
        BOOL bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject_Godrays);

        if (bShaderLinkStatus == FALSE)
            return FALSE;
        return TRUE;
    }

    BOOL createFBO(GLint textureWidth, GLint textureHeight, GLuint *fbo, GLuint *rbo, GLuint *texture)
    {
        // Check Capacity of Render Buffer
        GLint maxRenderBufferSize = 0;
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);
        if (maxRenderBufferSize < textureWidth || maxRenderBufferSize < textureHeight)
        {
            PrintLog("Textrue Size OverFlow");
            return FALSE;
        }
        // Create Custom Frame Buffer
        glGenFramebuffers(1, fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
        // Create Texture for FBO in which we are going to render
        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, *texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,
                     0, // means take what ever is required
                     GL_RGB,
                     textureWidth,
                     textureHeight,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_SHORT_5_6_5, // 565 green is 6 as the eye is more compatibe/reacts to green spectrum more
                     NULL);

        // attach above texture to framebuffer color attachment zero
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);

        // create render buffer to hold depth of custom FBO
        glGenRenderbuffers(1, rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
        /*GL_DRAW_RENDERBUFFER - OUTPUT
        GL_READ_RENDERBUFFER - READING FROM RENDER BUFFER TRANSFORM RENDERBUFFER
        GL_RENDERBUFFER*/

        // set storage of above render buffer of texture size for depth
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, textureWidth, textureHeight);
        /*GL_DEPTH_COMPONENT16 - 16 works on mobile and windows
            but we can also use 24 here
            */
        // attach above depth related render buffer to fbo
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *rbo);

        // check frame buffer status whether successful or not
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            PrintLog("Framebuffer creation status is not complete.\n");
            return FALSE;
        }
        // unbind with frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return TRUE;
    }

    void uninitialize_scene(void)
    {
        // function declaration

        // code

        // cube
        //  delete vbo_position
        if (vbo_scene_element_sphere)
        {
            glDeleteBuffers(1, &vbo_scene_element_sphere);
            vbo_scene_element_sphere = 0;
        }
        if (vbo_scene_normal_sphere)
        {
            glDeleteBuffers(1, &vbo_scene_normal_sphere);
            vbo_scene_normal_sphere = 0;
        }

        if (vbo_scene_position_sphere)
        {
            glDeleteBuffers(1, &vbo_scene_position_sphere);
            vbo_scene_position_sphere = 0;
        }

        // delete vao
        if (vao_scene_sphere)
        {
            glDeleteVertexArrays(1, &vao_scene_sphere);
            vao_scene_sphere = 0;
        }
    }

    void uninitialize_scene_Godrays(void)
    {
        // function declaration

        // code

        if (shaderProgramObject_Godrays)
        {
            glUseProgram(shaderProgramObject_Godrays);

            GLint numShaders = 0;
            glGetProgramiv(shaderProgramObject_Godrays, GL_ATTACHED_SHADERS, &numShaders);

            if (numShaders > 0)
            {
                GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
                if (pShaders != NULL)
                {
                    glGetAttachedShaders(shaderProgramObject_Godrays, numShaders, NULL, pShaders);
                    for (GLint i = 0; i < numShaders; i++)
                    {
                        glDetachShader(shaderProgramObject_Godrays, pShaders[i]);
                        glDeleteShader(pShaders[i]);
                        pShaders[i] = 0;
                    }
                    free(pShaders);
                    pShaders = NULL;
                }
            }
            glUseProgram(0);
            glDeleteProgram(shaderProgramObject_Godrays);
            shaderProgramObject_Godrays = 0;
        }
    }

    void uninitialize_cube(void)
    {
        // function declaration
        void ToggleFullScreen(void);
        void uninitialize_scene_Occlusion(void);

        // code

        // cube
        //  //vbo_texture
        if (vbo_texcoord_square)
        {
            glDeleteBuffers(1, &vbo_texcoord_square);
            vbo_texcoord_square = 0;
        }
        // delete vbo_position
        if (vbo_position_square)
        {
            glDeleteBuffers(1, &vbo_position_square);
            vbo_position_square = 0;
        }

        // delete vao
        if (vao_square)
        {
            glDeleteVertexArrays(1, &vao_square);
            vao_square = 0;
        }

        PrintLog("****************************************************************************************************\n");
    }
};
