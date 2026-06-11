// *******************************
// YAMUNA RIVER SIDE SCENE WITH KRISHNA AND THEIR FRIENDS WITH SOME COWS
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
#include "../../utils/gltf/AnimatedModel.h"
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

class DemoScene2 : public Scene
{

public:
    CubeMap *cubeMap;
    GLuint cubeMapTexture;
    Terrain *terrain;
    // GLuint brdfLookUp;
    WaterMatrix *waterMatrix;

    // =========== Yamuna Side scene models variables with smart pointer ==========
    
    // static models
    std::unique_ptr<Core::Model> krishnaSleeping;
    std::unique_ptr<Core::Model> mangoTree;
    std::unique_ptr<Core::Model> stone1;
    std::unique_ptr<Core::Model> stone2;
    std::unique_ptr<Core::Model> krishnaFriend1;
    std::unique_ptr<Core::Model> krishnaFriend2;
    std::unique_ptr<Core::Model> balram;
    std::unique_ptr<Core::Model> cow1;
    std::unique_ptr<Core::Model> cow2;
    std::unique_ptr<Core::Model> cow3;
    std::unique_ptr<Core::Model> mKaliya;
    float mKaliya_yPos = -3000.0f;

    std::unique_ptr<Core::Model> tree1;


    // dynamic models
    std::unique_ptr<Core::AnimatedModel> eagle;

    // ==========================================================
    
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

    // Shadow Variables
    GLuint depthMapFBO;
    GLuint depthMapTexture;
    const GLuint SHADOW_WIDTH = 4096;
    const GLuint SHADOW_HEIGHT = 4096;
    mat4 lightSpaceMatrix;
    vec3 shadowLightPos = vec3(15500.0f, 4000.0f, -5500.0f);
    bool isDepthPass = false;

    void bindShadowUniforms(Core::Shader* shader) {
        if (!shader) return;
        shader->SetUniform("u_isDepthPass", isDepthPass);
        shader->SetUniform("u_lightSpaceMatrix", lightSpaceMatrix);
        if (!isDepthPass) {
            shader->SetUniform("u_enableShadow", true);
            shader->SetUniform("u_shadowLightPos", shadowLightPos);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, depthMapTexture);
            shader->SetUniform("u_shadowMap", 6);
        }
    }

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
    DemoScene2()
    {   
        cubeMap = new CubeMap();
        // cubeMap[1] = new CubeMap();
        terrain = new Terrain(40.0f * 100.0f);
        waterMatrix = new WaterMatrix(300. * 400.);
        sceneCamera = new BezierCamera();
        // rain = new Rain(40000);
        // godRaysShader = new GodRaysShader();
    }

    bool initialize()
    {  
        const char *facesLight2[] =
        {   
            ".\\assets\\textures\\modelCubeMap\\vrundavan\\px.png",
            ".\\assets\\textures\\modelCubeMap\\vrundavan\\nx.png",
            ".\\assets\\textures\\modelCubeMap\\vrundavan\\py.png",
            ".\\assets\\textures\\modelCubeMap\\vrundavan\\ny.png",
            ".\\assets\\textures\\modelCubeMap\\vrundavan\\pz.png",
            ".\\assets\\textures\\modelCubeMap\\vrundavan\\nz.png"
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

        // Create depth FBO
        glGenFramebuffers(1, &depthMapFBO);
        glGenTextures(1, &depthMapTexture);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // // Camera

        // Initializing GLB Model
        programStaticPBR = new glshaderprogram({"./src/shaders/modelgltf/pbrStatic.vert", "./src/shaders/modelgltf/pbrMain.frag"});

        // static model
        krishnaSleeping = std::make_unique<Core::Model>();
        krishnaSleeping->LoadModel("./assets/models/scene2_models/Krishna_sleeping.glb");

        mangoTree = std::make_unique<Core::Model>();
        mangoTree->LoadModel("./assets/models/scene2_models/tree1.glb");

        stone1 = std::make_unique<Core::Model>();
        stone1->LoadModel("./assets/models/scene2_models/stone1.glb");

        stone2 = std::make_unique<Core::Model>();
        stone2->LoadModel("./assets/models/scene2_models/stone2.glb");

        mKaliya = std::make_unique<Core::Model>();
        mKaliya->LoadModel("./assets/models/scene1_models/Kaliya_Final.glb");

        krishnaFriend1 = std::make_unique<Core::Model>();
        krishnaFriend1->LoadModel("./assets/models/scene2_models/krishnaFriend/friend1.glb");

        krishnaFriend2 = std::make_unique<Core::Model>();
        krishnaFriend2->LoadModel("./assets/models/scene2_models/krishnaFriend/friend2.glb");

        balram = std::make_unique<Core::Model>();
        balram->LoadModel("./assets/models/scene2_models/krishnaFriend/balram.glb");

        cow1 = std::make_unique<Core::Model>();
        cow1->LoadModel("./assets/models/scene2_models/cow1.glb");

        cow2 = std::make_unique<Core::Model>();
        cow2->LoadModel("./assets/models/scene2_models/cow2.glb");

        cow3 = std::make_unique<Core::Model>();
        cow3->LoadModel("./assets/models/scene2_models/cow3.glb");

        tree1 = std::make_unique<Core::Model>();
        tree1->LoadModel("./assets/models/scene2_models/cow3.glb");

        // dynamic model
        eagle = std::make_unique<Core::AnimatedModel>();
        eagle->LoadModel("./assets/models/scene2_models/eagle/eagle.fbx");
        eagle->SetBaseColorTexture("./assets/models/scene2_models/eagle/baseTexture.png");

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
        terrain->setFreq(0.010f);            // broad beach hills
        terrain->setDispFactor(15.595f);        // softer hill height
        terrain->setTessMultiplier(1.600f);
        terrain->setTextureTransitionFactor(1.0f); // use green texture set only
        terrain->setGrassCoverage(0.0f);     // >1.0 avoids grass branch
        terrain->setWaterHeight(190.0f);       // keep your sea level

        waterMatrix->interpolateWaterColor = 1.0f;
        //waterMatrix->moveFactor = 0.0f;

        // if (!rain->initialize(2))
        // {
        //     PrintLog("Failed to initialize Rain");
        // }

        // Event System
        sceneEvents = new EventManager(
            {{START_T, {0.0f, 37.0f}},
             {FADE_IN, {0.0f, 3.0f}},
             {SC_T1, {0.0f, 35.0f}},
             {FADE_OUT, {35.0f, 2.0f}},
             {END_T, {37.0f, 0.0f}}},
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
{15611.400391f, 763.500000f, -10064.500000f},
{15611.400391f, 823.500000f, -10064.500000f},
{15611.400391f, 923.500000f, -10064.500000f},
{15611.400391f, 1023.500000f, -10064.500000f},
{15611.400391f, 1823.500000f, -10064.500000f},
{15611.400391f, 2443.500000f, -10064.500000f},
{15611.400391f, 3663.500000f, -10064.500000f},
{15711.400391f, 4223.500000f, -8924.500000f},
{15711.400391f, 4223.500000f, -8064.500000f},
{15711.400391f, 4223.500000f, -4404.500000f},
{15711.400391f, 5043.500000f, -3664.500000f},
{15711.400391f, 6303.500000f, -2284.500000f},
{14911.400391f, 6303.500000f, -1504.500000f},
{12811.400391f, 6303.500000f, -1024.500000f},
{11471.400391f, 6303.500000f, -1024.500000f},
{9991.400391f, 6303.500000f, -1024.500000f},
{7311.400391f, 6303.500000f, 615.500000f},
{5071.400391f, 6303.500000f, 1455.500000f},
{811.400391f, 5183.500000f, 3195.500000f},
{-228.599609f, 5183.500000f, 3195.500000f},
{-2028.599609f, 5183.500000f, 3195.500000f},
{-2028.599609f, 4703.500000f, 3195.500000f},
{-2028.599609f, 4143.500000f, 3195.500000f},
{-2028.599609f, 2523.500000f, 3195.500000f},
{-4348.599609f, 2083.500000f, 4795.500000f},
{-4348.599609f, 923.500000f, 4795.500000f},
{-4348.599609f, 923.500000f, 4795.500000f},
};


// YAW GLOBAL
std::vector<float> yawGlobalSC1 = {
435.000000f,
435.000000f,
435.000000f,
435.000000f,
435.000000f,
435.000000f,
435.000000f,
438.000000f,
438.000000f,
438.000000f,
438.000000f,
463.000000f,
508.000000f,
498.000000f,
498.000000f,
498.000000f,
496.000000f,
498.000000f,
494.000000f,
490.000000f,
486.000000f,
486.000000f,
488.000000f,
491.000000f,
488.000000f,
488.000000f,
488.000000f,
};


// PITCH GLOBAL
std::vector<float> pitchGlobalSC1 = {
-2.000000f,
	-6.000000f,
	-13.000000f,
	-26.000000f,
	-49.000000f,
	-64.000000f,
	-68.000000f,
	-68.000000f,
	-74.000000f,
	-74.000000f,
	-74.000000f,
	-74.000000f,
	-74.000000f,
	-42.000000f,
	-30.000000f,
	-21.000000f,
	-21.000000f,
	-21.000000f,
	-19.000000f,
	-19.000000f,
	-19.000000f,
	-13.000000f,
	-11.000000f,
	-9.000000f,
	-9.000000f,
	-1.000000f,
	-1.000000f,
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
        // Shadow Depth Pass
        mat4 lightProjectionMatrix = vmath::ortho(-6000.0f, 6000.0f, -6000.0f, 6000.0f, -10000.0f, 10000.0f);
        mat4 lightViewMatrix = vmath::lookat(shadowLightPos, vec3(15500.0f, 0.0f, -9500.0f), vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

        isDepthPass = true;
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        
        drawYamunaSideScene();
        
        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        isDepthPass = false;

        glViewport(0, 0, giWindowWidth, giWindowHeight);

        // Camera
        modelMatrix = mat4::identity();
        perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)giWindowWidth / (GLfloat)giWindowHeight, 10.0f, 10000000.0f);

        // modelLoader.display();
        // sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal);
        // sceneCamera->update();

        pushMatrix(modelMatrix);
        {
            terrain->shadowMap = depthMapTexture;
            terrain->shadowLightSpaceMatrix = lightSpaceMatrix;
            terrain->shadowLightPosition = shadowLightPos;
            terrain->draw(1.0f);
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

        drawYamunaSideScene();

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

        sceneCamera->displayBezierCurve();
    }

    void drawYamunaSideScene()
    {   
        drawSleepingKrishna();
        drawMangoTree();
        drawFlyingEagle();
        drawStone1();
        drawStone2();
        drawKrishnaFriend1();
        drawKrishnaFriend2();
        drawBalram();
        drawCow1();
        drawCow2();
        drawCow3();
        drawKaliyaModel();
    }  

    // ==================== yamuna side scene models drawing functions ====================
    void drawKaliyaModel(bool isBlack = false)
    {
        if (!mKaliya)
            return;

        pushMatrix(modelMatrix);
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            mKaliya->mTextureShader->Use();
            bindShadowUniforms(mKaliya->mTextureShader.get());
            mKaliya->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 swingModelMatrix =
                vmath::translate(8000.0f + -18988.599609f, mKaliya_yPos + -96.500000f, -9000.0f + 23255.500000f)  *
                vmath::scale(1000.0f + 691.000000f, 1000.0f + 691.000000f, 1000.0f + 691.000000f) *
                vmath::rotate(90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            mKaliya->mTextureShader->SetUniform("u_model", swingModelMatrix);
            mKaliya->mTextureShader->SetUniform("u_view", viewMatrix);
            mKaliya->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            mKaliya->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);

            mKaliya->Draw(mKaliya->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    // static models function
    void drawSleepingKrishna(bool isBlack = false)
    {
        if (!krishnaSleeping)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            krishnaSleeping->mTextureShader->Use();
            bindShadowUniforms(krishnaSleeping->mTextureShader.get());
            krishnaSleeping->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 krishnaSleepingModelMatrix =
                vmath::translate(15880.0f, 650.0f, -9600.0f) *
                vmath::scale(5.0f, 5.0f, 5.0f) *
                vmath::rotate(50.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            krishnaSleeping->mTextureShader->SetUniform("u_model", krishnaSleepingModelMatrix);
            krishnaSleeping->mTextureShader->SetUniform("u_view", viewMatrix);
            krishnaSleeping->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            krishnaSleeping->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            krishnaSleeping->mTextureShader->SetUniform("u_ApplyToon", false); 

            krishnaSleeping->Draw(krishnaSleeping->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawMangoTree(bool isBlack = false)
    {
        if (!mangoTree)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            mangoTree->mTextureShader->Use();
            bindShadowUniforms(mangoTree->mTextureShader.get());
            mangoTree->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 mangoTreeModelMatrix =
                vmath::translate(15700.0f, 500.0f, -9700.0f) *
                vmath::scale(250.0f, 250.0f, 250.0f) *
                vmath::rotate(-40.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            mangoTree->mTextureShader->SetUniform("u_model", mangoTreeModelMatrix);
            mangoTree->mTextureShader->SetUniform("u_view", viewMatrix);
            mangoTree->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            mangoTree->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            mangoTree->mTextureShader->SetUniform("u_ApplyToon", false); 

            mangoTree->Draw(mangoTree->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawStone1(bool isBlack = false)
    {
        if (!stone1)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            stone1->mTextureShader->Use();
            bindShadowUniforms(stone1->mTextureShader.get());
            stone1->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 stone1ModelMatrix =
                vmath::translate(15000.0f, 400.0f, -9000.0f) *
                vmath::scale(2000.0f, 2000.0f, 2000.0f) *
                vmath::rotate(90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            stone1->mTextureShader->SetUniform("u_model", stone1ModelMatrix);
            stone1->mTextureShader->SetUniform("u_view", viewMatrix);
            stone1->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            stone1->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            stone1->mTextureShader->SetUniform("u_ApplyToon", false); 

            stone1->Draw(stone1->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawStone2(bool isBlack = false)
    {
        if (!stone2)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            stone2->mTextureShader->Use();
            bindShadowUniforms(stone2->mTextureShader.get());
            stone2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 stone2ModelMatrix =
                vmath::translate(16200.0f, 650.0f, -11000.0f) *
                vmath::scale(1.0f, 1.0f, 1.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            stone2->mTextureShader->SetUniform("u_model", stone2ModelMatrix);
            stone2->mTextureShader->SetUniform("u_view", viewMatrix);
            stone2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            stone2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            stone2->mTextureShader->SetUniform("u_ApplyToon", false); 

            stone2->Draw(stone2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawKrishnaFriend1(bool isBlack = false)
    {
         if (!krishnaFriend1)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            krishnaFriend1->mTextureShader->Use();
            bindShadowUniforms(krishnaFriend1->mTextureShader.get());
            krishnaFriend1->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 krishnaFriend1ModelMatrix =
                vmath::translate(15000.0f, 700.0f, -8840.0f) *
                vmath::scale(5.0f, 5.0f, 5.0f) *
                vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f);
                
                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            krishnaFriend1->mTextureShader->SetUniform("u_model", krishnaFriend1ModelMatrix);
            krishnaFriend1->mTextureShader->SetUniform("u_view", viewMatrix);
            krishnaFriend1->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            krishnaFriend1->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            krishnaFriend1->mTextureShader->SetUniform("u_ApplyToon", false); 

            krishnaFriend1->Draw(krishnaFriend1->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }
    
    void drawKrishnaFriend2(bool isBlack = false)
    {
         if (!krishnaFriend2)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            krishnaFriend2->mTextureShader->Use();
            bindShadowUniforms(krishnaFriend2->mTextureShader.get());
            krishnaFriend2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 krishnaFriend2ModelMatrix =
                vmath::translate(16000.0f, 650.0f, -9000.0f) *
                vmath::scale(90.0f, 90.0f, 90.0f) *
                vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f);
                
                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            krishnaFriend2->mTextureShader->SetUniform("u_model", krishnaFriend2ModelMatrix);
            krishnaFriend2->mTextureShader->SetUniform("u_view", viewMatrix);
            krishnaFriend2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            krishnaFriend2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            krishnaFriend2->mTextureShader->SetUniform("u_ApplyToon", false); 

            krishnaFriend2->Draw(krishnaFriend2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawBalram(bool isBlack = false)
    {
         if (!balram)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            balram->mTextureShader->Use();
            bindShadowUniforms(balram->mTextureShader.get());
            balram->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 balramModelMatrix =
                vmath::translate(15300.0f, 410.0f, -7000.0f) *
                vmath::scale(120.0f, 120.0f, 120.0f) *
                vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f);
                
                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            balram->mTextureShader->SetUniform("u_model", balramModelMatrix);
            balram->mTextureShader->SetUniform("u_view", viewMatrix);
            balram->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            balram->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            balram->mTextureShader->SetUniform("u_ApplyToon", false); 

            balram->Draw(balram->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawCow1(bool isBlack = false)
    {
         if (!cow1)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            cow1->mTextureShader->Use();
            bindShadowUniforms(cow1->mTextureShader.get());
            cow1->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 cow1ModelMatrix =
                vmath::translate(15000.0f, 190.0f, -2500.0f) *
                vmath::scale(2.0f, 2.0f, 2.0f) *
                vmath::rotate(120.0f, 0.0f, 1.0f, 0.0f);
                
                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            cow1->mTextureShader->SetUniform("u_model", cow1ModelMatrix);
            cow1->mTextureShader->SetUniform("u_view", viewMatrix);
            cow1->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            cow1->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            cow1->mTextureShader->SetUniform("u_ApplyToon", false); 

            cow1->Draw(cow1->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawCow2(bool isBlack = false)
    {
         if (!cow2)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            cow2->mTextureShader->Use();
            bindShadowUniforms(cow2->mTextureShader.get());
            cow2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 cow2ModelMatrix =
                vmath::translate(16500.0f, 120.0f, -3500.0f) *
                vmath::scale(2.0f, 2.0f, 2.0f) *
                vmath::rotate(20.0f, 0.0f, 1.0f, 0.0f);
                
                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            cow2->mTextureShader->SetUniform("u_model", cow2ModelMatrix);
            cow2->mTextureShader->SetUniform("u_view", viewMatrix);
            cow2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            cow2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            cow2->mTextureShader->SetUniform("u_ApplyToon", false); 

            cow2->Draw(cow2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawCow3(bool isBlack = false)
    {
         if (!cow3)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            cow3->mTextureShader->Use();
            bindShadowUniforms(cow3->mTextureShader.get());
            cow3->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 cow3ModelMatrix =
                vmath::translate(17000.0f, 190.0f, -2500.0f) *
                vmath::scale(2.0f, 2.0f, 2.0f) *
                vmath::rotate(-120.0f, 0.0f, 1.0f, 0.0f);
                
                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            cow3->mTextureShader->SetUniform("u_model", cow3ModelMatrix);
            cow3->mTextureShader->SetUniform("u_view", viewMatrix);
            cow3->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            cow3->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            cow3->mTextureShader->SetUniform("u_ApplyToon", false); 

            cow3->Draw(cow3->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }


    // ============================================================

    // dynamic models functions
    void drawFlyingEagle(bool isBlack = false)
    {
        if (!eagle)
            return;

        // Advance the skinned animation once per frame (drawn once from display()).
        if (!isDepthPass) {
            eagle->Update((float)gDeltaTime);
        }

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            eagle->mShader->Use();
            bindShadowUniforms(eagle->mShader.get());
            eagle->mShader->SetUniform("isBlack", isBlack);

            vmath::mat4 eagleModelMatrix =
                vmath::translate(16000.0f, 2000.0f, -9000.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f) *
                vmath::scale(20.0f, 20.0f, 20.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            eagle->mShader->SetUniform("u_model", eagleModelMatrix);
            eagle->mShader->SetUniform("u_view", viewMatrix);
            eagle->mShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            eagle->mShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            eagle->mShader->SetUniform("u_ApplyToon", false);
            eagle->mShader->SetSampler2D("u_GGXLUT", 0, 5);

            eagle->mShader->SetUniform("u_DebugMode", 0);

            eagle->Draw(eagle->mShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    // =======================================================

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
        // sceneCamera->update();

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

            float threshold = 10.0f / 26.0f;
            if (sceneCamera->time >= threshold) {
                // Kaliya comes up
                mKaliya_yPos += 5.0f; 
                if (mKaliya_yPos > 800.0f) mKaliya_yPos = 800.0f;
                
                // Water turns dark black/blue
                waterMatrix->interpolateWaterColor -= 0.002f;
                if (waterMatrix->interpolateWaterColor < 0.0f) waterMatrix->interpolateWaterColor = 0.0f;

                cubeMap->isBarasat = 2; // Dark clouds
            } else {
                mKaliya_yPos = -3000.0f;
                waterMatrix->interpolateWaterColor = 1.0f;
                cubeMap->isBarasat = 0; // Normal sky
            }
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

        krishnaSleeping.reset();
        mangoTree.reset();
        
        // // rain
        // if (rain->alpha > 0.0f)
        // {
        //     rain->alpha -= 0.002f;
        // }

        // modelLoader.uninitialize();
    }
};
