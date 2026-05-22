//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// we will use this scene for showing their stay near the bank of the lake 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma once
#include "../../utils/common.h"
#include "../../shaders/model/Model_Shader.h"
#include "../../effects/cubemap/Cubemap.h"
#include "../../effects/terrain/Terrain.h"
#include "../../effects/water_matrix/WaterMatrix.h"
#include "../../utils/camera/BezierCamera.h"
#include "../../utils/camera/Camera.h"

// =========== kaliya mardan model related headers ===========
#include "../../utils/gltf/Model.h"
#include "../../utils/gltf/TextureModel.h"
#include "../../shaders/modelgltf/glshaderloader.h"
#include "../../shaders/modelgltf/glLight.h"
#include "../../shaders/modelgltf/glmodelloader.h"

// ===========================================================

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

class DemoScene0 : public Scene
{

public:
    CubeMap *cubeMap;
    GLuint cubeMapTexture;
    Terrain *terrain;
    // GLuint brdfLookUp;
    WaterMatrix *waterMatrix;
    // Rain *rain = NULL;

    // HowToLoadModel modelLoader;
    // std::unique_ptr<Core::Shader> mCubeMapShader;
    // std::unique_ptr<Core::Model> mRideScene;
    // std::unique_ptr<Core::Model> mChurch;
    // std::unique_ptr<Core::Model> mMarriageCar;
    // std::unique_ptr<Core::Model> mCross;

    // float carMovementX = 0.0f;
    // float carMovementZ = 0.0f;
    // bool fadingIn_Cube2 = true; // Track fade direction
    // bool fadeIn = true;
    // int iCurrentCubeMap = 0;

    // std::unique_ptr<Core::Model> mRoad;
    // std::unique_ptr<Core::Model> mTunnel;

    std::unique_ptr<Core::Model> mSwing;

    glshaderprogram *programStaticPBR;
    // glmodel *churchModel;
    // glmodel *roadModel;
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
    DemoScene0()
    {
        cubeMap = new CubeMap();
        // cubeMap[1] = new CubeMap();
        terrain = new Terrain(20. * 60.);
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
                ".\\assets\\textures\\modelCubeMap\\px.png",
                ".\\assets\\textures\\modelCubeMap\\nx.png",
                ".\\assets\\textures\\modelCubeMap\\py.png",
                ".\\assets\\textures\\modelCubeMap\\ny.png",
                ".\\assets\\textures\\modelCubeMap\\pz.png",
                ".\\assets\\textures\\modelCubeMap\\nz.png"
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

        mSwing = std::make_unique<Core::Model>();
        mSwing->LoadModel("./assets/models/scene1_models/Kaliya.glb");

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
        terrain->setFreq(0.031f);            // broad beach hills
        terrain->setDispFactor(13.000f);        // softer hill height
        terrain->setTessMultiplier(1.625f);
        terrain->setTextureTransitionFactor(1.0f); // use green texture set onl y
        terrain->setGrassCoverage(0.716f);     // >1.0 avoids grass branch
        terrain->setWaterHeight(400.0f);       // keep your sea level

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

     void drawSwingModel(bool isBlack = false)
    {
        if (!mSwing)
            return;

        pushMatrix(modelMatrix);
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            mSwing->mTextureShader->Use();
            mSwing->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 swingModelMatrix =
                vmath::translate(8000.0f, 800.0f, -9000.0f)  *
                vmath::scale(1000.0f, 1000.0f, 1000.0f) *
                vmath::rotate(90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            mSwing->mTextureShader->SetUniform("u_model", swingModelMatrix);
            mSwing->mTextureShader->SetUniform("u_view", viewMatrix);
            mSwing->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            mSwing->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);

            mSwing->Draw(mSwing->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
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

        drawSwingModel();

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

        mSwing.reset();

        // // rain
        // if (rain->alpha > 0.0f)
        // {
        //     rain->alpha -= 0.002f;
        // }

        // modelLoader.uninitialize();
    }
};
