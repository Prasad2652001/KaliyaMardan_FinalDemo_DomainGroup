//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// we will use this scene for showing beach sea
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma once
#include "../../utils/common.h"
#include "../../shaders/model/Model_Shader.h"
#include "../../effects/cubemap/Cubemap.h"
#include "../../effects/terrain/Terrain.h"
#include "../../effects/water_matrix/WaterMatrix.h"
#include "../../utils/camera/BezierCamera.h"
#include "../../utils/camera/Camera.h"
#include "../../utils/gltf/Model.h"
#include "../../utils/gltf/TextureModel.h"
#include "../../shaders/modelgltf/glshaderloader.h"
#include "../../shaders/modelgltf/glLight.h"
#include "../../shaders/modelgltf/glmodelloader.h"
#include "../../utils/EventManager.h"
#include "../Scene.h"

#include "../../utils/gltf/TextureModel.h"
#include "../../effects/terrain/Terrain.h"
#include "../../shaders/terrain/TerrainShader.h"
#include "../../shaders/godRays/GodRaysShader.h"
// #include "../../effects/rain/Rain.h"
// #include "../../shaders/rain/RainShader.h"

// #include "../../../scenes/howtoloadmodel/HowToLoadModel.h"

#define _DEBUG

extern Camera camera;
extern BezierCamera *globalBezierCamera;

extern float gModelTranslate[3];
extern float gModelRotate[3];
extern float gModelScale[3];

class DemoScene3 : public Scene
{

public:
    CubeMap *cubeMap;
    GLuint cubeMapTexture;
    Terrain *terrain;
    GLuint brdfLookUp = 0;
    WaterMatrix *waterMatrix;

    std::unique_ptr<Core::Model> mBoy;      // ship with flag
    std::unique_ptr<Core::Model> mBoy1;     // ship body wooden
    std::unique_ptr<Core::Model> mBoy2;     // zombie 
    std::unique_ptr<Core::Model> mBoy3;     // mansion 

    std::unique_ptr<Core::Model> mHouse;     // house 
    std::unique_ptr<Core::Model> mHouse1;     // house 
    std::unique_ptr<Core::Model> mHouse2;     // house 
    std::unique_ptr<Core::Model> mHouse3;     // house 
    std::unique_ptr<Core::Model> mHouse4;     // house 

    // Rain *rain = NULL;

    // std::unique_ptr<Core::Model> mRoad;
    // std::unique_ptr<Core::Model> mTunnel;

    glshaderprogram *programStaticPBR;
    SceneLight *lightManager;

    // Shaders
    GodRaysShader *godRaysShader;

    // Fadein Fadeout
    // float scaleFactor = 2.0f;

    // shadow
    // float maxShadowTranslate = 158.0f;
    // float shadowTranslate = 0.0f;

    BezierCamera sc1;
    BezierCamera sc2;
    BezierCamera sc3;

    // EVENT
    enum sceneEventIds
    {
        START_T,
        FADE_IN,
        SC_T1,
        FADE_OUT,
        END_T,
    };

    // member functions
    DemoScene3()
    {
        cubeMap = new CubeMap();
        // cubeMap[1] = new CubeMap();
        terrain = new Terrain(4. * 40.);
        waterMatrix = new WaterMatrix(300. * 400.);
        sceneCamera = new BezierCamera();
        // rain = new Rain(40000);
        // godRaysShader = new GodRaysShader();
    }

    bool initialize()
    {
        // const char *facesLight[] =
        //     {
        //         ".\\assets\\textures\\DayCubeMap\\px.png",
        //         ".\\assets\\textures\\DayCubeMap\\nx.png",
        //         ".\\assets\\textures\\DayCubeMap\\py.png",
        //         ".\\assets\\textures\\DayCubeMap\\ny.png",
        //         ".\\assets\\textures\\DayCubeMap\\pz.png",
        //         ".\\assets\\textures\\DayCubeMap\\nz.png"};
        // if (!cubeMap[1]->initialize(facesLight))
        // {
        //     PrintLog("Failed to initialize CubeMap");
        //     return FALSE;
        // }

        const char *facesLight2[] =
            {
                ".\\assets\\textures\\terrain\\cubemap_light\\px.png",
                ".\\assets\\textures\\terrain\\cubemap_light\\nx.png",
                ".\\assets\\textures\\terrain\\cubemap_light\\py.png",
                ".\\assets\\textures\\terrain\\cubemap_light\\ny.png",
                ".\\assets\\textures\\terrain\\cubemap_light\\pz.png",
                ".\\assets\\textures\\terrain\\cubemap_light\\nz.png"
            };
        if (!cubeMap->initialize(facesLight2))
        {
            PrintLog("Failed to initialize CubeMap");
            return FALSE;
        }

        if (!terrain)
        {
            PrintLog("Failed to initialize Terrain");
            return FALSE;
        }

        // ~~~~~~~~~~~~~~~~~~~~~~~~~model laoding paths~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        mBoy = std::make_unique<Core::Model>();
        mBoy->LoadModel("./assets/models/scene_3/ship.glb"); 
        
        mHouse = std::make_unique<Core::Model>();
        mHouse->LoadModel("./assets/models/scene_3/homeOne.glb"); 
        
        mHouse1 = std::make_unique<Core::Model>();
        mHouse1->LoadModel("./assets/models/scene_3/homeTwo1.glb"); 

        mHouse2 = std::make_unique<Core::Model>();
        mHouse2->LoadModel("./assets/models/scene_3/houseThree.glb"); 
        
        mHouse3 = std::make_unique<Core::Model>();
        mHouse3->LoadModel("./assets/models/scene_3/houseFour.glb"); 
       
        mHouse4 = std::make_unique<Core::Model>();
        mHouse4->LoadModel("./assets/models/scene_3/houseFive.glb"); 
        
        mBoy1 = std::make_unique<Core::Model>();
        mBoy1->LoadModel("./assets/models/scene_3/ship12.glb"); 
       
        mBoy2 = std::make_unique<Core::Model>();
        mBoy2->LoadModel("./assets/models/scene_3/final.glb"); 
        
        mBoy3 = std::make_unique<Core::Model>();
        mBoy3->LoadModel("./assets/models/scene_3/tree.glb"); 
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // DEFAULT MODEL TRANSFORM FOR Ship
        gModelTranslate[0] = 800.0f;
        gModelTranslate[1] = 95.0f;
        gModelTranslate[2] = -5000.0f;

        gModelRotate[0] = 0.0f;
        gModelRotate[1] = 0.0f;
        gModelRotate[2] = 0.0f;

        gModelScale[0] = 10.0f;
        gModelScale[1] = 10.0f;
        gModelScale[2] = 10.0f;
        // // Camera

        // Initializing GLB Model
        // programStaticPBR = new glshaderprogram({"./src/shaders/modelgltf/pbrStatic.vert", "./src/shaders/modelgltf/pbrMain.frag"});
        // churchModel = new glmodel("./assets/models/scene2/church.glb", aiProcessPreset_TargetRealtime_Quality, true);
        // roadModel = new glmodel("./assets/models/scene2/road.glb", aiProcessPreset_TargetRealtime_Quality, true);

        lightManager = new SceneLight();
        lightManager->addDirectionalLights({
            DirectionalLight(vec3(0.55f), 10.0f, vec3(0.0f, -1.0f, -1.0f)),
            DirectionalLight(vec3(0.55f), 10.0f, vec3(0.0f, -1.0f,  1.0f)),
            DirectionalLight(vec3(0.30f), 10.0f, vec3(0.0f, -1.0f,  0.0f))
        });
        lightManager->addSpotLights({
            SpotLight(vec3(1.0f), 10.0f, vec3(0.0f, 10.0f, 0.0f), 100.0f,
                      vec3(0.0f, -0.9f, -0.3f), 20.0f, 22.0f)
        });
        lightManager->setAmbient(vec3(0.05f));

        // Water
        waterMatrix->initialize();

        // IMPortantt
        // terrain->setWaterHeight(88.0f);
        // terrain->setTextureTransitionFactor(1.0f);
        // terrain->setGrassCoverage(0.75f);
        // main terrain
        terrain->setFreq(0.013f);            // broad beach hills
        terrain->setDispFactor(8.100f);        // softer hill height
        terrain->setTessMultiplier(1.0f);
        terrain->setTextureTransitionFactor(1.0f); // use green texture set only
        terrain->setGrassCoverage(0.5f);     // >1.0 avoids grass branch
        terrain->setWaterHeight(77.50f);       // keep your sea level
        // terrain->setScale(1.0f);
        // terrain->setOctaves(8);

        waterMatrix->interpolateWaterColor = 1.0f;
        waterMatrix->moveFactor = 0.0f;

        // if (!rain->initialize(2))
        // {
        //     PrintLog("Failed to initialize Rain");
        // }

        // Event System
        sceneEvents = new EventManager(
            {{START_T, {0.0f, 30.0f}},
             {FADE_IN, {0.0f, 3.0f}},
             {SC_T1, {0.0f, 28.0f}},
             {FADE_OUT, {28.0f, 2.0f}},
             {END_T, {30.0f, 0.0f}}},
            true);

        setupCamera();
        // sceneCamera->initialize();
        // sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal);
        // sceneCamera->handlePerspective = true;

        isInitialized = true;
        isSceneComplete = false;
        return TRUE;
    }

    void setupCamera()
    {
        std::vector<std::vector<float>> bezierPointsSC1 = {
            {761.399902f, 8713.500000f, -14994.500000f},
            {761.399902f, 8713.500000f, -14994.500000f},
            {-1648.600098f, 8713.500000f, -14994.500000f},
            {-3558.600098f, 8713.500000f, -14994.500000f},
            {-3558.600098f, 8713.500000f, -12814.500000f},
            {-3558.600098f, 8713.500000f, -9584.500000f},
            {-5008.600098f, 7733.500000f, -9584.500000f},
            {-5568.600098f, 7733.500000f, -6804.500000f},
            {-5568.600098f, 6433.500000f, -5954.500000f},
            {-5568.600098f, 6693.500000f, -5764.500000f},
            {-5568.600098f, 7063.500000f, -4514.500000f},
            {-5568.600098f, 7063.500000f, -2684.500000f},
            {-5568.600098f, 7063.500000f, -1564.500000f},
            {-5568.600098f, 7063.500000f, 1805.500000f},
            {-5568.600098f, 7063.500000f, 3915.500000f},
            {-5568.600098f, 7063.500000f, 5015.500000f},
        };

        // YAW GLOBAL
        std::vector<float> yawGlobalSC1 = {
            29.000000f,
            29.000000f,
            29.000000f,
            29.000000f,
            29.000000f,
            29.000000f,
            29.000000f,
            29.000000f,
            29.000000f,
            29.000000f,
            29.000000f,
            29.000000f,
            49.000000f,
            49.000000f,
            79.000000f,
            109.000000f,
        };

        // PITCH GLOBAL
        std::vector<float> pitchGlobalSC1 = {
            32.000000f,
            12.000000f,
            12.000000f,
            2.000000f,
            2.000000f,
            -8.000000f,
            2.000000f,
            2.000000f,
            2.000000f,
            2.000000f,
            -8.000000f,
            -8.000000f,
            -8.000000f,
            -8.000000f,
            -8.000000f,
            -8.000000f,
        };

        // FOV GLOBAL
        std::vector<float> fovGlobalSC1 = {
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
            -120.000000f,
        };

        sc1.initialize();
        sc1.setBezierPoints(bezierPointsSC1, yawGlobalSC1, pitchGlobalSC1, fovGlobalSC1);
        sc1.update();
    }
    void display()
    {
        // Camera
        modelMatrix = mat4::identity();
        perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)giWindowWidth / (GLfloat)giWindowHeight, 10.0f, 10000000.0f);

        // modelLoader.display();
        // sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal);
        // sceneCamera->update();

        pushMatrix(modelMatrix);
        {
            terrain->draw(false);
        }
        modelMatrix = popMatrix();

        pushMatrix(modelMatrix);
        {
            // Refraction
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
        modelMatrix = popMatrix();

        // Water Bed
        pushMatrix(modelMatrix);
        {
            modelMatrix = modelMatrix * translate(0.0f, (float)terrain->getWaterHeight() + 1.0f, 0.0f);
            waterMatrix->renderWaterQuad(terrain->getWaterHeight());
        }
        modelMatrix = popMatrix();

        pushMatrix(modelMatrix);
        {
            modelMatrix = modelMatrix * vmath::scale(1000000.0f, 1000000.0f, 1000000.0f);
            // Cubemap_Alpha = 1.0f;
            cubeMap->display();
        }
        modelMatrix = popMatrix();

         // Load Model
        if (mBoy)
        {
            pushMatrix(modelMatrix);
            {
            
                modelMatrix = modelMatrix *
                            vmath::translate(800.0f, 95.0f, -5000.0f) *
                            // vmath::rotate(0.0f, 1.0f, 0.0f, 0.0f) *
                            vmath::scale(10.0f, 10.0f, 10.0f);

                // vmath::mat4 modelMatrix =
                //     vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                //     vmath::rotate(gModelRotate[0], 1.0f, 0.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[1], 0.0f, 1.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[2], 0.0f, 0.0f, 1.0f) *
                //     vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]);

                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                mBoy->mTextureShader->Use();
                mBoy->mTextureShader->SetUniform("isBlack", false);
                
                mBoy->mTextureShader->SetUniform("u_model", modelMatrix);
                mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
                mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                // mBoy->mTextureShader->SetUniform("u_model", modelMatrix);
                // mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
                // mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                mBoy->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
                mBoy->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f)); // TODO set the light position  ... lightManager->getPointLightPosition()
                mBoy->mTextureShader->SetUniform("u_ApplyToon", true);
                mBoy->mTextureShader->SetUniform("u_ApplyRim", false);
                mBoy->mTextureShader->SetUniform("u_ApplySpecular", false);
                mBoy->Draw(mBoy->mTextureShader);
                glDisable(GL_BLEND);
            }
            modelMatrix = popMatrix();
        }

        if (mBoy1)
        {
            pushMatrix(modelMatrix);
            {
            
                modelMatrix = modelMatrix *
                            vmath::translate(497.0f, 97.0f, -5000.0f) *
                            vmath::rotate(0.0f, 0.0f, 00.0f, 0.0f) *
                            vmath::scale(1.0f, 1.0f, 1.0f);

                // vmath::mat4 modelMatrix =
                //     vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                //     vmath::rotate(gModelRotate[0], 1.0f, 0.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[1], 0.0f, 1.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[2], 0.0f, 0.0f, 1.0f) *
                //     vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]);

                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                mBoy1->mTextureShader->Use();
                mBoy1->mTextureShader->SetUniform("isBlack", false);
                
                mBoy1->mTextureShader->SetUniform("u_model", modelMatrix);
                mBoy1->mTextureShader->SetUniform("u_view", viewMatrix);
                mBoy1->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                // mBoy->mTextureShader->SetUniform("u_model", modelMatrix);
                // mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
                // mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                mBoy1->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
                mBoy1->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f)); // TODO set the light position  ... lightManager->getPointLightPosition()
                mBoy1->mTextureShader->SetUniform("u_ApplyToon", true);
                mBoy1->mTextureShader->SetUniform("u_ApplyRim", false);
                mBoy1->mTextureShader->SetUniform("u_ApplySpecular", false);
                mBoy1->Draw(mBoy1->mTextureShader);
                glDisable(GL_BLEND);
            }
            modelMatrix = popMatrix();
        }

        if (mBoy2)
        {
            pushMatrix(modelMatrix);
            {
            
                modelMatrix = modelMatrix *
                            vmath::translate(400.0f, 120.0f, -5000.0f) *
                            vmath::rotate(-90.0f, 1.0f, 0.0f, 0.0f) *
                            vmath::scale(50.0f, 50.0f, 50.0f);

                // vmath::mat4 modelMatrix =
                //     vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                //     vmath::rotate(gModelRotate[0], 1.0f, 0.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[1], 0.0f, 1.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[2], 0.0f, 0.0f, 1.0f) *
                //     vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]);

                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                mBoy2->mTextureShader->Use();
                mBoy2->mTextureShader->SetUniform("isBlack", false);
                
                mBoy2->mTextureShader->SetUniform("u_model", modelMatrix);
                mBoy2->mTextureShader->SetUniform("u_view", viewMatrix);
                mBoy2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                // mBoy->mTextureShader->SetUniform("u_model", modelMatrix);
                // mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
                // mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                mBoy2->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
                mBoy2->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f)); // TODO set the light position  ... lightManager->getPointLightPosition()
                mBoy2->mTextureShader->SetUniform("u_ApplyToon", true);
                mBoy2->mTextureShader->SetUniform("u_ApplyRim", false);
                mBoy2->mTextureShader->SetUniform("u_ApplySpecular", false);
                mBoy2->Draw(mBoy2->mTextureShader);
                glDisable(GL_BLEND);
            }
            modelMatrix = popMatrix();
        }

        if (mBoy3)
        {
            pushMatrix(modelMatrix);
            {
            
                modelMatrix = modelMatrix *
                            vmath::translate(5000.0f, 95.0f, -5000.0f) *
                            vmath::rotate(0.0f, 0.0f, 0.0f, 0.0f) *
                            vmath::scale(60.0f, 60.0f, 60.0f);

                // vmath::mat4 modelMatrix =
                //     vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                //     vmath::rotate(gModelRotate[0], 1.0f, 0.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[1], 0.0f, 1.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[2], 0.0f, 0.0f, 1.0f) *
                //     vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]);

                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                mBoy3->mTextureShader->Use();
                mBoy3->mTextureShader->SetUniform("isBlack", false);
                
                mBoy3->mTextureShader->SetUniform("u_model", modelMatrix);
                mBoy3->mTextureShader->SetUniform("u_view", viewMatrix);
                mBoy3->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                // mBoy->mTextureShader->SetUniform("u_model", modelMatrix);
                // mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
                // mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                mBoy3->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
                mBoy3->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f)); // TODO set the light position  ... lightManager->getPointLightPosition()
                mBoy3->mTextureShader->SetUniform("u_ApplyToon", true);
                mBoy3->mTextureShader->SetUniform("u_ApplyRim", false);
                mBoy3->mTextureShader->SetUniform("u_ApplySpecular", false);
                mBoy3->Draw(mBoy3->mTextureShader);
                glDisable(GL_BLEND);
            }
            modelMatrix = popMatrix();
        }

        // jule trees
        if (mBoy3)
        {
            pushMatrix(modelMatrix);
            {
            
                modelMatrix = modelMatrix *
                            vmath::translate(4090.0f, 95.0f, -5000.0f) *
                            vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f) *
                            vmath::scale(60.0f, 60.0f, 60.0f);

                // vmath::mat4 modelMatrix =
                //     vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                //     vmath::rotate(gModelRotate[0], 1.0f, 0.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[1], 0.0f, 1.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[2], 0.0f, 0.0f, 1.0f) *
                //     vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]);

                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                mBoy3->mTextureShader->Use();
                mBoy3->mTextureShader->SetUniform("isBlack", false);
                
                mBoy3->mTextureShader->SetUniform("u_model", modelMatrix);
                mBoy3->mTextureShader->SetUniform("u_view", viewMatrix);
                mBoy3->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                // mBoy->mTextureShader->SetUniform("u_model", modelMatrix);
                // mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
                // mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                mBoy3->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
                mBoy3->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f)); // TODO set the light position  ... lightManager->getPointLightPosition()
                mBoy3->mTextureShader->SetUniform("u_ApplyToon", true);
                mBoy3->mTextureShader->SetUniform("u_ApplyRim", false);
                mBoy3->mTextureShader->SetUniform("u_ApplySpecular", false);
                mBoy3->Draw(mBoy3->mTextureShader);
                glDisable(GL_BLEND);
            }
            modelMatrix = popMatrix();
        }

        if (mHouse)
        {
            pushMatrix(modelMatrix);
            {
            
                modelMatrix = modelMatrix *
                            vmath::translate(5050.0f, 150.0f, -5500.0f) *
                            vmath::rotate(0.0f, 0.0f, 0.0f, 0.0f) *
                            vmath::scale(50.0f, 50.0f, 50.0f);

                // vmath::mat4 modelMatrix =
                //     vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                //     vmath::rotate(gModelRotate[0], 1.0f, 0.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[1], 0.0f, 1.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[2], 0.0f, 0.0f, 1.0f) *
                //     vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]);

                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                mHouse->mTextureShader->Use();
                mHouse->mTextureShader->SetUniform("isBlack", false);
                
                mHouse->mTextureShader->SetUniform("u_model", modelMatrix);
                mHouse->mTextureShader->SetUniform("u_view", viewMatrix);
                mHouse->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                // mBoy->mTextureShader->SetUniform("u_model", modelMatrix);
                // mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
                // mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                mHouse->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
                mHouse->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f)); // TODO set the light position  ... lightManager->getPointLightPosition()
                mHouse->mTextureShader->SetUniform("u_ApplyToon", true);
                mHouse->mTextureShader->SetUniform("u_ApplyRim", false);
                mHouse->mTextureShader->SetUniform("u_ApplySpecular", false);
                mHouse->Draw(mHouse->mTextureShader);
                glDisable(GL_BLEND);
            }
            modelMatrix = popMatrix();
        }

        if (mHouse1)
        {
            pushMatrix(modelMatrix);
            {
            
                modelMatrix = modelMatrix *
                            vmath::translate(5500.0f, 150.0f, -5500.0f) *
                            vmath::rotate(0.0f, 0.0f, 0.0f, 0.0f) *
                            vmath::scale(50.0f, 50.0f, 50.0f);

                // vmath::mat4 modelMatrix =
                //     vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                //     vmath::rotate(gModelRotate[0], 1.0f, 0.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[1], 0.0f, 1.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[2], 0.0f, 0.0f, 1.0f) *
                //     vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]);

                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                mHouse1->mTextureShader->Use();
                mHouse1->mTextureShader->SetUniform("isBlack", false);
                
                mHouse1->mTextureShader->SetUniform("u_model", modelMatrix);
                mHouse1->mTextureShader->SetUniform("u_view", viewMatrix);
                mHouse1->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                // mBoy->mTextureShader->SetUniform("u_model", modelMatrix);
                // mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
                // mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                mHouse1->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
                mHouse1->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f)); // TODO set the light position  ... lightManager->getPointLightPosition()
                mHouse1->mTextureShader->SetUniform("u_ApplyToon", true);
                mHouse1->mTextureShader->SetUniform("u_ApplyRim", false);
                mHouse1->mTextureShader->SetUniform("u_ApplySpecular", false);
                mHouse1->Draw(mHouse1->mTextureShader);
                glDisable(GL_BLEND);
            }
            modelMatrix = popMatrix();
        }

        if (mHouse2)
        {
            pushMatrix(modelMatrix);
            {
            
                modelMatrix = modelMatrix *
                            vmath::translate(3800.0f, 170.0f, -5800.0f) *
                            vmath::rotate(0.0f, 0.0f, 0.0f, 0.0f) *
                            vmath::scale(0.250f, 0.250f, 0.250f);

                // vmath::mat4 modelMatrix =
                //     vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                //     vmath::rotate(gModelRotate[0], 1.0f, 0.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[1], 0.0f, 1.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[2], 0.0f, 0.0f, 1.0f) *
                //     vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]);

                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                mHouse2->mTextureShader->Use();
                mHouse2->mTextureShader->SetUniform("isBlack", false);
                
                mHouse2->mTextureShader->SetUniform("u_model", modelMatrix);
                mHouse2->mTextureShader->SetUniform("u_view", viewMatrix);
                mHouse2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                // mBoy->mTextureShader->SetUniform("u_model", modelMatrix);
                // mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
                // mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                mHouse2->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
                mHouse2->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f)); // TODO set the light position  ... lightManager->getPointLightPosition()
                mHouse2->mTextureShader->SetUniform("u_ApplyToon", true);
                mHouse2->mTextureShader->SetUniform("u_ApplyRim", false);
                mHouse2->mTextureShader->SetUniform("u_ApplySpecular", false);
                mHouse2->Draw(mHouse2->mTextureShader);
                glDisable(GL_BLEND);
            }
            modelMatrix = popMatrix();
        }

        if (mHouse3)
        {
            pushMatrix(modelMatrix);
            {
            
                modelMatrix = modelMatrix *
                            vmath::translate(3950.0f, 300.0f, -5400.0f) *
                            vmath::rotate(0.0f, 0.0f, 0.0f, 0.0f) *
                            vmath::scale(250.0f, 250.0f, 250.0f);

                // vmath::mat4 modelMatrix =
                //     vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                //     vmath::rotate(gModelRotate[0], 1.0f, 0.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[1], 0.0f, 1.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[2], 0.0f, 0.0f, 1.0f) *
                //     vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]);

                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                mHouse3->mTextureShader->Use();
                mHouse3->mTextureShader->SetUniform("isBlack", false);
                
                mHouse3->mTextureShader->SetUniform("u_model", modelMatrix);
                mHouse3->mTextureShader->SetUniform("u_view", viewMatrix);
                mHouse3->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                // mBoy->mTextureShader->SetUniform("u_model", modelMatrix);
                // mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
                // mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                mHouse3->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
                mHouse3->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f)); // TODO set the light position  ... lightManager->getPointLightPosition()
                mHouse3->mTextureShader->SetUniform("u_ApplyToon", true);
                mHouse3->mTextureShader->SetUniform("u_ApplyRim", false);
                mHouse3->mTextureShader->SetUniform("u_ApplySpecular", false);
                mHouse3->Draw(mHouse3->mTextureShader);
                glDisable(GL_BLEND);
            }
            modelMatrix = popMatrix();
        }
        
        if (mHouse4)
        {
            pushMatrix(modelMatrix);
            {
            
                modelMatrix = modelMatrix *
                            vmath::translate(4900.0f, 150.0f, -6300.0f) *
                            vmath::rotate(0.0f, 0.0f, 0.0f, 0.0f) *
                            vmath::scale(50.0f, 50.0f, 50.0f);

                // vmath::mat4 modelMatrix =
                //     vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                //     vmath::rotate(gModelRotate[0], 1.0f, 0.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[1], 0.0f, 1.0f, 0.0f) *
                //     vmath::rotate(gModelRotate[2], 0.0f, 0.0f, 1.0f) *
                    // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]);

                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                mHouse4->mTextureShader->Use();
                mHouse4->mTextureShader->SetUniform("isBlack", false);
                
                mHouse4->mTextureShader->SetUniform("u_model", modelMatrix);
                mHouse4->mTextureShader->SetUniform("u_view", viewMatrix);
                mHouse4->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                // mBoy->mTextureShader->SetUniform("u_model", modelMatrix);
                // mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
                // mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                mHouse4->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
                mHouse4->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f)); // TODO set the light position  ... lightManager->getPointLightPosition()
                mHouse4->mTextureShader->SetUniform("u_ApplyToon", true);
                mHouse4->mTextureShader->SetUniform("u_ApplyRim", false);
                mHouse4->mTextureShader->SetUniform("u_ApplySpecular", false);
                mHouse4->Draw(mHouse4->mTextureShader);
                glDisable(GL_BLEND);
            }
            modelMatrix = popMatrix();
        }

        // RAIN RENDERING
        // pushMatrix(modelMatrix);
        // {
        //     // modelMatrix = modelMatrix * translate(0.0f, -35.0f, -5.0f) * scale(1.0f,1.0f,1.0f);
        //     if (rain->alpha > 0.0f)
        //     {
        //         drawRain();
        //     }
        // }
        // modelMatrix = popMatrix();

        // sceneCamera->displayBezierCurve();
    }

    // RAIN RELATED
    // void drawRain(void)
    // {
    //     // code
    //     pushMatrix(modelMatrix);
    //     {
    //         rain->lightAmbient[0] = 0.0f;
    //         rain->lightAmbient[1] = 0.0f;
    //         rain->lightAmbient[2] = 0.0f;
    //         rain->lightAmbient[3] = 1.0f;

    //         rain->lightDiffuse[0] = 1.0f;
    //         rain->lightDiffuse[1] = 1.0f;
    //         rain->lightDiffuse[2] = 1.0f;
    //         rain->lightDiffuse[3] = 1.0f;

    //         rain->lightPosition[0] = 0.0f;
    //         rain->lightPosition[1] = 100.0f;
    //         rain->lightPosition[2] = -30.0f;
    //         rain->lightPosition[3] = 1.0f;

    //         rain->lightSpecular[0] = 1.0f;
    //         rain->lightSpecular[1] = 1.0f;
    //         rain->lightSpecular[2] = 1.0f;
    //         rain->lightSpecular[3] = 1.0f;

    //         // depth buffer madhe writing disable karnya sathi

    //         glEnable(GL_BLEND);

    //         rain->display();
    //         glDisable(GL_BLEND);
    //     }
    //     modelMatrix = popMatrix();
    // }
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    void displayScene(float terrainUp)
    {
        pushMatrix(modelMatrix);
        {
            terrain->up = terrainUp;
            terrain->draw(false);
        }
        modelMatrix = popMatrix();

        pushMatrix(modelMatrix);
        {
            modelMatrix = modelMatrix * vmath::scale(1000000.0f, 1000000.0f, 1000000.0f);
            cubeMap->display();
        }
        modelMatrix = popMatrix();
    }

    void displayTerrain(float terrainUp)
    {
        // Terrain
        pushMatrix(modelMatrix);
        {
            terrain->up = terrainUp;
            terrain->draw(false);
        }
        modelMatrix = popMatrix();
    }

    void update()
    {
        sceneCamera->time = globalTime;

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // terrain->setWaterHeight(85.0f);
        // waterMatrix->interpolateWaterColor = 1.0f;
        // terrain->setTextureTransitionFactor(1.0f);
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


        // if (terrain->getTextureTransitionFactor() < 1.0f)
        // {
        //     terrain->setTextureTransitionFactor(terrain->getTextureTransitionFactor() + 0.001f);
        // }

        // if (terrain->getGrassCoverage() < 0.5f)
        // {
        //     terrain->setGrassCoverage(terrain->getGrassCoverage() + 0.005f);
        // }

        // CAMERA UPDATE
        sceneCamera->time = sceneEvents->getEventTime(START_T);
        sceneEvents->increment();

        // CAMERA UPDATE
        if (sceneEvents->isEventInProgress(SC_T1))
        {
            sceneCamera = &sc1;
            sceneCamera->time = sceneEvents->getEventTime(SC_T1);
        }
        
        if (sceneEvents->isEventComplete(END_T))
            isSceneComplete = true;

        // terrain->setWaterHeight(100.0f - 15.000000f);
        // waterMatrix->interpolateWaterColor = 1.0f;
        // terrain->setTextureTransitionFactor(1.0f);

        float fadeSpeed = 0.001f; // Adjust speed for smooth fade effect

        // if (fadeIn)
        // {
        //     Cubemap_Alpha += fadeSpeed;
        //     if (Cubemap_Alpha >= 1.0f)
        //     {
        //         Cubemap_Alpha = 1.0f;
        //         // fadeIn = false; // Start fading out
        //     }
        // }
        // else
        // {
        //     Cubemap_Alpha -= fadeSpeed;
        //     if (Cubemap_Alpha <= 0.0f)
        //     {
        //         Cubemap_Alpha = 0.0f;
        //         fadeIn = true; // Start fading in the next texture
        //         iCurrentCubeMap += 1;
        //     }
        // }
    }

    void uninitialize()
    {
        if (cubeMap)
        {
            cubeMap->uninitialize();
            delete (cubeMap);
            cubeMap = NULL;
        }

        if (terrain)
        {
            delete terrain;
            terrain = nullptr;
        }

        // // rain
        // if (rain->alpha > 0.0f)
        // {
        //     rain->alpha -= 0.002f;
        // }

        // modelLoader.uninitialize();
    }
};
