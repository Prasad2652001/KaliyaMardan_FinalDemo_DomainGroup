// *******************************
// Kaliya Mardan scene 
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
#include "../../effects/rain/Rain.h"
#include "../../effects/fire/Fire.h"
#include "../../shaders/overlayColor/OverlayColorShader.h"
#include "../../effects/bloom/Bloom_Shaders.h"


#define _DEBUG

extern Camera camera;
extern BezierCamera *globalBezierCamera;
extern DOUBLE gDeltaTime;

extern float gModelTranslate[3];
extern float gModelRotate[3];
extern float gModelScale[3];

class DemoScene3 : public Scene
{

public:
    CubeMap *cubeMap;
    GLuint cubeMapTexture;
    Terrain *terrain;
    // GLuint brdfLookUp;
    WaterMatrix *waterMatrix;
    Rain *rain = NULL;
    Fire *fireEffect = NULL;
    OverlayColorShader *blackQuad = NULL;
    BloomShaders bloomEffect;
    float modelsRiseY = -600.0f; // Start underwater
    float modelsAlpha = 0.0f;
    float sceneFadeAlpha = 0.0f;
    float fireAlpha = 0.0f;

    // moment of kaliya nag variables for translation
    float kaliyaX = 7000.000f;
    float kaliyaZ = -16000.000f;

    // =========== kaliya mardan full scene models variables with smart pointer ==========
    
    std::unique_ptr<Core::Model> house1;
    std::unique_ptr<Core::Model> house2;
    std::unique_ptr<Core::Model> house3;
    std::unique_ptr<Core::Model> hutHouse;
    std::unique_ptr<Core::AnimatedModel> vrundavanGate;
    std::unique_ptr<Core::AnimatedModel> shreeKrishna;

    // Eight Krishna Tandav "position" models cross-faded to animate the dance
    // (fade in / fade out between static poses, sequenced 1..8 on a loop).
    static const int KRISHNA_POSE_COUNT = 8;
    std::unique_ptr<Core::AnimatedModel> krishnaPoses[KRISHNA_POSE_COUNT];
    float krishnaFadeTime = 0.0f;

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

    // Shadow Variables
    GLuint depthMapFBO;
    GLuint depthMapTexture;
    const GLuint SHADOW_WIDTH = 4096;
    const GLuint SHADOW_HEIGHT = 4096;
    mat4 lightSpaceMatrix;
    vec3 shadowLightPos = vec3(0.0f, 4000.0f, 4000.0f);
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
    DemoScene3()
    {   
        cubeMap = new CubeMap();
        // cubeMap[1] = new CubeMap();
        terrain = new Terrain(20.0f * 80.0f);
        waterMatrix = new WaterMatrix(300. * 400.);
        sceneCamera = new BezierCamera();
        rain = new Rain(40000);
        fireEffect = new Fire();
        // godRaysShader = new GodRaysShader();
    }

    bool initialize()
    {
        if (isInitialized)
            return TRUE;

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

        vrundavanGate = std::make_unique<Core::AnimatedModel>();
        // NOTE: Kaliya.fbx is ~62 MB and triggers an Assimp bad_alloc that
        // freezes the app while it thrashes memory. Use the GLB instead, which
        // loads quickly. AnimatedModel renders it statically (no bones).
        vrundavanGate->LoadModel("./assets/models/scene3_models/Kaliya.glb");

        // Krishna is animated by cross-fading three static Tandav poses (below),
        // so the single skinned Krishna.fbx is no longer loaded or drawn.
        // shreeKrishna = std::make_unique<Core::AnimatedModel>();
        // shreeKrishna->LoadModel("./assets/models/scene3_models/Krishna.fbx");

        // Eight static "position" poses of dancing Krishna (Meshy AI exports).
        // We cross-fade between them (fade in / fade out) to animate the Tandav.
        // These are GLB files with textures embedded, so no separate base-color
        // texture override is needed (the loader picks up the embedded ones).
        const char *krishnaPosePaths[KRISHNA_POSE_COUNT] = {
            "./assets/models/scene3_models/KrishnaTandav/1stPosition/Krishna_1st.glb",
            "./assets/models/scene3_models/KrishnaTandav/2ndPosition/Krishna_2nd.glb",
            "./assets/models/scene3_models/KrishnaTandav/3rdPosition/Krishna_3rd.glb",
            "./assets/models/scene3_models/KrishnaTandav/4thPosition/Krishna_4th.glb",
            "./assets/models/scene3_models/KrishnaTandav/5thPosition/Krishna_5th.glb",
            "./assets/models/scene3_models/KrishnaTandav/6thPosition/Krishna_6th.glb",
            "./assets/models/scene3_models/KrishnaTandav/7thPosition/Krishna_7th.glb",
            "./assets/models/scene3_models/KrishnaTandav/8thPosition/Krishna_8th.glb",
        };
        for (int i = 0; i < KRISHNA_POSE_COUNT; ++i)
        {
            krishnaPoses[i] = std::make_unique<Core::AnimatedModel>();
            krishnaPoses[i]->LoadModel(krishnaPosePaths[i]);
        }

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
        terrain->setWaterHeight(200.0f);       // keep your sea level

        waterMatrix->interpolateWaterColor = 1.0f;
        waterMatrix->moveFactor = 0.0f;

        // Kaliya Mardan happens on a turbulent river - drive the water stormy.
        waterMatrix->stormStrength = 1.0f;

        if (!rain->initialize(2))
        {
            PrintLog("Rain initialize failed (texture missing?) - continuing without rain\n");
            rain->alpha = 0.0f; // disable rain rendering
        }

        if (fireEffect)
        {
            if (!fireEffect->initialize())
            {
                PrintLog("Failed to initialize Fire effect in Scene 3\n");
            }
        }

        // Bloom effect for fire
        if (!bloomEffect.initialize_bloomShaderObject())
        {
            PrintLog("Failed to initialize Bloom effect in Scene 3\n");
        }
        bloomEffect.exposure = 1.8f;
        bloomEffect.gamma    = 1.2f;
        bloomEffect.blurAmount = 20;

        blackQuad = new OverlayColorShader();
        if (!blackQuad->initialize())
        {
            PrintLog("Failed to initialize OverlayColorShader in Scene 3\n");
        }

        // Event System
        sceneEvents = new EventManager(
            {{START_T, {0.0f, 45.0f}},
             {FADE_IN, {0.0f, 3.0f}},
             {SC_T1, {0.0f, 28.0f}},
             {FADE_OUT, {43.0f, 2.0f}},
             {END_T, {45.0f, 0.0f}}},
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
{-28.600082f, 328.500061f, 15.500017f},
{-398.600098f, 328.500061f, 800.500000f},
{-1203.600098f, 328.500061f, 1785.500000f},
{-1918.600098f, 328.500061f, 2685.500000f},
{-2888.600098f, 328.500061f, 3525.500000f},
{-3973.600098f, 328.500061f, 4790.500000f},
{-4773.600098f, 328.500061f, 5635.500000f},
{-5483.600098f, 328.500061f, 6310.500000f},
{-5793.600098f, 328.500061f, 6685.500000f},
{-5793.600098f, 388.500061f, 6685.500000f},
{-5888.600098f, 388.500061f, 6820.500000f},
{-5888.600098f, 418.500061f, 6820.500000f},
{-5888.600098f, 463.500061f, 6820.500000f},
{-5873.600098f, 458.500061f, 6830.500000f},
};

// YAW GLOBAL
std::vector<float> yawGlobalSC1 = {
133.000000f,
133.000000f,
133.000000f,
133.000000f,
133.000000f,
133.000000f,
133.000000f,
133.000000f,
131.000000f,
131.000000f,
133.000000f,
133.000000f,
135.000000f,
134.000000f,
};

// PITCH GLOBAL
std::vector<float> pitchGlobalSC1 = {
-7.000000f,
	-5.000000f,
	-5.000000f,
	-5.000000f,
	-5.000000f,
	-5.000000f,
	-5.000000f,
	-5.000000f,
	3.000000f,
	1.000000f,
	1.000000f,
	-2.000000f,
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
	};

        sc1.initialize();
        sc1.setBezierPoints(bezierPointsSC1, yawGlobalSC1, pitchGlobalSC1, fovGlobalSC1);
        sc1.update();
    }

   
    void display()
    {
        // Shadow Depth Pass
        mat4 lightProjectionMatrix = vmath::ortho(-6000.0f, 6000.0f, -6000.0f, 6000.0f, -10000.0f, 10000.0f);
        mat4 lightViewMatrix = vmath::lookat(shadowLightPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

        isDepthPass = true;
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        
        drawKaliyaMardanScene();
        
        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        isDepthPass = false;

        glViewport(0, 0, giWindowWidth, giWindowHeight);

        // Camera
        modelMatrix = mat4::identity();
        perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)giWindowWidth / (GLfloat)giWindowHeight, 10.0f, 10000000.0f);

        // modelLoader.display();
        // sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal , fovGlobal);
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
            // NOTE: viewport must match the FBO texture size (1920x1080),
            // not the window size, or the scene renders into only a corner of
            // the texture and the water samples the black (unrendered) region.
            waterMatrix->bindRefractionFBO(REFRACTION_FBO_WIDTH, REFRACTION_FBO_HEIGHT);
            {
                displayScene(-1.0);
            }
            waterMatrix->unbindRefractionFBO();

            // Reflection
            waterMatrix->bindReflectionFBO(REFLECTION_FBO_WIDTH, REFLECTION_FBO_HEIGHT);
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

        if (blackQuad && sceneFadeAlpha > 0.0f)
        {
            mat4 orthoProj = vmath::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
            mat4 id = mat4::identity();
            blackQuad->draw(orthoProj * id, 0.0f, 0.0f, 0.0f, sceneFadeAlpha);
        }

        // FIRE BLOOM (rendered FIRST so glow sits behind the models)
        if (fireEffect && !isDepthPass && fireAlpha > 0.0f)
        {
            // --- Pass 1: render fire into the HDR bloom FBO ---
            bloomEffect.bindBloomFBO();
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            pushMatrix(modelMatrix);
            {
                drawFire();
            }
            modelMatrix = popMatrix();
            bloomEffect.unbindBloomFBO();

            // Restore main viewport
            glViewport(0, 0, giWindowWidth, giWindowHeight);

            // --- Pass 2: ping-pong Gaussian blur on the bright buffer ---
            bloomEffect.renderBlurFBO();

            // Restore main viewport again (blur passes change it)
            glViewport(0, 0, giWindowWidth, giWindowHeight);

            // --- Pass 3: additively overlay bloom glow (no clear, models draw on top) ---
            bloomEffect.renderBloomGlowOnly(1.8f, 1.2f);
        }

        // Models drawn ON TOP of the bloom glow
        drawKaliyaMardanScene();

        // RAIN RENDERING
        pushMatrix(modelMatrix);
        {
            if (rain && rain->alpha > 0.0f)
            {
                drawRain();
            }
        }
        modelMatrix = popMatrix();

        // sceneCamera->displayBezierCurve();
    }

    void drawKaliyaMardanScene()
    {
        // drawHouse1();
        // drawHouse2();      
        // drawHouse3();
        drawKaliyaModel();
        drawKrishnaPosesFade();
        // drawCowHouse();
        // drawFarmLand();
        // drawHutHouse();
    }


    // ==================== kaliya mardan scene models drawing functions ====================
    
    void drawHouse1(bool isBlack = false)
    {
        if (!house1)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            house1->mTextureShader->Use();
            bindShadowUniforms(house1->mTextureShader.get());
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
            bindShadowUniforms(house1->mTextureShader.get());
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
            bindShadowUniforms(house2->mTextureShader.get());
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
            bindShadowUniforms(hutHouse->mTextureShader.get());
            hutHouse->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 hutHouseModelMatrix =
                vmath::translate(1110.0f, 50.0f, 500.0f) *
                vmath::scale(30.0f, 20.0f, 30.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

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
            bindShadowUniforms(house3->mTextureShader.get());
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

    void drawKaliyaModel(bool isBlack = false)
    {   
        if (!vrundavanGate)
            return;

        vrundavanGate->Update((float)gDeltaTime);

        pushMatrix(modelMatrix);
        {   
            // Both models share modelsAlpha — same quintic-eased fade variable.
            bool opaque = modelsAlpha >= 0.999f;
            if (opaque) {
                glDisable(GL_BLEND);
                glDepthMask(GL_TRUE);
            } else {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDepthMask(GL_FALSE);
            }

            vrundavanGate->mShader->Use();
            bindShadowUniforms(vrundavanGate->mShader.get());
            vrundavanGate->mShader->SetUniform("isBlack", isBlack);
            vrundavanGate->mShader->SetUniform("u_fade_alpha", modelsAlpha);
            vrundavanGate->mShader->SetUniform("u_UseAlpha", true);
            vrundavanGate->mShader->SetUniform("u_Alpha", modelsAlpha);

            // Kaliya.glb has tiny native units (~1), so it needs a large scale.
            // Placed just below Krishna (y=245) so Krishna stands on the heads.
            // Raised out of the water: at y=200 (the sea level) Kaliya's centre
            // sat on the waterline so half the body was submerged; lift it so the
            // body rises above the river surface.
            vmath::mat4 vrundavanGateModelMatrix =
                vmath::translate(-10.0f + -6128.600098f, 300.0f + 23.500046f + modelsRiseY, -40.0f + 7115.500000f) *
                vmath::scale(50.0f + 171.399918f, 50.0f + 171.399918f, 50.0f + 171.399918f) *
                vmath::rotate(-90.0f + -138.000000f, 0.0f, 1.0f, 0.0f);

            vrundavanGate->mShader->SetUniform("u_model", vrundavanGateModelMatrix);
            vrundavanGate->mShader->SetUniform("u_view", viewMatrix);
            vrundavanGate->mShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            vrundavanGate->mShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            vrundavanGate->mShader->SetUniform("u_ApplyToon", false);
            vrundavanGate->mShader->SetSampler2D("u_GGXLUT", 0, 5);
            vrundavanGate->mShader->SetUniform("u_DebugMode", 0);

            vrundavanGate->Draw(vrundavanGate->mShader);

            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawShreeKrishnaModel(bool isBlack = false)
    {
        if (!shreeKrishna)
            return;

        // Advance the skinned animation once per frame (drawn once from display()).
        shreeKrishna->Update((float)gDeltaTime);

        pushMatrix(modelMatrix);
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            shreeKrishna->mShader->Use();
            bindShadowUniforms(shreeKrishna->mShader.get());
            shreeKrishna->mShader->SetUniform("isBlack", isBlack);

            vmath::mat4 shreeKrishnaModelMatrix =
                vmath::translate(0.0f + objX, 245.0f + objY, -40.0f + objZ) *
                vmath::rotate(-90.0f + scaleY, 0.0f, 1.0f, 0.0f) *
                vmath::scale(0.18f + scaleX, 0.18f + scaleX, 0.18f + scaleX);

            shreeKrishna->mShader->SetUniform("u_model", shreeKrishnaModelMatrix);
            shreeKrishna->mShader->SetUniform("u_view", viewMatrix);
            shreeKrishna->mShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            shreeKrishna->mShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            shreeKrishna->mShader->SetUniform("u_ApplyToon", false);
            shreeKrishna->mShader->SetSampler2D("u_GGXLUT", 0, 5);

            shreeKrishna->mShader->SetUniform("u_DebugMode", 0);

            shreeKrishna->Draw(shreeKrishna->mShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    // Cross-fade the eight Krishna pose models: each pose holds fully opaque,
    // then dissolves into the next (fade out current + fade in next), sequenced
    // 1 -> 2 -> ... -> 8 -> 1 on an endless loop.
    void drawKrishnaPosesFade(bool isBlack = false)
    {
        // Advance the fade timeline here (display path owns per-frame time).
        if (!isDepthPass) {
            krishnaFadeTime += (float)gDeltaTime;
        }

        const float holdDur = 5.0f; // seconds a pose stays fully visible (slow cinematic hold)
        const float fadeDur = 3.0f; // seconds of cross-dissolve into next pose (slow dissolve)
        const float slotDur = holdDur + fadeDur;
        const float cycle   = slotDur * KRISHNA_POSE_COUNT;

        float t = fmodf(krishnaFadeTime, cycle);
        if (t < 0.0f) t += cycle;

        int   slot  = (int)(t / slotDur) % KRISHNA_POSE_COUNT;
        float local = t - slot * slotDur;            // time within current slot
        int   next  = (slot + 1) % KRISHNA_POSE_COUNT;

        float alpha[KRISHNA_POSE_COUNT] = {}; // zero-initialise all 8 poses
        if (local < holdDur)
        {
            alpha[slot] = 1.0f;                       // fully showing this pose
        }
        else
        {
            float f = (local - holdDur) / fadeDur;    // 0..1 cross-dissolve
            f = f < 0.0f ? 0.0f : (f > 1.0f ? 1.0f : f);
            
            // Smooth cosine cross-dissolve
            float smoothF = 0.5f - 0.5f * cosf(f * 3.14159265f);
            alpha[slot] = 1.0f - smoothF;
            alpha[next] = smoothF;
        }

        // Shared placement for all eight poses (same spot, same facing).
        // These poses are GLB (Y-up, already upright) so NO Z-up fix is needed.
        // Scale is calibrated to Kaliya's scale (50) the same way Scene 2.5 sized
        // this Krishna GLB. Tune translate.y / scale / the -90 yaw if needed.
        vmath::mat4 krishnaModelMatrix =
            vmath::translate(-10.0f + -6088.600098f + -13.600082f, 325.0f + 163.500046f + -6.499954f + modelsRiseY, -40.0f + 7095.500000f + 10.500017f) *
            vmath::rotate(-90.0f + -154.000000f, 0.0f, 1.0f, 0.0f) *
            vmath::scale(44.0f + -12.000000f + scaleX, 44.0f + -12.000000f + scaleX, 44.0f + -12.000000f + scaleX);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw the fading-out pose (slot) first, then the fading-in pose (next)
        // on top, so the cross-dissolve reads correctly. During a hold only the
        // current slot has alpha, so it's the only one drawn.
        int drawOrder[2] = { slot, next };
        for (int oi = 0; oi < 2; ++oi)
        {
            int i = drawOrder[oi];
            if (oi == 1 && next == slot)
                continue; // no second pose during a pure hold
            if (!krishnaPoses[i] || alpha[i] <= 0.001f)
                continue;

            // A fully-visible (held) pose is drawn solid with depth writes so it
            // self-occludes correctly; a cross-fading pose is blended with depth
            // writes off so the two overlapping poses dissolve into each other.
            bool opaque = (alpha[i] * modelsAlpha) >= 0.999f;
            if (opaque) { glDisable(GL_BLEND); glDepthMask(GL_TRUE); }
            else        { glEnable(GL_BLEND);  glDepthMask(GL_FALSE); }

            pushMatrix(modelMatrix);
            {
                krishnaPoses[i]->mShader->Use();
                bindShadowUniforms(krishnaPoses[i]->mShader.get());
                krishnaPoses[i]->mShader->SetUniform("isBlack", isBlack);
                krishnaPoses[i]->mShader->SetUniform("u_fade_alpha", modelsAlpha);
                krishnaPoses[i]->mShader->SetUniform("u_model", krishnaModelMatrix);
                krishnaPoses[i]->mShader->SetUniform("u_view", viewMatrix);
                krishnaPoses[i]->mShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                krishnaPoses[i]->mShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
                krishnaPoses[i]->mShader->SetUniform("u_ApplyToon", false);
                krishnaPoses[i]->mShader->SetSampler2D("u_GGXLUT", 0, 5);
                krishnaPoses[i]->mShader->SetUniform("u_DebugMode", 0);
                krishnaPoses[i]->mShader->SetUniform("u_UseAlpha", true);
                krishnaPoses[i]->mShader->SetUniform("u_Alpha", alpha[i]);

                krishnaPoses[i]->Draw(krishnaPoses[i]->mShader);
            }
            modelMatrix = popMatrix();
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
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
            bindShadowUniforms(cowHouse->mTextureShader.get());
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
            bindShadowUniforms(well->mTextureShader.get());
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
            bindShadowUniforms(farmLand->mTextureShader.get());
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

    // FIRE RELATED
    void drawFire(void)
    {
        if (!fireEffect) return;

        glDepthMask(GL_FALSE);
        
        // Draw one massive fire wall behind Krishna
        pushMatrix(modelMatrix);
        {
            // Position fire behind Krishna (Krishna base is ~ X:-6112, Y:482, Z:7066)
            // We push it back along X and Z, and lift it up so it frames him nicely.
            modelMatrix = modelMatrix * vmath::translate(-6300.0f , 450.0f + modelsRiseY, 7300.0f );
            
            // Rotate to face the camera more directly (approximate camera look direction)
            modelMatrix = modelMatrix * vmath::rotate(30.0f + -11.000000f + -68.600082f, 0.0f, 1.0f, 0.0f);
            
            // Scale it up
            modelMatrix = modelMatrix * vmath::scale(1000.0f, 1000.0f, 1.0f);
            
            fireEffect->display();
        }
        modelMatrix = popMatrix();

        glDepthMask(GL_TRUE);
    }

    // RAIN RELATED
    void drawRain(void)
    {
        pushMatrix(modelMatrix);
        {
            rain->lightAmbient[0]  = 0.0f;
            rain->lightAmbient[1]  = 0.0f;
            rain->lightAmbient[2]  = 0.0f;
            rain->lightAmbient[3]  = 1.0f;

            rain->lightDiffuse[0]  = 1.0f;
            rain->lightDiffuse[1]  = 1.0f;
            rain->lightDiffuse[2]  = 1.0f;
            rain->lightDiffuse[3]  = 1.0f;

            rain->lightPosition[0] = 0.0f;
            rain->lightPosition[1] = 100.0f;
            rain->lightPosition[2] = -30.0f;
            rain->lightPosition[3] = 1.0f;

            rain->lightSpecular[0] = 1.0f;
            rain->lightSpecular[1] = 1.0f;
            rain->lightSpecular[2] = 1.0f;
            rain->lightSpecular[3] = 1.0f;

            glEnable(GL_BLEND);
            rain->display();
            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }
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

        sceneCamera = &sc1;
        sceneCamera->time = sceneEvents->getEventTime(SC_T1);

        // Update sequence time
        sceneEvents->increment();
        
        if (sceneEvents->isEventComplete(END_T))
            isSceneComplete = true;

        float timeNow = sceneEvents->getT();

        // Models rise & fade: 18s → 28s (10-second quintic ease for a slow, smooth emergence)
        if (timeNow < 18.0f) {
            modelsRiseY = -600.0f;
            modelsAlpha = 0.0f;
        } else if (timeNow >= 18.0f && timeNow <= 28.0f) {
            float t = (timeNow - 18.0f) / 10.0f;           // 0..1 over 10 seconds
            // Quintic ease-in-out: much softer start and end than smoothstep
            float smoothT = t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
            modelsRiseY = -600.0f + (smoothT * 600.0f);
            modelsAlpha = smoothT;
        } else {
            modelsRiseY = 0.0f;
            modelsAlpha = 1.0f;
        }

        if (timeNow < 30.0f) {
            sceneFadeAlpha = 0.0f;
        } else if (timeNow >= 30.0f && timeNow <= 34.0f) {
            float t = (timeNow - 30.0f) / 4.0f;
            float smoothT = t * t * (3.0f - 2.0f * t);
            sceneFadeAlpha = smoothT;
        } else {
            sceneFadeAlpha = 1.0f;
        }

        if (timeNow < 34.0f) {
            fireAlpha = 0.0f;
        } else if (timeNow >= 34.0f && timeNow <= 38.0f) {
            float t = (timeNow - 34.0f) / 4.0f;
            float smoothT = t * t * (3.0f - 2.0f * t);
            fireAlpha = smoothT;
        } else {
            fireAlpha = 1.0f;
        }

        if (fireEffect) {
            fireEffect->alpha = fireAlpha;
        }

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
        shreeKrishna.reset();
        for (int i = 0; i < KRISHNA_POSE_COUNT; ++i)
            krishnaPoses[i].reset();
        cowHouse.reset();
        farmLand.reset();

        if (rain)
        {
            rain->uninitialize();
            delete rain;
            rain = nullptr;
        }

        if (fireEffect)
        {
            fireEffect->uninitialize();
            delete fireEffect;
            fireEffect = NULL;
        }

        if (blackQuad)
        {
            blackQuad->uninitialize();
            delete blackQuad;
            blackQuad = NULL;
        }

        // modelLoader.uninitialize();
    }
};
