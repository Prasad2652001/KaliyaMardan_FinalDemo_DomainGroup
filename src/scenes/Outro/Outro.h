#pragma once
#include "../../utils/common.h"
#include "../../effects/cubemap/Cubemap.h"
#include "../../utils/camera/BezierCamera.h"
#include "../Scene.h"

class OutroScene : public Scene
{
public:
    CubeMap *cubeMap;
    BezierCamera sc1;

    enum sceneEventIds
    {
        START_T,
        FADE_IN,
        SC_T1,
        FADE_OUT,
        END_T,
    };

    OutroScene()
    {
        cubeMap = new CubeMap();
        sceneCamera = new BezierCamera();
    }

    bool initialize()
    {
        const char *facesLight[] =
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
            PrintLog("OutroScene : Failed to initialize CubeMap\n");
            return FALSE;
        }

        sceneEvents = new EventManager(
        {
            {START_T,  {0.0f, 10.0f}},
            {FADE_IN,  {0.0f, 2.0f}},
            {SC_T1,    {0.0f, 8.0f}},
            {FADE_OUT, {8.0f, 2.0f}},
            {END_T,    {10.0f, 0.0f}}
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
            {0.0f, 0.0f, 600.0f},
            {0.0f, 0.0f, 600.0f}
        };

        std::vector<float> yawGlobalSC1 =
        {
            0.0f,
            0.0f
        };

        std::vector<float> pitchGlobalSC1 =
        {
            0.0f,
            0.0f
        };

        std::vector<float> fovGlobalSC1 =
        {
            -45.0f,
            -45.0f
        };

        sc1.initialize();
        sc1.setBezierPoints(bezierPointsSC1, yawGlobalSC1, pitchGlobalSC1, fovGlobalSC1);
        sc1.update();

        sceneCamera = &sc1;
    }

    void display()
    {
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

        pushMatrix(modelMatrix);
        {
            modelMatrix = vmath::scale(1.0f, 1.0f, 1.0f);

            if (sceneEvents->isEventInProgress(FADE_IN))
                commonShaders->overlayColorShader->draw(
                    modelMatrix, 0.0f, 0.0f, 0.0f,
                    lerp(sceneEvents->getEventTime(FADE_IN), 1.0f, 0.0f));
            else
                commonShaders->overlayColorShader->draw(
                    modelMatrix, 0.0f, 0.0f, 0.0f,
                    lerp(sceneEvents->getEventTime(FADE_OUT), 0.0f, 1.0f));
        }
        modelMatrix = popMatrix();
    }

    void update()
    {
        sceneEvents->increment();

        if (sceneEvents->isEventInProgress(SC_T1))
        {
            sceneCamera = &sc1;
            sceneCamera->time = sceneEvents->getEventTime(SC_T1);
        }

        if (sceneEvents->isEventComplete(END_T))
        {
            isSceneComplete = true;
        }
    }

    void uninitialize()
    {
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
