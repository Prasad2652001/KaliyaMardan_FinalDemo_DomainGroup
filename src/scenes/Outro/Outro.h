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

    struct FadeEvent
    {
        int fadeInID;
        int fadeOutID;
        GLuint texture;
    };

    enum sceneEventIds
    {
        START_T,
        FADE_IN,
        SC_T1,
        SLIDE_GROUP_FADE_IN,
        SLIDE_GROUP_FADE_OUT,
        SLIDE_TECHSTACK_FADE_IN,
        SLIDE_TECHSTACK_FADE_OUT,
        SLIDE_MUSIC_FADE_IN,
        SLIDE_MUSIC_FADE_OUT,
        SLIDE_GROUPLEAD_FADE_IN,
        SLIDE_GROUPLEAD_FADE_OUT,
        SLIDE_TEAM_FADE_IN,
        SLIDE_TEAM_FADE_OUT,
        SLIDE_SPECIAL_THANKS_FADE_IN,
        SLIDE_SPECIAL_THANKS_FADE_OUT,
        SLIDE_DR_FADE_IN,
        SLIDE_DR_FADE_OUT,
        FADE_OUT,
        END_T,
    };

    GLuint textureIDs[14];
    std::vector<FadeEvent> fadeEvents;

    OutroScene()
    {
        cubeMap = new CubeMap();
        sceneCamera = new BezierCamera();
    }

    // Function to load textures
    bool LoadTextures(GLuint* textures, const char* filenames[], size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            if (LoadPNGImage(&textures[i], filenames[i]) == FALSE)
            {
                PrintLog("Failed to load texture: %s\n", filenames[i]);
                return false;
            }
        }
        return true;
    }

    bool initialize()
    {
        const char *facesLight[] =
        {
            ".\\assets\\textures\\modelCubeMap\\vrundavan\\px.png",
            ".\\assets\\textures\\modelCubeMap\\vrundavan\\nx.png",
            ".\\assets\\textures\\modelCubeMap\\vrundavan\\py.png",
            ".\\assets\\textures\\modelCubeMap\\vrundavan\\ny.png",
            ".\\assets\\textures\\modelCubeMap\\vrundavan\\pz.png",
            ".\\assets\\textures\\modelCubeMap\\vrundavan\\nz.png"
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
            {SLIDE_GROUP_FADE_IN, {0.0f, 2.0f}},
            {SLIDE_GROUP_FADE_OUT, {5.0f, 2.0f}},
            {SLIDE_TECHSTACK_FADE_IN, {7.0f, 2.0f}},
            {SLIDE_TECHSTACK_FADE_OUT, {9.0f, 2.0f}},
            {SLIDE_MUSIC_FADE_IN, {11.0f, 2.0f}},
            {SLIDE_MUSIC_FADE_OUT, {14.0f, 2.0f}},
            {SLIDE_GROUPLEAD_FADE_IN, {17.0f, 2.0f}},
            {SLIDE_GROUPLEAD_FADE_OUT, {20.0f, 2.0f}},
            {SLIDE_TEAM_FADE_IN, {23.0f, 2.0f}},
            {SLIDE_TEAM_FADE_OUT, {26.0f, 2.0f}},
            {SLIDE_SPECIAL_THANKS_FADE_IN, {29.0f, 2.0f}},
            {SLIDE_SPECIAL_THANKS_FADE_OUT, {32.0f, 2.0f}},
            {SLIDE_DR_FADE_IN, {35.0f, 2.0f}},
            {SLIDE_DR_FADE_OUT, {38.0f, 2.0f}},
            {FADE_OUT, {41.0f, 2.0f}},
            {END_T,    {44.0f, 2.0f}}
        }, true);

        // Array of file paths
        const char* textureFiles[] = {
            "./assets/textures/credits/OutroBackground_1.png",
            "./assets/textures/credits/OutroBackground_1.png",
            "./assets/textures/credits/OutroBackground_1.png",
            "./assets/textures/credits/OutroBackground_1.png",
            "./assets/textures/credits/OutroBackground_1.png",
            "./assets/textures/credits/OutroBackground_1.png",
            "./assets/textures/credits/OutroBackground_1.png",
            "./assets/textures/credits/OutroBackground_1.png" };

        // Load textures
        if (LoadTextures(textureIDs, textureFiles, 8) == false)
        {
            PrintLog("Failed to load all textures\n");
            return false;
        }

        // Create fade events
        fadeEvents = {
            {SLIDE_GROUP_FADE_IN, SLIDE_GROUP_FADE_OUT, textureIDs[0]},
            {SLIDE_TECHSTACK_FADE_IN, SLIDE_TECHSTACK_FADE_OUT, textureIDs[1]},
            {SLIDE_MUSIC_FADE_IN, SLIDE_MUSIC_FADE_OUT, textureIDs[2]},
            {SLIDE_GROUPLEAD_FADE_IN, SLIDE_GROUPLEAD_FADE_OUT, textureIDs[3]},
            {SLIDE_TEAM_FADE_IN, SLIDE_TEAM_FADE_OUT, textureIDs[4]},
            {SLIDE_SPECIAL_THANKS_FADE_IN, SLIDE_SPECIAL_THANKS_FADE_OUT, textureIDs[5]},
            {SLIDE_DR_FADE_IN, SLIDE_DR_FADE_OUT, textureIDs[6]},          
        };

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

        // Process fade events
        for (const auto& event : fadeEvents)
        {
            float alpha = 0.0f;

            // Check if fade-in event has started and fade-out event has not started
            if (sceneEvents->isEventStarted(event.fadeInID) && !sceneEvents->isEventStarted(event.fadeOutID))
            {
                alpha = lerp(sceneEvents->getEventTime(event.fadeInID), 0.0f, 1.0f);
            }
            else if (sceneEvents->isEventInProgress(event.fadeOutID))
            {
                alpha = lerp(sceneEvents->getEventTime(event.fadeOutID), 1.0f, 0.0f);
            }
            // else if(sceneEvents->isEventStarted(event.fadeInID) && !sceneEvents->isEventStarted(event.fadeOutID))
            // {
            // alpha = 0.0f;
            // }

            commonShaders->overlayTextureShader->drawQuadWithTexture(event.texture, modelMatrix, alpha);
        }

        // Fading
        pushMatrix(modelMatrix);
        {
            modelMatrix = vmath::scale(1.0f, 1.0f, 1.0f);
            if (sceneEvents->isEventInProgress(FADE_IN))
                commonShaders->overlayColorShader->draw(modelMatrix, 0.0f, 0.0f, 0.0f, lerp(sceneEvents->getEventTime(FADE_IN), 1.0f, 0.0f));
            else
                commonShaders->overlayColorShader->draw(modelMatrix, 0.0f, 0.0f, 0.0f, lerp(sceneEvents->getEventTime(FADE_OUT), 0.0f, 1.0f));
        }
        modelMatrix = popMatrix();

        /*pushMatrix(modelMatrix);
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
        modelMatrix = popMatrix();*/
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
