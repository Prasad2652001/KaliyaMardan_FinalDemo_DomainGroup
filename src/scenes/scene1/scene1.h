// *******************************
// vrundavan scene 
// *******************************
#pragma once
#include "../../utils/common.h"
#include "../../shaders/model/Model_Shader.h"
#include "../../effects/cubemap/Cubemap.h"
#include "../../effects/terrain/Terrain.h"
#include "../../effects/water_matrix/WaterMatrix.h"
#include "../../utils/camera/BezierCamera.h"
#include "../../utils/camera/Camera.h"

// model related headers
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


#define _DEBUG

extern Camera camera;
extern BezierCamera *globalBezierCamera;

class DemoScene1 : public Scene
{

public:
    CubeMap *cubeMap;
    GLuint cubeMapTexture;
    Terrain *terrain;
    // GLuint brdfLookUp;
    WaterMatrix *waterMatrix;
    // Rain *rain = NULL;

    // moment of kaliya nag variables for translation
    float kaliyaX = 7000.000f;
    float kaliyaZ = -16000.000f;

    // =========== vrundavan full scene models variables with smart pointer ==========
    
    std::unique_ptr<Core::Model> house1;
    std::unique_ptr<Core::Model> house2;
    std::unique_ptr<Core::Model> house3;
    std::unique_ptr<Core::Model> hutHouse;
    std::unique_ptr<Core::Model> vrundavanGate;
    std::unique_ptr<Core::Model> cowHouse;
    std::unique_ptr<Core::Model> well;
    std::unique_ptr<Core::Model> farmLand;

    // ==========================================================
   

    // ==========================================================

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
    DemoScene1()
    {   
        cubeMap = new CubeMap();
        // cubeMap[1] = new CubeMap();
        terrain = new Terrain(20.0f * 80.0f);
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

        // // Camera

        // Initializing GLB Model
        programStaticPBR = new glshaderprogram({"./src/shaders/modelgltf/pbrStatic.vert", "./src/shaders/modelgltf/pbrMain.frag"});
        
        // churchModel = new glmodel("./assets/models/scene2/church.glb", aiProcessPreset_TargetRealtime_Quality, true);
        // roadModel = new glmodel("./assets/models/scene2/road.glb", aiProcessPreset_TargetRealtime_Quality, true);

        cowHouse = std::make_unique<Core::Model>();
        cowHouse->LoadModel("./assets/models/scene1_models/vrundavan/cowHouse.glb");

        house1 = std::make_unique<Core::Model>();
        house1->LoadModel("./assets/models/scene1_models/vrundavan/house1.glb");

        house2 = std::make_unique<Core::Model>();
        house2->LoadModel("./assets/models/scene1_models/vrundavan/house2.glb");

        house3 = std::make_unique<Core::Model>();
        house3->LoadModel("./assets/models/scene1_models/vrundavan/house3.glb");

        vrundavanGate = std::make_unique<Core::Model>();
        vrundavanGate->LoadModel("./assets/models/scene1_models/vrundavan/vrundavanGate.glb");
        
        well = std::make_unique<Core::Model>();
        well->LoadModel("./assets/models/scene1_models/vrundavan/well.glb");

        farmLand = std::make_unique<Core::Model>();
        farmLand->LoadModel("./assets/models/scene1_models/vrundavan/farmLand.glb");

        hutHouse = std::make_unique<Core::Model>();
        hutHouse->LoadModel("./assets/models/scene1_models/vrundavan/hutHouse.glb");

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

        // IMPortanttt
        terrain->setFreq(0.020f);            // broad beach hills
        terrain->setDispFactor(15.595f);        // softer hill height
        terrain->setTessMultiplier(1.600f);
        terrain->setTextureTransitionFactor(1.0f); // use green texture set only
        terrain->setGrassCoverage(0.0f);     // >1.0 avoids grass branch
        terrain->setWaterHeight(-20.0f);       // keep your sea level

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
            waterMatrix->bindRefractionFBO(1920, 1080);
            {
                displayScene(-1.0);
            }
            waterMatrix->unbindRefractionFBO();

            // Refraction
            waterMatrix->bindReflectionFBO(1920, 1080);
            {
                displayScene(1.0);
            }
            waterMatrix->unbindReflectionFBO();
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

        drawVrundavanScene();

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

    void drawVrundavanScene()
    {
        drawHouse1();
        drawHouse2();      
        drawHouse3();
        drawVrundavanGate();
        drawCowHouse();
        drawFarmLand();
        drawHutHouse();
    }


    // ==================== vrundavan scene models drawing functions ====================
    
    void drawHouse1(bool isBlack = false)
    {
        if (!house1)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            house1->mTextureShader->Use();
            house1->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house1ModelMatrix =
                vmath::translate(-40.0f, 0.0f, 42.0f) *
                vmath::scale(30.0f, 20.0f, 30.0f) *
                vmath::rotate(-32.0f, 0.0f, 1.0f, 0.0f);

            // vmath::translate(kaliyaX, 600.000f, kaliyaZ) *
            // vmath::scale(1000.000f, 1000.000f, 1000.000f) *
            // vmath::rotate(130.000f, 0.000f, 1.000f, 0.000f);

            house1->mTextureShader->SetUniform("u_model", house1ModelMatrix);
            house1->mTextureShader->SetUniform("u_view", viewMatrix);
            house1->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            house1->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            house1->mTextureShader->SetUniform("u_ApplyToon", false); 

            house1->Draw(house1->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

         if (!house1)
            return;

        // draw same house with different scaling
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            house1->mTextureShader->Use();
            house1->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house1ModelMatrix =
                vmath::translate(633.0f, 0.0f, 220.0f) *
                vmath::scale(15.0f, 15.0f, 15.0f) *
                vmath::rotate(-0.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            house1->mTextureShader->SetUniform("u_model", house1ModelMatrix);
            house1->mTextureShader->SetUniform("u_view", viewMatrix);
            house1->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            house1->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            house1->mTextureShader->SetUniform("u_ApplyToon", false); 

            house1->Draw(house1->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawHouse2(bool isBlack = false)
    {
        if (!house2)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            house2->mTextureShader->Use();
            house2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house2ModelMatrix =
                vmath::translate(1110.0f, 50.0f, 500.0f) *
                vmath::scale(30.0f, 20.0f, 30.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            house2->mTextureShader->SetUniform("u_model", house2ModelMatrix);
            house2->mTextureShader->SetUniform("u_view", viewMatrix);
            house2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            house2->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            house2->mTextureShader->SetUniform("u_ApplyToon", false); 

            //house2->mTextureShader->exposure = 1.2f;
            house2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);

            house2->Draw(house2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawHutHouse(bool isBlack = false)
    {
        if (!hutHouse)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            hutHouse->mTextureShader->Use();
            hutHouse->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 hutHouseModelMatrix =
                // vmath::translate(1110.0f, 50.0f, 500.0f) *
                // vmath::scale(30.0f, 20.0f, 30.0f) *
                // vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            hutHouse->mTextureShader->SetUniform("u_model", hutHouseModelMatrix);
            hutHouse->mTextureShader->SetUniform("u_view", viewMatrix);
            hutHouse->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            hutHouse->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            hutHouse->mTextureShader->SetUniform("u_ApplyToon", false); 

            //hutHouse->mTextureShader->exposure = 1.2f;
            hutHouse->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);

            hutHouse->Draw(hutHouse->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawHouse3(bool isBlack = false)
    {   
        if (!house3)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            house3->mTextureShader->Use();
            house3->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house3ModelMatrix =
                vmath::translate(400.0f, 0.0f, 200.0f) *
                vmath::scale(50.0f, 30.0f, 50.0f) *
                vmath::rotate(68.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            house3->mTextureShader->SetUniform("u_model", house3ModelMatrix);
            house3->mTextureShader->SetUniform("u_view", viewMatrix);
            house3->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            house3->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            house3->mTextureShader->SetUniform("u_ApplyToon", false); 

            //house3->mTextureShader->exposure = 1.2f;
            house3->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);

            house3->Draw(house3->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawVrundavanGate(bool isBlack = false)
    {   
        if (!vrundavanGate)
            return;
        
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            vrundavanGate->mTextureShader->Use();
            vrundavanGate->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 vrundavanGateModelMatrix =
                vmath::translate(1500.0f, -220.0f, 1500.0f) *
                vmath::scale(10.0f, 10.0f, 10.0f) *
                vmath::rotate(-130.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            vrundavanGate->mTextureShader->SetUniform("u_model", vrundavanGateModelMatrix);
            vrundavanGate->mTextureShader->SetUniform("u_view", viewMatrix);
            vrundavanGate->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            //vrundavanGate->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            vrundavanGate->mTextureShader->SetUniform("u_ApplyToon", false); 

            //vrundavanGate->mTextureShader->exposure = 1.2f;
            vrundavanGate->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);

            vrundavanGate->Draw(vrundavanGate->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawCowHouse(bool isBlack = false)
    {
        if (!cowHouse)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            cowHouse->mTextureShader->Use();
            cowHouse->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 cowHouseModelMatrix =
                vmath::translate(1050.0f, 0.0f, 700.0f) *
                vmath::scale(10.0f, 10.0f, 10.0f) *
                vmath::rotate(70.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            cowHouse->mTextureShader->SetUniform("u_model", cowHouseModelMatrix);
            cowHouse->mTextureShader->SetUniform("u_view", viewMatrix);
            cowHouse->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            cowHouse->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            cowHouse->mTextureShader->SetUniform("u_ApplyToon", false); 

            cowHouse->Draw(cowHouse->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // draw well here
         if (!well)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            well->mTextureShader->Use();
            well->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 wellModelMatrix =
                vmath::translate(500.0f, 3.0f, 633.0f) *
                vmath::scale(1.0f, 1.0f, 1.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            well->mTextureShader->SetUniform("u_model", wellModelMatrix);
            well->mTextureShader->SetUniform("u_view", viewMatrix);
            well->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            well->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            well->mTextureShader->SetUniform("u_ApplyToon", false); 

            well->Draw(well->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawFarmLand(bool isBlack = false)
    {   
        if (!farmLand)
            return;
        
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            farmLand->mTextureShader->Use();
            farmLand->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 farmLandModelMatrix =
                vmath::translate(-533.0f, 20.0f, 232.0f) *
                vmath::scale(40.0f, 20.0f, 40.0f) *
                vmath::rotate(-30.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            farmLand->mTextureShader->SetUniform("u_model", farmLandModelMatrix);
            farmLand->mTextureShader->SetUniform("u_view", viewMatrix);
            farmLand->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            //farmLand->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            farmLand->mTextureShader->SetUniform("u_ApplyToon", false); 

            //farmLand->mTextureShader->exposure = 1.2f;
            farmLand->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);

            farmLand->Draw(farmLand->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    // ============================================================

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

        kaliyaX += 150.5f; // Move Kaliya along the X-axis
        kaliyaZ += 150.3f; // Move Kaliya along the Z-axis

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

        //mSwing.reset();

        house1.reset();
        house2.reset();
        house3.reset();
        vrundavanGate.reset();
        cowHouse.reset();
        farmLand.reset();

        // // rain
        // if (rain->alpha > 0.0f)
        // {
        //     rain->alpha -= 0.002f;
        // }

        // modelLoader.uninitialize();
    }
};
