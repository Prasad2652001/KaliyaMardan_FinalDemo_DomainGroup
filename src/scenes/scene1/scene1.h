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

extern float gModelTranslate[3];
extern float gModelRotate[3];
extern float gModelScale[3];

extern bool gEnableLighting;
extern bool gEnableTexture;
// extern float gDeltaTime;

class DemoScene1 : public Scene
{
public:
    CubeMap* cubeMap;
    BezierCamera sc1;

    // ModelShader shoesRenderer;

    glshaderprogram* programStaticPBR;
    glshaderprogram* programAnimatedPBR;   // for dynamic model  
    SceneLight* lightManager;

    std::unique_ptr<Core::Model> mKart;

    glmodel* animatedModel;                // for dynamic model 
    bool bPlayAnimation;                   // for dynamic model 
    int animationIndex;                    // for dynamic model 

    GLuint brdfLookUp = 0;

    enum sceneEventIds
    {
        START_T,
        FADE_IN,
        SC_T1,
        FADE_OUT,
        END_T,
    };

    DemoScene1()
    {
        cubeMap = new CubeMap();
        sceneCamera = new BezierCamera();

        programStaticPBR = NULL;
        programAnimatedPBR = NULL;         // for dynamic model 
        lightManager = NULL;
        animatedModel = NULL;              // for dynamic model 
        bPlayAnimation = true;             // for dynamic model 
        animationIndex = 0;                // for dynamic model 
    }

    bool initialize()
    {
        const char* facesLight[] =
        {
            ".\\assets\\textures\\DayCubeMap-2\\px.png",
            ".\\assets\\textures\\DayCubeMap-2\\nx.png",
            ".\\assets\\textures\\DayCubeMap-2\\py.png",
            ".\\assets\\textures\\DayCubeMap-2\\ny.png",
            ".\\assets\\textures\\DayCubeMap-2\\pz.png",
            ".\\assets\\textures\\DayCubeMap-2\\nz.png"
        };

        if (!cubeMap->initialize(facesLight))
        {
            PrintLog("DemoScene1 : Failed to initialize CubeMap\n");
            return FALSE;
        }

        // shoesRenderer.initialize_ModelShaderObject(commonModels->ModelShoes);

        // Scene2 style shader init
        programStaticPBR = new glshaderprogram({
            "./src/shaders/modelgltf/pbrStatic.vert",
            "./src/shaders/modelgltf/pbrMain.frag"
            });

        // for dynamic model  : animated shader init
        programAnimatedPBR = new glshaderprogram({
            "./src/shaders/modelgltf/pbrAnimated.vert",
            "./src/shaders/modelgltf/pbrMain.frag"
            });

        // Scene2 style GLB loading
        mKart = std::make_unique<Core::Model>();
        mKart->LoadModel("./assets/models/girl/scene.glb");

        // for dynamic model  : animated / rigged model loading
        animatedModel = new glmodel(
            "./assets/models/test/scene.glb",
            aiProcessPreset_TargetRealtime_Quality,
            true
        );

        lightManager = new SceneLight();
        lightManager->addDirectionalLights({
            DirectionalLight(vec3(1.0f, 1.0f, 1.0f), 0.8f, vec3(0.0f, -1.0f, -1.0f))
            });
        lightManager->setAmbient(vec3(0.25f, 0.25f, 0.25f));

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
            {-200.0f, 100.0f, 900.0f},
            { 200.0f, 100.0f, 900.0f}
        };

        std::vector<float> yawGlobalSC1 = { 0.0f, 20.0f };
        std::vector<float> pitchGlobalSC1 = { 0.0f, 0.0f };
        std::vector<float> fovGlobalSC1 = { -45.0f, -45.0f };

        sc1.initialize();
        sc1.setBezierPoints(bezierPointsSC1, yawGlobalSC1, pitchGlobalSC1, fovGlobalSC1);
        sc1.update();

        sceneCamera = &sc1;
    }

    void drawKartModel(bool isBlack = false)
    {
        if (!mKart)
            return;

        pushMatrix(modelMatrix);
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            mKart->mTextureShader->Use();
            mKart->mTextureShader->SetUniform("isBlack", isBlack);

            vmath::mat4 kartModelMatrix =
                vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
                vmath::rotate(gModelRotate[0], 1.0f, 0.0f, 0.0f) *
                vmath::rotate(gModelRotate[1], 0.0f, 1.0f, 0.0f) *
                vmath::rotate(gModelRotate[2], 0.0f, 0.0f, 1.0f) *
                vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]);

            mKart->mTextureShader->SetUniform("u_model", kartModelMatrix);
            mKart->mTextureShader->SetUniform("u_view", viewMatrix);
            mKart->mTextureShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            mKart->mTextureShader->SetSampler2D("u_GGXLUT", brdfLookUp, 5);

            mKart->Draw(mKart->mTextureShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    // for dynamic model 
    void drawAnimatedModel()
    {
        if (!programAnimatedPBR || !animatedModel || !lightManager)
            return;

        programAnimatedPBR->use();

        glUniformMatrix4fv(programAnimatedPBR->getUniformLocation("pMat"), 1, GL_FALSE, perspectiveProjectionMatrix);
        glUniformMatrix4fv(programAnimatedPBR->getUniformLocation("vMat"), 1, GL_FALSE, viewMatrix);

        vec3 eyePos = USE_FPV_CAM ? camera.getEye() : globalBezierCamera->getEye();
        glUniform3fv(programAnimatedPBR->getUniformLocation("viewPos"), 1, eyePos);

        glUniform1i(programAnimatedPBR->getUniformLocation("specularGloss"), GL_FALSE);
        glUniform1f(programAnimatedPBR->getUniformLocation("clipy"), 0.0f);

        lightManager->setLightUniform(programAnimatedPBR, false);

        mat4 animModelMatrix =
            vmath::translate(gModelTranslate[0], gModelTranslate[1], gModelTranslate[2]) *
            vmath::rotate(gModelRotate[0], 1.0f, 0.0f, 0.0f) *
            vmath::rotate(gModelRotate[1], 0.0f, 1.0f, 0.0f) *
            vmath::rotate(gModelRotate[2], 0.0f, 0.0f, 1.0f) *
            vmath::scale(gModelScale[0], gModelScale[1], gModelScale[2]);

        glUniformMatrix4fv(programAnimatedPBR->getUniformLocation("mMat"), 1, GL_FALSE, animModelMatrix);

        GLint boneLoc = programAnimatedPBR->getUniformLocation("finalBonesMatrices[0]");
        if (boneLoc != -1)
        {
            animatedModel->setBoneMatrixUniform(boneLoc, animationIndex);
        }

        animatedModel->draw(programAnimatedPBR);
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

        updateGlobalViewMatrix();

        pushMatrix(modelMatrix);
        {
            modelMatrix = modelMatrix * vmath::scale(2000000.0f, 2000000.0f, 2000000.0f);
            cubeMap->display();
        }
        modelMatrix = popMatrix();

        // OBJ shoe model added
        // pushMatrix(modelMatrix);
        // {
        //     modelMatrix = modelMatrix *
        //         vmath::translate(0.0f, -20.0f, -150.0f) *
        //         vmath::scale(18.0f, 18.0f, 18.0f) *
        //         vmath::rotate(-90.0f, 1.0f, 0.0f, 0.0f);

        //     shoesRenderer.render_Model();
        // }
        // modelMatrix = popMatrix();

        // Scene2 style GLB draw
        // drawKartModel();

        // for dynamic model  : animated model draw
        drawAnimatedModel();

        // fade
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

        // for dynamic model  : animated model update
        if (animatedModel && bPlayAnimation)
        {
            animatedModel->update((float)gDeltaTime, animationIndex, 0, 0.0f, true);
        }

        if (sceneEvents->isEventComplete(END_T))
            isSceneComplete = true;
    }

    void uninitialize()
    {
        if (animatedModel)
        {
            delete animatedModel;
            animatedModel = NULL;
        }

        if (lightManager)
        {
            delete lightManager;
            lightManager = NULL;
        }

        if (programAnimatedPBR)
        {
            delete programAnimatedPBR;
            programAnimatedPBR = NULL;
        }

        if (programStaticPBR)
        {
            delete programStaticPBR;
            programStaticPBR = NULL;
        }

        mKart.reset();

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

