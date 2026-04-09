#pragma once
#include "../utils/camera/BezierCamera.h"
#include "../utils/EventManager.h"

class Scene
{
public:
    bool isInitialized = false;
    BezierCamera *sceneCamera;
    EventManager *sceneEvents;
    bool isSceneComplete = false;
    bool isSecondScene = false;
    Scene()
    {
        sceneEvents = nullptr;
    }

    virtual bool initialize() = 0;
    virtual void setupCamera() = 0;
    virtual void display() = 0;
    virtual void update() = 0;
    virtual void uninitialize() = 0;

};
