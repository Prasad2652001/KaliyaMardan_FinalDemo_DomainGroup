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

        terrain = new Terrain(10 * 100.);
        waterMatrix = new WaterMatrix(300. * 40.);
        godRaysShader = new GodRaysShader();
    }

    bool initialize()
    {
        // // Shaders
        shineShader->initialize();
        // // Toy room
        // //////////////////////////////////////////////////////// gltf model

        mToyMarriage = std::make_unique<Core::Model>();
        mToyMarriage->LoadModel("assets/models/scene1_models/spidey.glb");

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

        mCross = std::make_unique<Core::Model>();
        mCross->LoadModel("./assets/models/scene1/cross.glb");

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

        // sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal, fovGlobal);
        // sceneCamera->update();
         pushMatrix(modelMatrix);
        {
            modelMatrix = modelMatrix * vmath::translate(4600.000000f, 800.000000f, 4700.000000f) * vmath::scale(1000000.0f, 1000000.0f, 1000000.0f);
            cubemap->display();
        }
        modelMatrix = popMatrix();

        // TERRAIN GODRAYS
        //displayGodRays();

        // DRAW TERRAIN IN BLACK
        pushMatrix(modelMatrix);
        {
            terrain->up = 1;
            terrain->draw(false);
        }
        modelMatrix = popMatrix();

        pushMatrix(modelMatrix);
        {
            drawToyMarriage();
        }
        modelMatrix = popMatrix();

        /*
        // Room
        pushMatrix(modelMatrix);
        {
            drawRoom();
        }
        modelMatrix = popMatrix();

        // Baby
        pushMatrix(modelMatrix);
        {
            drawGltfBabyModel();
        }
        modelMatrix = popMatrix();
        // Baby
        pushMatrix(modelMatrix);
        {
            drawCross();
        }
        modelMatrix = popMatrix(); */


        // Fading
        /* pushMatrix(modelMatrix);
        {
            modelMatrix = vmath::scale(1.0f, 1.0f, 1.0f);
            if (sceneEvents->isEventInProgress(FADE_IN))
                commonShaders->overlayColorShader->draw(modelMatrix, 0.0f, 0.0f, 0.0f, lerp(sceneEvents->getEventTime(FADE_IN), 1.0f, 0.0f));
            else
                commonShaders->overlayColorShader->draw(modelMatrix, 0.0f, 0.0f, 0.0f, lerp(sceneEvents->getEventTime(FADE_OUT), 0.0f, 1.0f));
        }
        modelMatrix = popMatrix(); */

        // Marrriage stage shadow
       /*  pushMatrix(modelMatrix);
        {
            modelMatrix = modelMatrix * vmath::translate(-10.0f + 4.40f, 1.0f + -0.80f + 0.20f, 18.0f + -24.80f) * vmath::rotate(90.0f, 1.0f, 0.0f, 0.0f) * vmath::scale(9.04f + 0.50f, 6.03f + 0.50f, 1.0f);
            commonShaders->textureShader->drawQuadWithTexture(texture_marriageStage, modelMatrix, viewMatrix, perspectiveProjectionMatrix, 1.0f);
        }
        modelMatrix = popMatrix();

        // Boy laying shadow
        pushMatrix(modelMatrix);
        {
            modelMatrix = modelMatrix * vmath::translate(-10.0f + -8.45f, 1.0f + -0.70f, 18.0f + -10.0f) * vmath::rotate(90.0f, 1.0f, 0.0f, 0.0f) * vmath::rotate(180.0f, 0.0f, 1.0f, 0.0f) * vmath::rotate(-74.0f, 0.0f, 0.0f, 1.0f) * vmath::scale(9.04f + -1.5f, 6.03f + 5.85f, 1.0f);
            commonShaders->textureShader->drawQuadWithTexture(texture_layingBoy, modelMatrix, viewMatrix, perspectiveProjectionMatrix, 1.0f);
        }
        modelMatrix = popMatrix(); */

        // sceneCamera->displayBezierCurve();
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
            static float anglecounter = 0.01f;

            // Use the shader program object
            mToyMarriage->mTextureShader->Use();
            modelMatrix = vmath::translate(-6.000000f, 0.000000f, -6.000000f);
            vmath::mat4 scaleMatrix = vmath::scale(12.000000f, 12.000000f, 12.000000f);
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
            modelMatrix = vmath::translate(-35.000000f, 38.000000f, 11.000000f);
            vmath::mat4 scaleMatrix = vmath::scale(2.000000f + -1.800000f, 2.000000f + -1.800000f, 2.000000f + -1.800000f);
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
        // EVENT UPDATE
        // sceneCamera->time = globalTime;
        sceneCamera->time = sceneEvents->getEventTime(SC_T);
        sceneEvents->increment();

        /// Completing scene hereq
        if (sceneEvents->isEventComplete(START_T))
        {
            isSceneComplete = true;
        }
    }

    void uninitialize()
    {
    }
};
