#pragma once
#include "../../utils/common.h"
#include "../../shaders/model/Model_Shader.h"
#include "../../effects/cubemap/Cubemap.h"
#include "../../utils/camera/BezierCamera.h"
#include "../../utils/camera/Camera.h"
#include "../../utils/gltf/Model.h"
#include "../../utils/gltf/TextureModel.h"
#include "../../shaders/modelgltf/glshaderloader.h"
#include "../../shaders/modelgltf/glLight.h"
#include "../../shaders/modelgltf/glmodelloader.h"
#include "../../utils/EventManager.h"
#include "../Scene.h"

extern Camera camera;
extern BezierCamera* globalBezierCamera;

class DemoScene2 : public Scene
{
public:
    CubeMap* cubeMap;
    BezierCamera sc1;

    // ModelShader shoesRenderer;

    glshaderprogram* programStaticPBR;
    SceneLight* lightManager;

    std::unique_ptr<Core::Model> mSwing;

    GLuint brdfLookUp = 0;

    enum sceneEventIds
    {
        START_T,
        FADE_IN,
        SC_T1,
        FADE_OUT,
        END_T,
    };

    DemoScene2()
    {
        cubeMap = new CubeMap();
        sceneCamera = new BezierCamera();

        programStaticPBR = NULL;
        lightManager = NULL;
    }

    bool initialize()
    {
        const char* facesLight[] =
        {
            ".\\assets\\textures\\DayCubeMap\\px.png",
            ".\\assets\\textures\\DayCubeMap\\nx.png",
            ".\\assets\\textures\\DayCubeMap\\py.png",
            ".\\assets\\textures\\DayCubeMap\\ny.png",
            ".\\assets\\textures\\DayCubeMap\\pz.png",
            ".\\assets\\textures\\DayCubeMap\\nz.png"
        };

        if (!cubeMap->initialize(facesLight))
        {
            PrintLog("DemoScene2 : Failed to initialize CubeMap\n");
            return FALSE;
        }

        // shoesRenderer.initialize_ModelShaderObject(commonModels->ModelShoes);

        programStaticPBR = new glshaderprogram({
            "./src/shaders/modelgltf/pbrStatic.vert",
            "./src/shaders/modelgltf/pbrMain.frag"
            });

        mSwing = std::make_unique<Core::Model>();
        mSwing->LoadModel("./assets/models/Swing/scene.glb");

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

        sceneEvents = new EventManager(
            {
                {START_T,  {0.0f, 15.0f}},
                {FADE_IN,  {0.0f, 2.0f}},
                {SC_T1,    {0.0f, 13.0f}},
                {FADE_OUT, {13.0f, 2.0f}},
                {END_T,    {15.0f, 0.0f}}
            }, true);

        setupCamera();

        isInitialized = true;
        isSceneComplete = false;
        return TRUE;
    }

    void setupCamera()
    {
        std::vector<std::vector<float>> bezierPointsSC1 =
        {
            {0.0f, 200.0f, 1200.0f},
            {0.0f, 100.0f, 700.0f}
        };

        std::vector<float> yawGlobalSC1 = { 0.0f, 0.0f };
        std::vector<float> pitchGlobalSC1 = { 10.0f, 0.0f };
        std::vector<float> fovGlobalSC1 = { -45.0f, -45.0f };

        sc1.initialize();
        sc1.setBezierPoints(bezierPointsSC1, yawGlobalSC1, pitchGlobalSC1, fovGlobalSC1);
        sc1.update();

        sceneCamera = &sc1;
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
                vmath::translate(200.0f, -30.0f, -250.0f) *
                vmath::scale(22.0f, 22.0f, 22.0f) *
                vmath::rotate(-90.0f, 1.0f, 0.0f, 0.0f);

            mSwing->mTextureShader->SetUniform("u_model", swingModelMatrix);
            mSwing->mTextureShader->SetUniform("u_view", viewMatrix);
            mSwing->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            mSwing->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);

            mSwing->Draw(mSwing->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
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
            modelMatrix = modelMatrix * vmath::scale(2000000.0f, 2000000.0f, 2000000.0f);
            cubeMap->display();
        }
        modelMatrix = popMatrix();

        // OBJ shoe model
        // pushMatrix(modelMatrix);
        // {
        //     modelMatrix = modelMatrix *
        //         vmath::translate(0.0f, -20.0f, -150.0f) *
        //         vmath::scale(18.0f, 18.0f, 18.0f) *
        //         vmath::rotate(-90.0f, 1.0f, 0.0f, 0.0f);

        //     shoesRenderer.render_Model();
        // }
        // modelMatrix = popMatrix();

        drawSwingModel();

        pushMatrix(modelMatrix);
        {
            modelMatrix = vmath::scale(1.0f, 1.0f, 1.0f);

            if (sceneEvents->isEventInProgress(FADE_IN))
            {
                commonShaders->overlayColorShader->draw(
                    modelMatrix,
                    0.0f, 0.0f, 0.0f,
                    lerp(sceneEvents->getEventTime(FADE_IN), 1.0f, 0.0f)
                );
            }
            else
            {
                commonShaders->overlayColorShader->draw(
                    modelMatrix,
                    0.0f, 0.0f, 0.0f,
                    lerp(sceneEvents->getEventTime(FADE_OUT), 0.0f, 1.0f)
                );
            }
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

        mSwing.reset();

        // shoesRenderer.uninitialize_ModelShaderObject();

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

