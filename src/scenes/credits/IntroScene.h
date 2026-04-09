#pragma once

#include "../../utils/common.h"
#include "../../includes/helper_timer.h"
#include "../../scenes/Credits/Credits.h"

class IntroScene : public Scene
{
private:
    bool isInitialized = false;

    enum sceneEventIds
    {
        START_T,
        FADE_IN,
        FADE_OUT,
        END_T,
    };

    GLuint texture_grpname;

public:
    IntroScene(void)
    {
        sceneCamera = new BezierCamera();
    }

    bool initialize(void)
    {
        // initialize default camera
        sceneCamera->initialize();
        sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal);
        sceneCamera->update();

        // Event System
        sceneEvents = new EventManager(
            {{START_T, {0.0f, 10.0f}},
             {FADE_IN, {0.0f, 3.0f}},
             {FADE_OUT, {7.0f, 3.0f}},
             {END_T, {10.0f, 0.0f}}},
            true);

        if (LoadPNGImage(&texture_grpname, "./assets/textures/credits/00-grpname.png") == FALSE)
        {
            PrintLog("Failed to load texture_grpname texture\n");
            return false;
        }

        isInitialized = true;
        return TRUE;
    }

    void setupCamera()
    {
    }

    void display(void)
    {
        // Outro Texture
        pushMatrix(modelMatrix);
        {
            commonShaders->overlayTextureShader->drawQuadWithTexture(texture_grpname, modelMatrix, 1.0f);
        }
        modelMatrix = popMatrix();

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
    }

    void update(void)
    {
        sceneEvents->increment();

        /// Completing scene hereq
        if (sceneEvents->isEventComplete(END_T))
        {
            isSceneComplete = true;
        }
    }

    void uninitialize()
    {
        if (isInitialized)
        {
        }
    }
};
