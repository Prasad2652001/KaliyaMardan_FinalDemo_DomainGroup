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
    std::unique_ptr<Core::Model> house5;
    std::unique_ptr<Core::Model> house6;
    std::unique_ptr<Core::Model> house7;
    std::unique_ptr<Core::Model> house8;


    std::unique_ptr<Core::Model> hutHouse;
    std::unique_ptr<Core::Model> vrundavanGate;
    std::unique_ptr<Core::Model> cowHouse;
    std::unique_ptr<Core::Model> cow1;
    std::unique_ptr<Core::Model> cow2;
    std::unique_ptr<Core::Model> well;
    std::unique_ptr<Core::Model> farmer2_bailgadi;
    std::unique_ptr<Core::Model> farmer3;
    std::unique_ptr<Core::Model> tree1;
    std::unique_ptr<Core::Model> tree2;
    std::unique_ptr<Core::Model> whiteBull;
    std::unique_ptr<Core::Model> treeKatta;
    std::unique_ptr<Core::Model> nandBaba;
    std::unique_ptr<Core::Model> gavkari1;
    std::unique_ptr<Core::Model> gavkari2_ladies;
    std::unique_ptr<Core::Model> yashoda;
    std::unique_ptr<Core::Model> gavkari3;
    std::unique_ptr<Core::Model> gavkari4_ladies;
    std::unique_ptr<Core::Model> tulsi;

    // std::unique_ptr<Core::AnimatedModel> farmer1;

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

    // Shadow variables
    GLuint depthMapFBO;
    GLuint depthMapTexture;
    const GLuint SHADOW_WIDTH = 4096;
    const GLuint SHADOW_HEIGHT = 4096;
    mat4 lightSpaceMatrix;
    vec3 shadowLightPos = vec3(0.0f, 4000.0f, 4000.0f); // Adjust light pos to cover the scene
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
    DemoScene1()
    {   
        cubeMap = new CubeMap();
        // cubeMap[1] = new CubeMap();
        terrain = new Terrain(20.0f * 80.0f);
        // waterMatrix = new WaterMatrix(300. * 400.);
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

        // // Camera

        // Initializing GLB Model
        programStaticPBR = new glshaderprogram({"./src/shaders/modelgltf/pbrStatic.vert", "./src/shaders/modelgltf/pbrMain.frag"});

        cowHouse = std::make_unique<Core::Model>();
        cowHouse->LoadModel("./assets/models/scene1_models/vrundavan/cowHouse.glb");

        cow1 = std::make_unique<Core::Model>();
        cow1->LoadModel("./assets/models/scene1_models/vrundavan/cow1.glb");

        cow2 = std::make_unique<Core::Model>();
        cow2->LoadModel("./assets/models/scene1_models/vrundavan/cow2.glb");
        
        house1 = std::make_unique<Core::Model>();
        house1->LoadModel("./assets/models/scene1_models/vrundavan/house1.glb");

        house2 = std::make_unique<Core::Model>();
        house2->LoadModel("./assets/models/scene1_models/vrundavan/house2.glb");

        house3 = std::make_unique<Core::Model>();
        house3->LoadModel("./assets/models/scene1_models/vrundavan/house3.glb");

        house5 = std::make_unique<Core::Model>();
        house5->LoadModel("./assets/models/scene1_models/vrundavan/house5.glb");

        house6 = std::make_unique<Core::Model>();
        house6->LoadModel("./assets/models/scene1_models/vrundavan/house6.glb");

        house7 = std::make_unique<Core::Model>();
        house7->LoadModel("./assets/models/scene1_models/vrundavan/house7.glb");

        house8 = std::make_unique<Core::Model>();
        house8->LoadModel("./assets/models/scene1_models/vrundavan/house8.glb");

        vrundavanGate = std::make_unique<Core::Model>();
        vrundavanGate->LoadModel("./assets/models/scene1_models/vrundavan/vrundavanGate.glb");
        
        well = std::make_unique<Core::Model>();
        well->LoadModel("./assets/models/scene1_models/vrundavan/well.glb");

        hutHouse = std::make_unique<Core::Model>();
        hutHouse->LoadModel("./assets/models/scene1_models/vrundavan/hutHouse.glb");

        farmer2_bailgadi = std::make_unique<Core::Model>();
        farmer2_bailgadi->LoadModel("./assets/models/scene1_models/vrundavan/farmer2_bailgadi.glb");

        farmer3 = std::make_unique<Core::Model>();
        farmer3->LoadModel("./assets/models/scene1_models/vrundavan/farmer3.glb");

        tree1 = std::make_unique<Core::Model>();
        tree1->LoadModel("./assets/models/scene1_models/vrundavan/peepal_tree.glb");

        tree2 = std::make_unique<Core::Model>();
        tree2->LoadModel("./assets/models/scene1_models/vrundavan/tree2.glb");

        whiteBull = std::make_unique<Core::Model>();
        whiteBull->LoadModel("./assets/models/scene1_models/vrundavan/whiteBull.glb");

        treeKatta = std::make_unique<Core::Model>();
        treeKatta->LoadModel("./assets/models/scene1_models/vrundavan/treeKatta.glb");

        nandBaba = std::make_unique<Core::Model>();
        nandBaba->LoadModel("./assets/models/scene1_models/vrundavan/gavkari/nandBaba.fbx");

        gavkari1 = std::make_unique<Core::Model>();
        gavkari1 ->LoadModel("./assets/models/scene1_models/vrundavan/gavkari/gavkari1.glb");

        gavkari2_ladies = std::make_unique<Core::Model>();
        gavkari2_ladies ->LoadModel("./assets/models/scene1_models/vrundavan/gavkari/gavkari2_ladies.glb");

        yashoda = std::make_unique<Core::Model>();
        yashoda ->LoadModel("./assets/models/scene1_models/vrundavan/gavkari/yashoda.glb");

        gavkari3 = std::make_unique<Core::Model>();
        gavkari3 ->LoadModel("./assets/models/scene1_models/vrundavan/gavkari/gavkari3.glb");

        gavkari4_ladies = std::make_unique<Core::Model>();
        gavkari4_ladies ->LoadModel("./assets/models/scene1_models/vrundavan/gavkari/gavkari4_ladies.glb");


        tulsi = std::make_unique<Core::Model>();
        tulsi ->LoadModel("./assets/models/scene1_models/vrundavan/gavkari/tulsi.glb");

        // farmer1 = std::make_unique<Core::AnimatedModel>();   
        // farmer1->LoadModel("./assets/models/scene1_models/vrundavan/nandBaba/nandBaba.fbx");
        // farmer1->SetBaseColorTexture("./assets/models/scene1_models/vrundavan/nandBaba/baseTexture.png");

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
        // waterMatrix->initialize();

        // IMPortanttt
        terrain->setFreq(0.020f);            // broad beach hills
        terrain->setDispFactor(15.595f);        // softer hill height
        terrain->setTessMultiplier(1.600f);
        terrain->setTextureTransitionFactor(1.0f); // use green texture set only
        terrain->setGrassCoverage(0.0f);     // >1.0 avoids grass branch
        terrain->setWaterHeight(-20.0f);       // keep your sea level

        // waterMatrix->interpolateWaterColor = 1.0f;
        // waterMatrix->moveFactor = 0.0f;

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
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        setupCamera();
        // sceneCamera->initialize();
        // sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal);
        sceneCamera->handlePerspective = true;

        isInitialized = true;
        isSceneComplete = false;
        return TRUE;
    }

    void setupCamera()
    {
    
        std::vector<std::vector<float>> bezierPointsSC1 = {
    {7591.399902f, 3313.500000f, 5725.500000f},
    {7341.399902f, 3313.500000f, 5455.500000f},
    {6851.399902f, 3113.500000f, 5175.500000f},
    {6851.399902f, 2563.500000f, 5175.500000f},
    {6051.399902f, 2563.500000f, 4295.500000f},
    {6051.399902f, 2233.500000f, 4295.500000f},
    {5211.399902f, 1583.500000f, 3305.500000f},
    {4741.399902f, 1583.500000f, 3305.500000f},
    {4501.399902f, 1093.500000f, 3305.500000f},
    {3771.399902f, 893.500000f, 2795.500000f},
    {3771.399902f, 583.500000f, 2795.500000f},
    {3771.399902f, 323.500000f, 2795.500000f},
    {2621.399902f, 183.500000f, 2105.500000f},
    {2621.399902f, 133.500000f, 2105.500000f},
    {2171.399902f, 93.500000f, 1805.500000f},
    {2051.399902f, 93.500000f, 1695.500000f},
    {1941.399902f, 93.500000f, 1615.500000f},
    {1741.399902f, 93.500000f, 1615.500000f},
    {1741.399902f, 113.500000f, 1595.500000f},
    {1741.399902f, 113.500000f, 1525.500000f},
    {1741.399902f, 113.500000f, 1475.500000f},
    {1741.399902f, 113.500000f, 1475.500000f},
    {1741.399902f, 113.500000f, 1385.500000f},
    {1741.399902f, 113.500000f, 1145.500000f},
    {1741.399902f, 113.500000f, 875.500000f},
    {1551.399902f, 113.500000f, 515.500000f},
    {1401.399902f, 113.500000f, 405.500000f},
    {1331.399902f, 113.500000f, 235.500000f},
    {1211.399902f, 113.500000f, 135.500000f},
    {1081.399902f, 113.500000f, 5.500000f},
    {751.399902f, 113.500000f, -134.500000f},
    {341.399902f, 163.500000f, -84.500000f},
    {51.399902f, 153.500000f, 145.500000f},
    {51.399902f, 103.500000f, 145.500000f},
    {51.399902f, 103.500000f, 155.500000f},
    {51.399902f, 103.500000f, 225.500000f},
    {41.399902f, 103.500000f, 375.500000f},
    {41.399902f, 103.500000f, 535.500000f},
    {41.399902f, 103.500000f, 895.500000f},
    {41.399902f, 103.500000f, 905.500000f},
    {411.399902f, 103.500000f, 1175.500000f},
    {621.399902f, 103.500000f, 1175.500000f},
    {1041.399902f, 103.500000f, 1175.500000f},
    {1231.399902f, 103.500000f, 1175.500000f},
    {1231.399902f, 223.500000f, 1175.500000f},
    {1231.399902f, 713.500000f, 1175.500000f},
    {1231.399902f, 1213.500000f, 1175.500000f},
    {1231.399902f, 1953.500000f, 1175.500000f},
    {1231.399902f, 3143.500000f, 1175.500000f},
    {1231.399902f, 3983.500000f, 1175.500000f},
    {1231.399902f, 4463.500000f, 1175.500000f},
    {1231.399902f, 4793.500000f, 1175.500000f},
    {1231.399902f, 5273.500000f, 1175.500000f},
    {1231.399902f, 5963.500000f, 1175.500000f},
    {1231.399902f, 6673.500000f, 1175.500000f},
    {1231.399902f, 6673.500000f, 1175.500000f},
    };


// YAW GLOBAL
std::vector<float> yawGlobalSC1 = {
219.000000f,
216.000000f,
216.000000f,
216.000000f,
216.000000f,
216.000000f,
216.000000f,
216.000000f,
216.000000f,
216.000000f,
216.000000f,
214.000000f,
215.000000f,
219.000000f,
220.000000f,
220.000000f,
219.000000f,
219.000000f,
217.000000f,
217.000000f,
215.000000f,
215.000000f,
209.000000f,
205.000000f,
197.000000f,
176.000000f,
166.000000f,
159.000000f,
155.000000f,
142.000000f,
134.000000f,
128.000000f,
152.000000f,
171.000000f,
178.000000f,
189.000000f,
197.000000f,
207.000000f,
207.000000f,
156.000000f,
156.000000f,
194.000000f,
204.000000f,
204.000000f,
204.000000f,
204.000000f,
204.000000f,
204.000000f,
204.000000f,
204.000000f,
204.000000f,
204.000000f,
204.000000f,
204.000000f,
204.000000f,
204.000000f,
};


// PITCH GLOBAL
std::vector<float> pitchGlobalSC1 = {
-34.000000f,
	-34.000000f,
	-31.000000f,
	-28.000000f,
	-28.000000f,
	-25.000000f,
	-25.000000f,
	-25.000000f,
	-25.000000f,
	-21.000000f,
	-17.000000f,
	-12.000000f,
	-11.000000f,
	-9.000000f,
	-6.000000f,
	-6.000000f,
	-4.000000f,
	-4.000000f,
	-4.000000f,
	-4.000000f,
	-4.000000f,
	-4.000000f,
	-4.000000f,
	-4.000000f,
	-4.000000f,
	-4.000000f,
	-4.000000f,
	-4.000000f,
	-4.000000f,
	-4.000000f,
	-1.000000f,
	-7.000000f,
	-11.000000f,
	-11.000000f,
	-10.000000f,
	-7.000000f,
	-7.000000f,
	-2.000000f,
	-2.000000f,
	-2.000000f,
	-2.000000f,
	-2.000000f,
	-2.000000f,
	-26.000000f,
	-38.000000f,
	-52.000000f,
	-69.000000f,
	-82.000000f,
	-82.000000f,
	-86.000000f,
	-83.000000f,
	-83.000000f,
	-85.000000f,
	-85.000000f,
	-85.000000f,
	-85.000000f,
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
        
        drawVrundavanScene();
        
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

        // pushMatrix(modelMatrix);
        // {
        //      // Refraction
        //     // waterMatrix->bindRefractionFBO(1920, 1080);
        //     {
        //         displayScene(-1.0);
        //     }
        //     waterMatrix->unbindRefractionFBO();

        //     // Refraction
        //     waterMatrix->bindReflectionFBO(1920, 1080);
        //     {
        //         displayScene(1.0);
        //     }
        //     waterMatrix->unbindReflectionFBO();
        // }
        // modelMatrix = popMatrix();

        // Water Bed
        // pushMatrix(modelMatrix);
        // {
        //     modelMatrix = modelMatrix * translate(0.0f, (float)terrain->getWaterHeight() + 1.0f, 0.0f);
        //     waterMatrix->renderWaterQuad(terrain->getWaterHeight());
        // }
        // modelMatrix = popMatrix();

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
        drawHouse5();
        drawHouse6();
        drawHouse7();
        drawHouse8();

        drawKaliyaModel();
        drawCowHouse();
        drawHutHouse();
        drawFarmers();
        drawTree1();
        drawTree2();
        drawWhiteBull();
        drawTreeKatta();
        drawGavkari();
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
            bindShadowUniforms(house1->mTextureShader.get());
            house1->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house1ModelMatrix =
                vmath::translate(-40.0f, 0.0f, 42.0f) *
                vmath::scale(43.0f, 33.0f, 43.0f) *
                vmath::rotate(-32.0f, 0.0f, 1.0f, 0.0f);

            house1->mTextureShader->SetUniform("u_model", house1ModelMatrix);
            house1->mTextureShader->SetUniform("u_view", viewMatrix);
            house1->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            house1->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            house1->mTextureShader->SetUniform("u_ApplyToon", false); 

            house1->Draw(house1->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();


        // draw same house with different scaling/ left side from first house - small
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

        
        // draw same house with different scaling/ in front of first house - small
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            house1->mTextureShader->Use();
            bindShadowUniforms(house1->mTextureShader.get());
            house1->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house1ModelMatrix =
                vmath::translate(-30.0f, 0.0f, 1030.0f) *
                vmath::scale(15.0f, 15.0f, 15.0f) *
                vmath::rotate(-210.0f, 0.0f, 1.0f, 0.0f);

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

        // draw same house with different scaling/ in front and left side of first house - big
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            house1->mTextureShader->Use();
            bindShadowUniforms(house1->mTextureShader.get());
            house1->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house1ModelMatrix =
                   vmath::translate(630.0f, 0.0f, 1420.0f) *
                   vmath::scale(30.0f, 20.0f, 30.0f) *
                   vmath::rotate(-190.0f, 0.0f, 1.0f, 0.0f);

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

         // draw same house with different scaling/ in front and left side of first house - big
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            house1->mTextureShader->Use();
            bindShadowUniforms(house1->mTextureShader.get());
            house1->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house1ModelMatrix =
                   vmath::translate(1700.0f, 0.0f, 500.0f) *
                   vmath::scale(30.0f, 20.0f, 30.0f) *
                   vmath::rotate(-20.0f, 0.0f, 1.0f, 0.0f);

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
                vmath::translate(-400.0f, 140.0f, 500.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(70.0f, 0.0f, 1.0f, 0.0f);

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

        // tulsi vrundavan
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tulsi->mTextureShader->Use();
            bindShadowUniforms(tulsi->mTextureShader.get());
            tulsi->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tulsiModelMatrix =
                vmath::translate(-70.0f, 0.0f, 630.0f) *
                vmath::scale(5.0f, 5.0f, 5.0f) *
                vmath::rotate(70.0f, 0.0f, 1.0f, 0.0f); 

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tulsi->mTextureShader->SetUniform("u_model", tulsiModelMatrix);
            tulsi->mTextureShader->SetUniform("u_view", viewMatrix);
            tulsi->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tulsi->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            tulsi->mTextureShader->SetUniform("u_ApplyToon", false); 

            //house2->mTextureShader->exposure = 1.2f;
            tulsi->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);

            tulsi->Draw(tulsi->mTextureShader);

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
                vmath::translate(833.0f, 10.0f, 200.0f) *
                vmath::scale(60.0f, 60.0f, 60.0f) *
                vmath::rotate(190.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

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

        // 2nd hut house
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            hutHouse->mTextureShader->Use();
            bindShadowUniforms(hutHouse->mTextureShader.get());
            hutHouse->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 hutHouseModelMatrix =
                //vmath::translate(833.0f, 50.0f, 200.0f) *
                vmath::translate(-330.0f, 10.0f, 1000.0f) *
                vmath::scale(60.0f, 60.0f, 60.0f) *
                vmath::rotate(210.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

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

         // 3rd hut house
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            hutHouse->mTextureShader->Use();
            bindShadowUniforms(hutHouse->mTextureShader.get());
            hutHouse->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 hutHouseModelMatrix =
                vmath::translate(400.0f, 10.0f, -70.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-180.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

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
                vmath::translate(400.0f, 0.0f, 300.0f) *
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

        // 2nd house3
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            house3->mTextureShader->Use();
            bindShadowUniforms(house3->mTextureShader.get());
            house3->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house3ModelMatrix =
                vmath::translate(40.0f, 0.0f, 1300.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-100.0f, 0.0f, 1.0f, 0.0f);

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

    void drawHouse5(bool isBlack = false)
    {   
        if (!house5)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            house5->mTextureShader->Use();
            bindShadowUniforms(house5->mTextureShader.get());
            house5->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house5ModelMatrix =
                vmath::translate(1000.0f, 0.0f, -100.0f) *
                vmath::scale(35.0f, 35.0f, 35.0f) *
                vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            house5->mTextureShader->SetUniform("u_model", house5ModelMatrix);
            house5->mTextureShader->SetUniform("u_view", viewMatrix);
            house5->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            house5->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            house5->mTextureShader->SetUniform("u_ApplyToon", false); 

            //house3->mTextureShader->exposure = 1.2f;
            house5->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);

            house5->Draw(house5->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawHouse6(bool isBlack = false)
    {   
        if (!house6)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            house6->mTextureShader->Use();
            bindShadowUniforms(house6->mTextureShader.get());
            house6->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house6ModelMatrix =
                vmath::translate(1200.0f, 100.0f, 1800.0f) *
                vmath::scale(400.0f, 400.0f, 400.0f) *
                vmath::rotate(160.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            house6->mTextureShader->SetUniform("u_model", house6ModelMatrix);
            house6->mTextureShader->SetUniform("u_view", viewMatrix);
            house6->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            house6->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            house6->mTextureShader->SetUniform("u_ApplyToon", false); 

            //house3->mTextureShader->exposure = 1.2f;
            house6->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);

            house6->Draw(house6->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawHouse7(bool isBlack = false)
    {   
        if (!house7)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            house7->mTextureShader->Use();
            bindShadowUniforms(house7->mTextureShader.get());
            house7->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house7ModelMatrix =
                vmath::translate(2200.0f, 105.0f, 700.0f) *
                vmath::scale(150.0f, 150.0f, 150.0f) *
                vmath::rotate(-20.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            house7->mTextureShader->SetUniform("u_model", house7ModelMatrix);
            house7->mTextureShader->SetUniform("u_view", viewMatrix);
            house7->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            house7->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            house7->mTextureShader->SetUniform("u_ApplyToon", false); 

            //house3->mTextureShader->exposure = 1.2f;
            house7->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            
            house7->Draw(house7->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();  

        // house 2  
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            house7->mTextureShader->Use();
            bindShadowUniforms(house7->mTextureShader.get());
            house7->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house7ModelMatrix =
                vmath::translate(1500.0f, 100.0f, -330.0f) *
                vmath::scale(200.0f, 200.0f, 200.0f) *
                vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            house7->mTextureShader->SetUniform("u_model", house7ModelMatrix);
            house7->mTextureShader->SetUniform("u_view", viewMatrix);
            house7->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            house7->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            house7->mTextureShader->SetUniform("u_ApplyToon", false); 

            //house3->mTextureShader->exposure = 1.2f;
            house7->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            
            house7->Draw(house7->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    } 

    void drawHouse8(bool isBlack = false)
    {   
        if (!house8)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            house8->mTextureShader->Use();
            bindShadowUniforms(house8->mTextureShader.get());
            house8->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house8ModelMatrix =
                vmath::translate(300.0f, 150.0f, 2000.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(120.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            house8->mTextureShader->SetUniform("u_model", house8ModelMatrix);
            house8->mTextureShader->SetUniform("u_view", viewMatrix);
            house8->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            house8->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            house8->mTextureShader->SetUniform("u_ApplyToon", false); 
            
            house8->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            
            house8->Draw(house8->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            house8->mTextureShader->Use();
            bindShadowUniforms(house8->mTextureShader.get());
            house8->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 house8ModelMatrix =
                vmath::translate(600.0f, 150.0f, -500.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);
            
                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            house8->mTextureShader->SetUniform("u_model", house8ModelMatrix);
            house8->mTextureShader->SetUniform("u_view", viewMatrix);
            house8->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            house8->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
            house8->mTextureShader->SetUniform("u_ApplyToon", false); 
            
            house8->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            
            house8->Draw(house8->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    } 

    void drawKaliyaModel(bool isBlack = false)
    {   
        if (!vrundavanGate)
            return;
        
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            vrundavanGate->mTextureShader->Use();
            bindShadowUniforms(vrundavanGate->mTextureShader.get());
            vrundavanGate->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 vrundavanGateModelMatrix =
                vmath::translate(2000.0f, 170.0f, 1600.0f) *
                vmath::scale(300.0f, 300.0f, 300.0f) *
                vmath::rotate(60.0f, 0.0f, 1.0f, 0.0f);

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
            bindShadowUniforms(cowHouse->mTextureShader.get());
            cowHouse->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 cowHouseModelMatrix =
                vmath::translate(-400.0f, 0.0f, 70.0f) *
                vmath::scale(40.0f, 40.0f, 40.0f) *
                vmath::rotate(-120.0f, 0.0f, 1.0f, 0.0f);

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

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            cow1->mTextureShader->Use();
            bindShadowUniforms(cow1->mTextureShader.get());
            cow1->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 cow1ModelMatrix =
                vmath::translate(-400.0f + 1661.399902f , 0.0f + 3.500046f, 70.0f + 15.500017f) *
                vmath::scale(0.5f , 0.5f , 0.5f) *
                vmath::rotate(-120.0f, 0.0f, 1.0f, 0.0f);

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

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            cow2->mTextureShader->Use();
            bindShadowUniforms(cow2->mTextureShader.get());
            cow2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 cow2ModelMatrix =
                vmath::translate(-400.0f + 1661.399902f + objX, 0.0f + 3.500046f, 70.0f + 15.500017f + objZ) *
                vmath::scale(0.5f , 0.5f , 0.5f) *
                vmath::rotate(-120.0f, 0.0f, 1.0f, 0.0f);

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
                vmath::scale(4.0f, 4.0f, 4.0f) *
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

    void drawFarmers(bool isBlack = false)
    {
        // if (!farmer1)
        //     return;

        // // Advance the skinned animation once per frame (drawn once from display()).
        // farmer1->Update((float)gDeltaTime);

        // pushMatrix(modelMatrix);
        // {
        //     glEnable(GL_BLEND);
        //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //     farmer1->mShader->Use();
        //     farmer1->mShader->SetUniform("isBlack", isBlack);

        //     vmath::mat4 farmer1ModelMatrix =
        //         vmath::translate(400.0f, 0.0f, 1000.0f) *
        //         vmath::rotate(90.0f, 0.0f, 1.0f, 0.0f) *
        //         vmath::scale(0.3f, 0.3f, 0.3f);

        //         // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
        //         // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
        //         // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

        //     farmer1->mShader->SetUniform("u_model", farmer1ModelMatrix);
        //     farmer1->mShader->SetUniform("u_view", viewMatrix);
        //     farmer1->mShader->SetUniform("u_projection", perspectiveProjectionMatrix);
        //     farmer1->mShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f));
        //     farmer1->mShader->SetUniform("u_ApplyToon", false);
        //     farmer1->mShader->SetSampler2D("u_GGXLUT", 0, 5);

        //     farmer1->mShader->SetUniform("u_DebugMode", 0);

        //     farmer1->Draw(farmer1->mShader);

        //     glDisable(GL_BLEND);
        // }
        // modelMatrix = popMatrix();

        if (!farmer2_bailgadi)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            farmer2_bailgadi->mTextureShader->Use();
            bindShadowUniforms(farmer2_bailgadi->mTextureShader.get());
            farmer2_bailgadi->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 farmer2_bailgadiModelMatrix =
                vmath::translate(1630.0f, 60.0f, 1400.0f) *
                vmath::scale(70.0f, 70.0f, 70.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            farmer2_bailgadi->mTextureShader->SetUniform("u_model", farmer2_bailgadiModelMatrix);
            farmer2_bailgadi->mTextureShader->SetUniform("u_view", viewMatrix);
            farmer2_bailgadi->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            farmer2_bailgadi->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            farmer2_bailgadi->mTextureShader->SetUniform("u_ApplyToon", false); 
            
            farmer2_bailgadi->Draw(farmer2_bailgadi->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        if (!farmer3)
            return;

        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            farmer3->mTextureShader->Use();
            bindShadowUniforms(farmer3->mTextureShader.get());
            farmer3->mTextureShader->SetUniform("isBlack", isBlack);
            
            vmath::mat4 farmer3ModelMatrix =
                vmath::translate(1000.0f, 28.0f, 1000.0f) *
                vmath::scale(30.0f, 30.0f, 30.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);    

            farmer3->mTextureShader->SetUniform("u_model", farmer3ModelMatrix);
            farmer3->mTextureShader->SetUniform("u_view", viewMatrix);
            farmer3->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            farmer3->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            farmer3->mTextureShader->SetUniform("u_ApplyToon", false); 

            farmer3->Draw(farmer3->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

    }

    void drawTree1(bool isBlack = false)
    {   
        if (!tree2)
            return;
        // first pair of trees - left side of scene
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(2.0f, -40.0f, -500.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // second pair of trees - right side of scene
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(-400.0f, -40.0f, 1300.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // third pair of trees - right side of scene - in front of second pair
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(2000.0f, -40.0f, 0.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // fourth pair of trees - right side of scene - in front of third pair
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(3000.0f, -50.0f, 500.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

         // fifth peeple tree center at vrundavan
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree1->mTextureShader->Use();
            bindShadowUniforms(tree1->mTextureShader.get());
            tree1->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree1ModelMatrix =
                vmath::translate(800.0f, -60.0f, 700.0f) *
                vmath::scale(300.0f, 300.0f, 300.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree1->mTextureShader->SetUniform("u_model", tree1ModelMatrix);
            tree1->mTextureShader->SetUniform("u_view", viewMatrix);
            tree1->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree1->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree1->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree1->Draw(tree1->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

    }

    void drawTree2(bool isBlack = false)
    {   
        // first tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(1600.0f, 40.0f, 1900.0f) *
                vmath::scale(80.0f, 80.0f, 80.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // second tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(1000.0f, 0.0f, 3000.0f) *
                vmath::scale(80.0f, 80.0f, 80.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // third tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(2500.0f, 0.0f, 3000.0f) *
                vmath::scale(80.0f, 80.0f, 80.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // fourth tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(5000.0f, 33.0f, 0.0f) *
                vmath::scale(80.0f, 80.0f, 80.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // fifth tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(5000.0f, 30.0f, 900.0f) *
                vmath::scale(80.0f, 80.0f, 80.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // sixth tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(4100.0f, 0.0f, 3000.0f) *
                vmath::scale(80.0f, 80.0f, 80.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // seventh tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(3000.0f, 100.0f, -1000.0f) *
                vmath::scale(80.0f, 80.0f, 80.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // eight tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(4000.0f, 100.0f, 1900.0f) *
                vmath::scale(80.0f, 80.0f, 80.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // ninth tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(4000.0f, 60.0f, 600.0f) *
                vmath::scale(80.0f, 80.0f, 80.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // tenth tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(2600.0f, 40.0f, 2500.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // eleventh tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(50.0f, 0.0f, 2500.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // twelth tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(-400.0f, 400.0f, 3000.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // thirteen tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(4000.0f, 100.0f, 4000.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // thirteen tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(2000.0f, 300.0f, -1500.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // fourtinth tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(6000.0f, 100.0f, 2500.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // fiftinth tree
        pushMatrix(modelMatrix);    
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

            tree2->mTextureShader->Use();
            bindShadowUniforms(tree2->mTextureShader.get());
            tree2->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 tree2ModelMatrix =
                vmath::translate(4000.0f, 400.0f, 6000.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            tree2->mTextureShader->SetUniform("u_model", tree2ModelMatrix);
            tree2->mTextureShader->SetUniform("u_view", viewMatrix);
            tree2->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            tree2->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            tree2->mTextureShader->SetUniform("u_ApplyToon", false); 

            tree2->Draw(tree2->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
        
    }
    
    void drawWhiteBull(bool isBlack = false)
    {
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            whiteBull->mTextureShader->Use();
            bindShadowUniforms(whiteBull->mTextureShader.get());
            whiteBull->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 whiteBullModelMatrix =
                vmath::translate(2500.0f, 50.0f, 1500.0f) *
                vmath::scale(50.0f, 50.0f, 50.0f) *
                vmath::rotate(80.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            whiteBull->mTextureShader->SetUniform("u_model", whiteBullModelMatrix);
            whiteBull->mTextureShader->SetUniform("u_view", viewMatrix);
            whiteBull->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            whiteBull->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            whiteBull->mTextureShader->SetUniform("u_ApplyToon", false); 
            
            whiteBull->Draw(whiteBull->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

    }

    void drawTreeKatta(bool isBlack = false)
    {
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            treeKatta->mTextureShader->Use();
            bindShadowUniforms(treeKatta->mTextureShader.get());
            treeKatta->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 treeKattaModelMatrix =
                vmath::translate(800.0f, 3.0f, 700.0f) *
                vmath::scale(150.0f, 150.0f, 150.0f) *
                vmath::rotate(90.0f, 0.0f, 1.0f, 0.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            treeKatta->mTextureShader->SetUniform("u_model", treeKattaModelMatrix);
            treeKatta->mTextureShader->SetUniform("u_view", viewMatrix);
            treeKatta->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            treeKatta->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            treeKatta->mTextureShader->SetUniform("u_ApplyToon", false); 
            
            treeKatta->Draw(treeKatta->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

    }
    
    void drawGavkari(bool isBlack = false)
    {   
        // nand baba
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            nandBaba->mTextureShader->Use();
            bindShadowUniforms( nandBaba->mTextureShader.get());
            nandBaba->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4  nandBabaModelMatrix =
                    vmath::translate(1000.0f, 0.0f, 700.0f) *
                    vmath::rotate(90.0f, 0.0f, 1.0f, 0.0f) *
                    vmath::scale(0.3f, 0.3f, 0.3f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            nandBaba->mTextureShader->SetUniform("u_model",  nandBabaModelMatrix);
            nandBaba->mTextureShader->SetUniform("u_view", viewMatrix);
            nandBaba->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            nandBaba->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            nandBaba->mTextureShader->SetUniform("u_ApplyToon", false); 
            
            nandBaba->Draw(nandBaba->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // gavkari 1 talk with nandbaba
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            gavkari1->mTextureShader->Use();
            bindShadowUniforms( gavkari1->mTextureShader.get());
            gavkari1->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4  gavkari1ModelMatrix =
                    vmath::translate(1030.0f, 25.0f, 710.0f) *
                    vmath::rotate(-130.0f, 0.0f, 1.0f, 0.0f) *
                    vmath::scale(25.0f, 25.0f, 25.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            gavkari1->mTextureShader->SetUniform("u_model",  gavkari1ModelMatrix);
            gavkari1->mTextureShader->SetUniform("u_view", viewMatrix);
            gavkari1->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            gavkari1->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            gavkari1->mTextureShader->SetUniform("u_ApplyToon", false); 
            
            gavkari1->Draw(gavkari1->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // gavkari 3talk with nandbaba 
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            gavkari3->mTextureShader->Use();
            bindShadowUniforms(gavkari3->mTextureShader.get());
            gavkari3->mTextureShader->SetUniform("isBlack", isBlack);
            
            vmath::mat4  gavkari3ModelMatrix =
                    vmath::translate(1000.0f, 28.0f, 670.0f) *
                    vmath::rotate(40.0f, 0.0f, 1.0f, 0.0f) *
                    vmath::scale(28.0f, 28.0f, 28.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            gavkari3->mTextureShader->SetUniform("u_model",  gavkari3ModelMatrix);
            gavkari3->mTextureShader->SetUniform("u_view", viewMatrix);
            gavkari3->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            gavkari3->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            gavkari3->mTextureShader->SetUniform("u_ApplyToon", false); 
            
            gavkari3->Draw(gavkari3->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();

        // gavkari 2 ladies with water matka
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            gavkari2_ladies->mTextureShader->Use();
            bindShadowUniforms( gavkari2_ladies->mTextureShader.get());
            gavkari2_ladies->mTextureShader->SetUniform("isBlack", isBlack);
            
            vmath::mat4  gavkari2_ladiesModelMatrix =
                    vmath::translate(1400.0f, 30.0f, 900.0f) *
                    vmath::rotate(30.0f, 0.0f, 1.0f, 0.0f) *
                    vmath::scale(30.0f, 30.0f, 30.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            gavkari2_ladies->mTextureShader->SetUniform("u_model",  gavkari2_ladiesModelMatrix);
            gavkari2_ladies->mTextureShader->SetUniform("u_view", viewMatrix);
            gavkari2_ladies->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            gavkari2_ladies->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            gavkari2_ladies->mTextureShader->SetUniform("u_ApplyToon", false); 
            
            gavkari2_ladies->Draw(gavkari2_ladies->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
        
        // yashoda 
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            yashoda->mTextureShader->Use();
            bindShadowUniforms( yashoda->mTextureShader.get());
            yashoda->mTextureShader->SetUniform("isBlack", isBlack);
            
            vmath::mat4  yashodaModelMatrix =
                    vmath::translate(-50.0f, 30.0f, 700.0f) *
                    vmath::rotate(460.0f, 0.0f, 1.0f, 0.0f) *
                    vmath::scale(25.0f, 25.0f, 25.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            yashoda->mTextureShader->SetUniform("u_model",  yashodaModelMatrix);
            yashoda->mTextureShader->SetUniform("u_view", viewMatrix);
            yashoda->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            yashoda->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            yashoda->mTextureShader->SetUniform("u_ApplyToon", false); 
            
            yashoda->Draw(yashoda->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
        
        // gavkari 4 ladies talk with yashoda
        pushMatrix(modelMatrix);
        {   
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            gavkari4_ladies->mTextureShader->Use();
            bindShadowUniforms( gavkari4_ladies->mTextureShader.get());
            gavkari4_ladies->mTextureShader->SetUniform("isBlack", isBlack);
            
            vmath::mat4  gavkari4_ladiesModelMatrix =
                    vmath::translate(0.0f, 30.0f, 680.0f) *
                    vmath::rotate(-50.0f, 0.0f, 1.0f, 0.0f) *
                    vmath::scale(25.0f, 25.0f, 25.0f);

                // vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                // vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]) *
                // vmath::rotate(gModelRotate[0], 0.0f, 1.0f, 0.0f);

            gavkari4_ladies->mTextureShader->SetUniform("u_model",  gavkari4_ladiesModelMatrix);
            gavkari4_ladies->mTextureShader->SetUniform("u_view", viewMatrix);
            gavkari4_ladies->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            gavkari4_ladies->mTextureShader->SetSampler2D("u_GGXLUT", 0, 5);
            gavkari4_ladies->mTextureShader->SetUniform("u_ApplyToon", false); 
            
            gavkari4_ladies->Draw(gavkari4_ladies->mTextureShader);

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
        // sceneCamera->time = 1;
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

        // // rain
        // if (rain->alpha > 0.0f)
        // {
        //     rain->alpha -= 0.002f;
        // }

        // modelLoader.uninitialize();
    }
};
