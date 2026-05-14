#pragma once
#include "../../utils/common.h"
#include "../../effects/cubemap/Cubemap.h"
#include "../../utils/camera/BezierCamera.h"
#include "../../includes/SphereAish.h"
#include "../../shaders/model/Model_Shader.h"
#include "../../shaders/shine/ShineShader.h"
#include "../../utils/gltf/Model.h"
#include "../../utils/gltf/TextureModel.h"  
#include "../../utils/EventManager.h"
#include "../../effects/terrain/Terrain.h"

#include "../../shaders/terrain/TerrainShader.h"
#include "../../shaders/godRays/GodRaysShader.h"
#include "../../effects/water_matrix/WaterMatrix.h"

#include "../Scene.h"

#define _DEBUG

class ToyScene1 : public Scene
{

public:
    CubeMap *cubemap;
    Terrain *terrain;

    WaterMatrix *waterMatrix;

    
    // Shaders
    GodRaysShader *godRaysShader;

    BezierCamera sc1;
    BezierCamera sc2;
    BezierCamera sc3;
    BezierCamera sc4;

    // Shaders
    ShineShader *shineShader;

    GLuint brdfLookUp;
    GLuint cubeMapTexture;

    std::unique_ptr<Core::Shader> mCubeMapShader;
    std::unique_ptr<Core::Model> mToyMarriage;
    std::unique_ptr<Core::Model> mBoy;
    std::unique_ptr<Core::Model> mRoom;
    std::unique_ptr<Core::Model> mCouple;
    std::unique_ptr<Core::Model> mCross;

    // EVENT
    enum sceneEventIds
    {
        START_T,
        FADE_IN,
        SC_T,
        SHINE_EYES,
        FADE_OUT,
        END_T,
    };

    GLuint texture_marriageStage;
    GLuint texture_layingBoy;

    // member functions
    ToyScene1()
    {   
        cubemap = new CubeMap();
        sceneCamera = new BezierCamera();
        shineShader = new ShineShader();
        
        terrain = new Terrain(20.0f * 80.0f);
        waterMatrix = new WaterMatrix(300. * 80.);

        godRaysShader = new GodRaysShader();
    }

    bool initialize()
    {
        // // Shaders
        shineShader->initialize();
        // // Toy room
        // //////////////////////////////////////////////////////// gltf model

        mToyMarriage = std::make_unique<Core::Model>();
        mToyMarriage->LoadModel("assets/models/scene1_models/KaliyaMardan.glb");

        
        mCross = std::make_unique<Core::Model>();
        mCross->LoadModel("assets/models/scene1_models/Kaliya.glb");

        if (!terrain)
        {
            PrintLog("Failed to initialize Terrain");
            return FALSE;
        }


        // Water
        waterMatrix->initialize();

        // Godrays initialization
        godRaysShader->initialize();

      /*   mBoy = std::make_unique<Core::Model>();
        // mBoy->LoadModel("./assets/models/scene1/boy/boy.glb");
        mBoy->LoadModel("./assets/models/scene1/boy/boy.glb");

        mRoom = std::make_unique<Core::Model>();
        mRoom->LoadModel("./assets/models/scene1/boyhouse.glb");


        brdfLookUp = Core::TextureModel::LoadTextureModel("./assets/textures/newbrdfLUT.png"); */

         // Cubemap
        const char *facesLight[] =
            {
            ".\\assets\\textures\\modelCubeMap\\px.jpg",
            ".\\assets\\textures\\modelCubeMap\\nx.jpg",
            ".\\assets\\textures\\modelCubeMap\\py.jpg",
            ".\\assets\\textures\\modelCubeMap\\ny.jpg",
            ".\\assets\\textures\\modelCubeMap\\pz.jpg",
            ".\\assets\\textures\\modelCubeMap\\nz.jpg"
            };

        if (!cubemap->initialize(facesLight))
        {
            PrintLog("Failed to initialize CubeMap");
            return FALSE;
        }

        std::vector<std::string> faces{
            ".\\assets\\textures\\modelCubeMap\\px.png",
            ".\\assets\\textures\\modelCubeMap\\nx.png",
            ".\\assets\\textures\\modelCubeMap\\py.png",
            ".\\assets\\textures\\modelCubeMap\\ny.png",
            ".\\assets\\textures\\modelCubeMap\\pz.png",
            ".\\assets\\textures\\modelCubeMap\\nz.png"};
        cubeMapTexture = Core::TextureModel::LoadCubeMapModel(faces);
        //////////////////////////////////////////////////////////////////////
        
        // Event System
        sceneEvents = new EventManager(
            {{START_T, {0.0f, 30.0f}},
             {FADE_IN, {0.0f, 3.0f}},
             {SC_T, {0.0f, 30.0f}},
             {SHINE_EYES, {20.0f, 10.0f}},
             {FADE_OUT, {27.0f, 3.0f}},
             {END_T, {30.0f, 0.0f}}},
            true);

        // Camera
        setupCamera();
        // sceneCamera->initialize();
        sceneCamera->handlePerspective = true;

      /*   if (LoadPNGImage(&texture_marriageStage, "./assets/textures/scene2/toy/shadowLagna.png") == FALSE)
        {
            PrintLog("Failed to load texture_marriageStage texture\n");
            return false;
        }

        if (LoadPNGImage(&texture_layingBoy, "./assets/textures/scene1/laying_boy.png") == FALSE)
        {
            PrintLog("Failed to load texture_layingBoy texture\n");
            return false;
        } */

        isInitialized = true;
        return TRUE;
    }

    void setupCamera()
    {
        ///////////////////////////////// SC1
        std::vector<std::vector<float>> bezierPointsSC1 = {
            {-9.000000f, 4.000000f, 1.000000f},
            {-9.000000f, 4.000000f, 1.000000f},
            {-9.000000f, 4.000000f, 1.000000f},
            {-9.000000f, 4.000000f, 5.000000f},
            {-9.000000f, 4.000000f, 12.000000f},
            {-9.000000f, 7.000000f, 16.000000f},
            {-9.000000f, 8.000000f, 20.000000f},
            {-11.000000f, 8.000000f, 20.000000f},
            {-14.000000f, 6.000000f, 21.000000f},
            {-17.000000f, 9.000000f, 22.000000f},
            {-22.000000f, 9.000000f, 22.000000f},
            {-23.000000f, 7.000000f, 16.000000f},
            {-27.000000f, 7.000000f, 11.000000f},
            {-23.000000f, 7.000000f, -2.000000f},
            {-20.000000f, 7.000000f, -4.000000f},
            {-15.000000f, 6.000000f, -6.000000f},
            {-11.000000f, 6.000000f, -6.000000f},
            {-2.000000f, 6.000000f, 1.000000f},
            {-2.000000f, 6.000000f, 1.000000f},
            {-2.000000f, 6.000000f, 1.000000f},
            {-2.000000f, 6.000000f, 1.000000f},
            {-2.000000f, 6.000000f, 1.000000f},
            {-2.000000f, 6.000000f, 2.000000f},
            {-2.000000f, 6.000000f, 2.000000f},
            {-2.000000f, 6.000000f, 3.000000f},
            {-2.000000f, 6.000000f, 3.000000f},
            {-2.000000f, 6.000000f, 4.000000f},
            {-2.000000f, 6.000000f, 4.000000f},
            {-2.000000f, 5.000000f, 5.000000f},
            {-2.000000f, 5.000000f, 5.000000f},
            {-2.000000f, 3.000000f, 5.000000f},
            {-2.000000f, 3.000000f, 5.000000f},
        };

        // YAW GLOBAL
        std::vector<float> yawGlobalSC1 = {
            -66.000000f,
            -66.000000f,
            -66.000000f,
            -67.000000f,
            -75.000000f,
            -75.000000f,
            -75.000000f,
            -75.000000f,
            -73.000000f,
            -60.000000f,
            -53.000000f,
            -47.000000f,
            -36.000000f,
            -16.000000f,
            7.000000f,
            45.000000f,
            77.000000f,
            154.000000f,
            154.000000f,
            154.000000f,
            156.000000f,
            156.000000f,
            157.000000f,
            159.000000f,
            164.000000f,
            164.000000f,
            169.000000f,
            169.000000f,
            177.000000f,
            177.000000f,
            177.000000f,
            177.000000f,
        };

        // PITCH GLOBAL
        std::vector<float> pitchGlobalSC1 = {
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -10.000000f,
            -10.000000f,
            -10.000000f,
            -10.000000f,
            -14.000000f,
            -14.000000f,
            -14.000000f,
            -14.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -6.000000f,
            -1.000000f,
            -1.000000f,
            9.000000f,
            22.000000f,
            22.000000f,
        };

        // FOV GLOBAL
        std::vector<float> fovGlobalSC1 = {
           1820.000000f,
            -1200.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -620.000000f,
            -1220.000000f,
            -1520.000000f,
            -1660.000000f,
            -1660.000000f,
            -1660.000000f,
            -1660.000000f,
            -1660.000000f,
            -1220.000000f,
            -1220.000000f,
            -820.000000f,
            -200.000000f,
            700.000000f,
            700.000000f,
            700.000000f,
        };
        sc1.initialize();
        sc1.setBezierPoints(bezierPointsSC1, yawGlobalSC1, pitchGlobalSC1, fovGlobalSC1);
        sc1.update();

        sceneCamera = &sc1;
    }

    void display()
    {
         // Camera
        modelMatrix = mat4::identity();
        perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)giWindowWidth / (GLfloat)giWindowHeight, 10.0f, 10000000.0f);

        // modelLoader.display();
        // sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal);

        // TERRAIN GODRAYS
        displayGodRays();

        // Fading
       /*  pushMatrix(modelMatrix);
        {   
            modelMatrix = vmath::scale(1.0f, 1.0f, 1.0f);
            if (sceneEvents->isEventInProgress(FADE_IN))
                commonShaders->overlayColorShader->draw(modelMatrix, 0.0f, 0.0f, 0.0f, lerp(sceneEvents->getEventTime(FADE_IN), 1.0f, 0.0f));
            else
                commonShaders->overlayColorShader->draw(modelMatrix, 0.0f, 0.0f, 0.0f, lerp(sceneEvents->getEventTime(FADE_OUT), 0.0f, 1.0f));
        }   
        modelMatrix = popMatrix(); */
            
        // displayScene(1.0);
        
        // sceneCamera->displayBezierCurve();
    }   

    void displayGodRays()
    {   
        // FIRST PASS
        glBindFramebuffer(GL_FRAMEBUFFER, godRaysShader->FBOscene);
        glClearBufferfv(GL_COLOR, 0, vec4(1.0f, 1.0f, 1.0f, 1.0f));
        glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, 1920, 1080);
        {   
            // DRAW TERRAIN IN BLACK
            pushMatrix(modelMatrix);
            {   
                // ==== this variable adjust terrain all properties for specific scene == added by prasad
                terrain->octaves = 10;
                terrain->frequency = 0.010f;
                terrain->grassCoverage = 0.716f;
                terrain->tessMultiplier = 1.625f;
                terrain->dispFactor = 20.226f;

                terrain->isGodRaysOcclusion = true;
                terrain->up = 1.0f;
                terrain->draw(false);
                terrain->isGodRaysOcclusion = false;
            }
            modelMatrix = popMatrix();

           
            // DRAW LIGHT SOURCE IN WHITE
            pushMatrix(modelMatrix);
            {   
                glUseProgram(commonShaders->colorShader->shaderProgramObject);

                vmath::mat4 translationMatrix = mat4::identity();
                vmath::mat4 rotateMatrix = mat4::identity();

                translationMatrix = vmath::translate(-18200.000000f + 6900.0f, 12400.000000f + -5400.0f, -1004.0f + 36800.000000f + 17400.0f);

                modelMatrix = translationMatrix * vmath::scale(50.0f + 188.502304f, 50.0f + 188.502304f, 50.0f + 188.502304f);

                glVertexAttrib3f(MATRIX_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
                glUniformMatrix4fv(commonShaders->colorShader->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
                glUniformMatrix4fv(commonShaders->colorShader->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
                glUniformMatrix4fv(commonShaders->colorShader->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

                glBindVertexArray(godRaysShader->vao_scene_sphere);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, godRaysShader->sphereAish->getNumberOfSphereVertices());
                glBindVertexArray(0);

                glUseProgram(0);
            }
            modelMatrix = popMatrix();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // SECOND PASS
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        {   
            pushMatrix(modelMatrix);
            {
                glUseProgram(commonShaders->colorShader->shaderProgramObject);

                vmath::mat4 translationMatrix = mat4::identity();
                vmath::mat4 rotateMatrix = mat4::identity();
            
                translationMatrix = vmath::translate(-18200.000000f + 6900.0f, 12400.000000f + -5400.0f, -1004.0f + 36800.000000f + 17400.0f);

                modelMatrix = translationMatrix * vmath::scale(50.0f + 188.502304f, 50.0f + 188.502304f, 50.0f + 188.502304f);

                glVertexAttrib3f(MATRIX_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
                glUniformMatrix4fv(commonShaders->colorShader->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
                glUniformMatrix4fv(commonShaders->colorShader->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
                glUniformMatrix4fv(commonShaders->colorShader->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

                glBindVertexArray(godRaysShader->vao_scene_sphere);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, godRaysShader->sphereAish->getNumberOfSphereVertices());
                glBindVertexArray(0);

                glUseProgram(0);
            }
            modelMatrix = popMatrix();
            
            // Reset modelMatrix before water FBO rendering
            modelMatrix = mat4::identity();

            // Water FBO
            {   
                // Reflection
                waterMatrix->bindReflectionFBO(1920, 1080);
                {
                    displayScene(1.0);
                }
                waterMatrix->unbindReflectionFBO();
                
                // Refraction
                waterMatrix->bindRefractionFBO(1920, 1080);
                {
                    displayScene(-1.0);
                }
                waterMatrix->unbindRefractionFBO();
            }   

            displayScene(1.0);

            drawToyMarriage();

            drawCross();
            
            // Water Bed
            pushMatrix(modelMatrix);
            {
                modelMatrix = modelMatrix * translate(0.0f, 200.900028f, 0.0f) * scale(1100.0f, 1.0f, 1100.0f);
                waterMatrix->renderWaterQuad(terrain->getWaterHeight());
            }
            modelMatrix = popMatrix();

          
            // cubemap
            pushMatrix(modelMatrix);
            {
                modelMatrix = modelMatrix * vmath::scale(500000.0f, 500000.0f, 500000.0f);
                cubemap->display();
            }
            modelMatrix = popMatrix();
        }

        // THIRD PASS
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glUseProgram(godRaysShader->shaderProgramObject_Godrays);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, godRaysShader->texture_fbo_scene);
        glUniform1f(0, 1.1f);
        glUniform1f(1, 0.06f);
        glUniform1f(2, 0.98f);
        glUniform1f(3, 0.5f);
        glUniform1i(4, 100);
        glUniform2fv(5, 1, setScreenSpaceCoords(perspectiveProjectionMatrix * viewMatrix * translate(-3.45f, -0.3f, 2.828f), vec4(-18200.000000f + 6900.0f, 12400.000000f + -5400.0f, -1004.0f + 36800.000000f + 17400.0f, 1.0f)));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glUseProgram(0);
        glDisable(GL_BLEND);
    }   

    vec2 setScreenSpaceCoords(const mat4 &vp, const vec4 &pos)
    {   
        auto transform = [](const mat4 &m, const vec4 &v) -> vec4
        {   
            vec4 out;
            out[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
            out[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
            out[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
            out[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
            return out;
        };
        vec4 ssPos = transform(vp, pos);
        float ssX = ssPos[0] / ssPos[3];
        float ssY = ssPos[1] / ssPos[3];

        // map ssX and ssY from [-1, 1] to [0, 1]
        ssX = ssX * 0.5f + 0.5f;
        ssY = ssY * 0.5f + 0.5f;
        return vec2(ssX, ssY);
    }

    void displayScene(float terrainUp)
    {   
        // Terrain
        displayTerrain(terrainUp);

        //drawPineTrees();
    }   
    
    void displayTerrain(float terrainUp)
    {       
        pushMatrix(modelMatrix);
        {   
            modelMatrix = vmath::scale(100000.0f,0.0f,100000.0f);
            terrain->up = terrainUp;
            terrain->draw(false);
        }   
        modelMatrix = popMatrix();
    }

    void drawRoom()
    {
        pushMatrix(modelMatrix);
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            static float anglecounter = 0.01f;

            // Use the shader program object
            mRoom->mTextureShader->Use();
            modelMatrix = vmath::translate(26.300064f, -3.299999f, 14.100018f);
            vmath::mat4 scaleMatrix = vmath::scale(1.0f, 1.0f, 1.0f);
            vmath::mat4 rotatationMatrix = vmath::rotate(0.0f, anglecounter, 0.0f);

            mRoom->mTextureShader->SetUniform("u_model", modelMatrix * rotatationMatrix * scaleMatrix);
            mRoom->mTextureShader->SetUniform("u_view", viewMatrix);
            mRoom->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            mRoom->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
            mRoom->mTextureShader->SetSamplerCubeMap("u_LambertianEnvSampler", cubeMapTexture, 6);
            mRoom->mTextureShader->SetSamplerCubeMap("u_GGXEnvSampler", cubeMapTexture, 7);
            mRoom->mTextureShader->exposure = 1.8;

            mRoom->Draw(mRoom->mTextureShader);
            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawToyMarriage()
    {   
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            static float anglecounter = 180.0f;

            // Use the shader program object
            mToyMarriage->mTextureShader->Use();
            modelMatrix = vmath::translate(objX + 2000.0f , objY + 5000.0f, objZ);
            vmath::mat4 scaleMatrix = vmath::scale(150.000000f, 150.000000f, 150.000000f);
            vmath::mat4 rotatationMatrix = vmath::rotate(0.0f, anglecounter, 0.0f);

            mToyMarriage->mTextureShader->SetUniform("u_model", modelMatrix * rotatationMatrix * scaleMatrix);
            mToyMarriage->mTextureShader->SetUniform("u_view", viewMatrix);
            mToyMarriage->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            mToyMarriage->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
            mToyMarriage->mTextureShader->SetSamplerCubeMap("u_LambertianEnvSampler", cubeMapTexture, 6);
            mToyMarriage->mTextureShader->SetSamplerCubeMap("u_GGXEnvSampler", cubeMapTexture, 7);

            mToyMarriage->Draw(mToyMarriage->mTextureShader);
            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawCross()
    {
        pushMatrix(modelMatrix);
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Use the shader program object
            mCross->mTextureShader->Use();
            modelMatrix = vmath::translate(-35.000000f, 2000.000000f, 11.000000f);
            vmath::mat4 scaleMatrix = vmath::scale(40.0f,40.0f,40.0f);
            vmath::mat4 rotatationMatrix = vmath::rotate(0.0f, 91.0f, 0.0f);

            mCross->mTextureShader->SetUniform("u_model", modelMatrix * rotatationMatrix * scaleMatrix);
            mCross->mTextureShader->SetUniform("u_view", viewMatrix);
            mCross->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            mCross->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
            mCross->mTextureShader->SetSamplerCubeMap("u_LambertianEnvSampler", cubeMapTexture, 6);
            mCross->mTextureShader->SetSamplerCubeMap("u_GGXEnvSampler", cubeMapTexture, 7);

            mCross->Draw(mCross->mTextureShader);
            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawGltfBabyModel()
    {
        pushMatrix(modelMatrix);
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            static float anglecounter = 0.01f;

            // Use the shader program object
            mBoy->mTextureShader->Use();
            modelMatrix = vmath::translate(-18.500000f, 0.000000f, 8.500000f);
            vmath::mat4 scaleMatrix = vmath::scale(4.299997f, 4.299997f, 4.299997f);
            vmath::mat4 rotatationMatrix = vmath::rotate(0.0f, -196.000000f, 0.0f);


            mBoy->mTextureShader->SetUniform("u_model", modelMatrix * rotatationMatrix * scaleMatrix);
            mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
            mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            mBoy->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
            mBoy->mTextureShader->SetSamplerCubeMap("u_LambertianEnvSampler", cubeMapTexture, 6);
            mBoy->mTextureShader->SetSamplerCubeMap("u_GGXEnvSampler", cubeMapTexture, 7);

            mBoy->mTextureShader->exposure = 1.5;

            mBoy->Draw(mBoy->mTextureShader);
            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        if (sceneCamera->time >= 0.8f)
        {
            // Left Eye
            pushMatrix(modelMatrix);
            {
                modelMatrix = modelMatrix * vmath::translate(-10.220003f, 5.389997f, 5.459996f) * vmath::scale(0.200000f, 0.200000f, 0.200000f) * vmath::rotate(0.0f, degToRad(90.0f) + 89.000000f, 0.0f);
                shineShader->drawQuad(modelMatrix, viewMatrix, perspectiveProjectionMatrix, lerp(sceneEvents->getEventTime(SHINE_EYES), 3.0, 10));
            }
            modelMatrix = popMatrix();

            // Right Eye
            pushMatrix(modelMatrix);
            {
                modelMatrix = modelMatrix * vmath::translate(-10.220003f - 0.710000f, 5.389997f + 0.190000f, 5.459996f + -1.189999f) * vmath::scale(0.200000f, 0.200000f, 0.200000f) * vmath::rotate(0.0f, degToRad(90.0f) + 89.000000f, 0.0f);
                shineShader->drawQuad(modelMatrix, viewMatrix, perspectiveProjectionMatrix, lerp(sceneEvents->getEventTime(SHINE_EYES), 3.0, 15));
            }
            modelMatrix = popMatrix();
        }
    }

    void update()
    {
        
         // sceneCamera->time = globalTime;
        if (terrain->getTextureTransitionFactor() < 1.0f)
        {
            terrain->setTextureTransitionFactor(terrain->getTextureTransitionFactor() + 0.001f);
        }

        if (terrain->getGrassCoverage() < 0.5f)
        {
            terrain->setGrassCoverage(terrain->getGrassCoverage() + 0.0005f);
        }

        /* // CAMERA UPDATE
        sceneCamera->time = sceneEvents->getEventTime(SC_T1);
        // sceneCamera->time = globalTime;
        sceneEvents->increment();

        // / Completing scene hereq
        if (sceneEvents->isEventComplete(END_T))
        {
            isSceneComplete = true;
        } */

        terrain->setWaterHeight(1000.0f - 20.000000f);
        waterMatrix->interpolateWaterColor = 1.0f;
        terrain->setTextureTransitionFactor(1.0f);
    }   

    void uninitialize()
    {
        if (cubemap)
        {
            cubemap->uninitialize();
            delete (cubemap);
            cubemap = NULL;
        }

        if (terrain)
        {
            delete terrain;
            terrain = nullptr;
        }

        // modelLoader.uninitialize();

        if (cubemap)
        {
            cubemap->uninitialize();
            delete (cubemap);
            cubemap = NULL;
        }
    }
};
