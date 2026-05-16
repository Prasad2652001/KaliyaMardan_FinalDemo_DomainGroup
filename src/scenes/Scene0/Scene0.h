/**
 * TODO:
 * scene : railway station
 * 2. toon shader
 */
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

extern Camera camera;
extern BezierCamera *globalBezierCamera;

class DemoScene0 : public Scene
{
public:
    CubeMap *cubeMap;
    BezierCamera sc1;

    GLuint cubeMapTexture;
    Terrain *terrain;
    WaterMatrix *waterMatrix;


    // ModelShader shoesRenderer;

    glshaderprogram *programStaticPBR;
    SceneLight *lightManager;

    std::unique_ptr<Core::Model> mPillar;
    std::unique_ptr<Core::Model> mBoy;

    GLuint brdfLookUp = 0;
    GLuint walltexture;
    GLuint texture_wall;
    GLuint texture_wall2;
    GLuint texture_floor;
    GLuint textureIDs[3];

    enum sceneEventIds
    {
        START_T,
        FADE_IN,
        SC_T1,
        FADE_OUT,
        END_T,
    };

    DemoScene0()
    {
        cubeMap = new CubeMap();
        sceneCamera = new BezierCamera();
        terrain = new Terrain(4. * 40.);
        waterMatrix = new WaterMatrix(300. * 400.);

        programStaticPBR = NULL;
        lightManager = NULL;
    }

    bool initialize()
    {
         const char *facesLight[] =
        {
            ".\\assets\\textures\\terrain\\cubemap_light\\px.png",
            ".\\assets\\textures\\terrain\\cubemap_light\\nx.png",
            ".\\assets\\textures\\terrain\\cubemap_light\\py.png",
            ".\\assets\\textures\\terrain\\cubemap_light\\ny.png",
            ".\\assets\\textures\\terrain\\cubemap_light\\pz.png",
            ".\\assets\\textures\\terrain\\cubemap_light\\nz.png"
        };

        if (!cubeMap->initialize(facesLight))
        {
            PrintLog(" Failed to initialize CubeMap\n");
            return FALSE;
        }

        // shoesRenderer.initialize_ModelShaderObject(commonModels->ModelShoes);

        programStaticPBR = new glshaderprogram({"./src/shaders/modelgltf/pbrStatic.vert",
                                                "./src/shaders/modelgltf/pbrMain.frag"});

 /*        mBoy = std::make_unique<Core::Model>();
        mBoy->LoadModel("./assets/models/scene_0/boy.glb"); // main line
        // mBoy->LoadModel("./assets/models/SceneCave/Chatri.glb"); testing

        mPillar = std::make_unique<Core::Model>();
        mPillar->LoadModel("./assets/models/scene_0/PillarModel.glb"); */

        lightManager = new SceneLight();
        lightManager->addDirectionalLights({DirectionalLight(vec3(0.55f), 10.0f, vec3(0.0f, -1.0f, -1.0f)),
                                            DirectionalLight(vec3(0.55f), 10.0f, vec3(0.0f, -1.0f, 1.0f)),
                                            DirectionalLight(vec3(0.30f), 10.0f, vec3(0.0f, -1.0f, 0.0f))});
        lightManager->addSpotLights({SpotLight(vec3(1.0f), 10.0f, vec3(0.0f, 10.0f, 0.0f), 100.0f,
                                               vec3(0.0f, -0.9f, -0.3f), 20.0f, 22.0f)});
        lightManager->setAmbient(vec3(0.05f));

        sceneEvents = new EventManager(
            {{START_T, {0.0f, 15.0f}},
             {FADE_IN, {0.0f, 2.0f}},
             {SC_T1, {0.0f, 13.0f}},
             {FADE_OUT, {13.0f, 2.0f}},
             {END_T, {15.0f, 0.0f}}},
            true);


        // Water
        waterMatrix->initialize();

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

       /*  const char *textureFiles[] = {
            "./assets/textures/scene_0/wall.png",
            "./assets/textures/scene_0/wall2.png",
            "./assets/textures/scene_0/floor.png"}; */

       /*  // Load single texture
        if (LoadPNGImage(&texture_wall, "./assets/textures/scene_0/wall.png") == FALSE)
        {
            PrintLog("Failed to Wall load texture\n");
            return false;
        }

        if (LoadPNGImage(&texture_wall2, "./assets/textures/scene_0/wall2.png") == FALSE)
        {
            PrintLog("Failed to Wall2 load texture\n");
            return false;
        }

        if (LoadPNGImage(&texture_floor, "./assets/textures/scene_0/floor.png") == FALSE)
        {
            PrintLog("Failed to floor load texture\n");
            return false;
        } */

        setupCamera();
        
        isInitialized = true;
        isSceneComplete = false;
        return TRUE;
    }

    bool LoadTextures(GLuint *textures, const char *filenames[], size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            if (LoadPNGImage(&textures[i], filenames[i]) == FALSE)
            {
                PrintLog("Failed to load texture: %s\n", filenames[i]);
                return false;
            }
            else
            {
                PrintLog("Succesfully loaded texture: %s\n", filenames[i]);
            }
        }
        return true;
    }

    void setupCamera()
    {
        std::vector<std::vector<float>> bezierPointsSC1 =
            {
                {0.0f, 10.0f, 60.0f},
                {0.0f, 0.0f, 0.0f}};

        std::vector<float> yawGlobalSC1 = {-90.0f, -90.0f}; // around Y axis  ... side look
        std::vector<float> pitchGlobalSC1 = {0.0f, 0.0f};   // around X axis  ... up down
        std::vector<float> fovGlobalSC1 = {-45.0f, -45.0f}; // aroudn Z axis  ... zoom level

        sc1.initialize();
        sc1.setBezierPoints(bezierPointsSC1, yawGlobalSC1, pitchGlobalSC1, fovGlobalSC1);
        sc1.update();

        sceneCamera = &sc1;
    }

    void drawPillarModel(mat4 transformedModelMatrix, bool isBlack = false)
    {
        if (!mPillar)
            return;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mPillar->mTextureShader->Use();
        mPillar->mTextureShader->SetUniform("isBlack", isBlack);

        mPillar->mTextureShader->SetUniform("u_model", transformedModelMatrix);
        mPillar->mTextureShader->SetUniform("u_view", viewMatrix);
        mPillar->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
        mPillar->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);

        mPillar->Draw(mPillar->mTextureShader);

        glDisable(GL_BLEND);
    }

    void display()
    {
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

        modelMatrix = mat4::identity();
        perspectiveProjectionMatrix = vmath::perspective(
            45.0f,
            (GLfloat)giWindowWidth / (GLfloat)giWindowHeight,
            0.1f,
            10000000.0f);

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


        /* // scene
        // Load Floor
        pushMatrix(modelMatrix);
        {
            modelMatrix = modelMatrix *
                          vmath::translate(0.0f, 1.0f, 20.0f) *
                          vmath::scale(50.0f, 1.0f, 20.0f) *
                          vmath::rotate(90.0f, 1.0f, 0.0f, 0.0f);

            commonShaders->textureShader->drawQuadWithTexture(texture_floor, modelMatrix, viewMatrix, perspectiveProjectionMatrix, 1.0f, 2.5f, 2.5f, vec4(10.0f, 10.0f, 10.0f, 1.0f), true, false, 64.0f);
        }
        modelMatrix = popMatrix();

        // Load bgWall
        pushMatrix(modelMatrix);
        {
            modelMatrix = modelMatrix *
                          vmath::translate(0.0f, 20.0f, 0.0f) *
                          vmath::scale(50.0f, 20.0f, 1.0f);

            commonShaders->textureShader->drawQuadWithTexture(texture_wall, modelMatrix, viewMatrix, perspectiveProjectionMatrix, 1.0f, 1.0f, 1.0f, vec4(10.0f, 10.0f, 10.0f, 1.0f), true, false, 64.0f);
        }
        modelMatrix = popMatrix();
 */
        /*   // Load Pillar
        {
            //  Pillar - 1
            pushMatrix(modelMatrix);
            {
                modelMatrix = modelMatrix *
                              vmath::translate(30.0f, 0.0f, 10.0f) *
                              vmath::scale(4.0f, 10.0f, 4.0f);
                drawPillarModel(modelMatrix);
            }
            modelMatrix = popMatrix();

            //  Pillar - 2
            pushMatrix(modelMatrix);
            {
                modelMatrix = modelMatrix *
                              vmath::translate(-10.0f, 0.0f, 20.0f) *
                              vmath::scale(4.0f, 10.0f, 4.0f);
                drawPillarModel(modelMatrix);
            }
            modelMatrix = popMatrix();

            //  Pillar - 3
            pushMatrix(modelMatrix);
            {
                modelMatrix = modelMatrix *
                              vmath::translate(-20.0f, 0.0f, 40.0f) *
                              vmath::scale(8.0f, 10.0f, 4.0f);
                drawPillarModel(modelMatrix);
            }
            modelMatrix = popMatrix();
        }
        */
      /*   // Load Model
        if (mBoy)
        {
            pushMatrix(modelMatrix);
            {
                modelMatrix = modelMatrix *
                              vmath::translate(-4.0f, 4.0f, 24.0f) *
                              vmath::scale(7.0f, 7.0f, 7.0f) *
                              vmath::rotate(55.0f, 0.0f, 1.0f, 0.0f);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                mBoy->mTextureShader->Use();
                mBoy->mTextureShader->SetUniform("isBlack", false);

                mBoy->mTextureShader->SetUniform("u_model", modelMatrix);
                mBoy->mTextureShader->SetUniform("u_view", viewMatrix);
                mBoy->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
                mBoy->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);
                mBoy->mTextureShader->SetUniform("u_LightPosition", vec4(10.0f, 10.0f, 10.0f, 1.0f)); // TODO set the light position  ... lightManager->getPointLightPosition()
                mBoy->mTextureShader->SetUniform("u_ApplyToon", true);
                mBoy->mTextureShader->SetUniform("u_ApplyRim", false);
                mBoy->mTextureShader->SetUniform("u_ApplySpecular", false);
                mBoy->Draw(mBoy->mTextureShader);
                glDisable(GL_BLEND);
            }
            modelMatrix = popMatrix();
        } */
    }

    
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
        sceneCamera->time = sceneEvents->getEventTime(START_T);
        sceneEvents->increment();

        if (sceneEvents->isEventInProgress(SC_T1))
        {
            sceneCamera = &sc1;
            sceneCamera->time = sceneEvents->getEventTime(SC_T1);
        }

        if (sceneEvents->isEventComplete(END_T))
            isSceneComplete = true;
    }

    void uninitialize()
    {
        if (lightManager)
        {
            delete lightManager;
            lightManager = NULL;
        }

        if (programStaticPBR)
        {
            delete programStaticPBR;
            programStaticPBR = NULL;
        }

        if (cubeMap)
        {
            cubeMap->uninitialize();
            delete cubeMap;
            cubeMap = NULL;
        }

        if (sceneEvents)
        {
            delete sceneEvents;
            sceneEvents = NULL;
        }

        sceneCamera = NULL;
        isInitialized = false;
        isSceneComplete = false;
    }
};
