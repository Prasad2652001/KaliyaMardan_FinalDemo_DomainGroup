#pragma once

#include "../../utils/common.h"
#include "../../includes/helper_timer.h"
#include "../../scenes/Credits/Credits.h"

class OutroScene2 : public Scene
{
private:
    bool isInitialized = false;

    // Struct to represent a fade event
    struct FadeEvent
    {
        int fadeInID;
        int fadeOutID;
        GLuint texture;
    };
    // Define Event IDs
    enum EventID
    {
        START_T = 0,
        FADE_IN,
        GL_FADE_IN,
        GL_FADE_OUT,
        SONG_FADE_IN,
        SONG_FADE_OUT,
        TECHSTACK_FADE_IN,
        TECHSTACK_FADE_OUT,
        REFERENCE_FADE_IN,
        REFERENCE_FADE_OUT,
        EFFECTS_FADE_IN,
        EFFECTS_FADE_OUT,
        SPECIALTHANKS_FADE_IN,
        SPECIALTHANKS_FADE_OUT,
        GUIDE_FADE_IN,
        GUIDE_FADE_OUT,
        IGNITEDBY_FADE_IN,
        IGNITEDBY_FADE_OUT,
        FADE_OUT,
        END_T,
    };

    GLuint textureIDs[16];
    std::vector<FadeEvent> fadeEvents;

public:
    OutroScene2(void)
    {
        sceneCamera = new BezierCamera();
    }

    // Function to load textures
    bool LoadTextures(GLuint *textures, const char *filenames[], size_t count)
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

    bool initialize(void)
    {
        // initialize default camera
        sceneCamera->initialize();
        sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal);
        sceneCamera->update();

        // Event System
        sceneEvents = new EventManager(
            {
                {START_T, {0.0f, 3.0f}},
                {FADE_IN, {0.0f, 3.0f}},
                {GL_FADE_IN, {0.0f, 2.0f}},
                {GL_FADE_OUT, {5.0f, 2.0f}},
                {SONG_FADE_IN, {7.0f, 2.0f}},
                {SONG_FADE_OUT, {9.0f, 2.0f}},
                {TECHSTACK_FADE_IN, {11.0f, 2.0f}},
                {TECHSTACK_FADE_OUT, {14.0f, 2.0f}},
                {REFERENCE_FADE_IN, {17.0f, 2.0f}},
                {REFERENCE_FADE_OUT, {20.0f, 2.0f}},
                {EFFECTS_FADE_IN, {23.0f, 2.0f}},
                {EFFECTS_FADE_OUT, {26.0f, 2.0f}},
                {SPECIALTHANKS_FADE_IN, {29.0f, 2.0f}},
                {SPECIALTHANKS_FADE_OUT, {32.0f, 2.0f}},
                {GUIDE_FADE_IN, {35.0f, 2.0f}},
                {GUIDE_FADE_OUT, {38.0f, 2.0f}},
                {IGNITEDBY_FADE_IN, {41.0f, 2.0f}},
                {IGNITEDBY_FADE_OUT, {44.0f, 2.0f}},
                {FADE_OUT, {47.0f, 2.0f}},
                {END_T, {50.0f, 2.0f}},
            },
            false);

        // Array of file paths
        const char *textureFiles[] = {
            "./assets/textures/credits/09-gl.png",
            "./assets/textures/credits/10-music.png",
            "./assets/textures/credits/11-ref.png",
            "./assets/textures/credits/12-effects.png",
            "./assets/textures/credits/13-tech.png",
            "./assets/textures/credits/14-thanks.png",
            "./assets/textures/credits/15-guide.png",
            "./assets/textures/credits/16-sir.png"};

        // Load textures
        if (LoadTextures(textureIDs, textureFiles, 8) == false)
        {
            PrintLog("Failed to load all textures\n");
            return false;
        }

        // Create fade events
        fadeEvents = {
            {GL_FADE_IN, GL_FADE_OUT, textureIDs[0]},
            {SONG_FADE_IN, SONG_FADE_OUT, textureIDs[1]},
            {REFERENCE_FADE_IN, REFERENCE_FADE_OUT, textureIDs[2]},
            {EFFECTS_FADE_IN, EFFECTS_FADE_OUT, textureIDs[3]},
            {TECHSTACK_FADE_IN, TECHSTACK_FADE_OUT, textureIDs[4]},
            {SPECIALTHANKS_FADE_IN, SPECIALTHANKS_FADE_OUT, textureIDs[5]},
            {GUIDE_FADE_IN, GUIDE_FADE_OUT, textureIDs[6]},
            {IGNITEDBY_FADE_IN, IGNITEDBY_FADE_OUT, textureIDs[7]}};

        isInitialized = true;
        return true;
    }

    void setupCamera()
    {
    }

    void display(void)
    {
        // Process fade events
        for (const auto &event : fadeEvents)
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
