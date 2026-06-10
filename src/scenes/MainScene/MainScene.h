#pragma once

#include "../../utils/common.h"
#include "../../utils/camera/Camera.h"
#include "../../utils/camera/BezierCamera.h"
#include "../../effects/tint/Tint.h"
#include "../Scene.h"
// #include "../Intro/Intro.h"
#include "../Scene0/Scene0.h"
#include "../Scene1/Scene1.h"
#include "../Scene2/Scene2.h"
#include "../Scene3/Scene3.h"
#include "../Scene4/Scene4.h"
#include "../Outro/Outro.h"

extern Camera camera;
void setGlobalBezierCamera(BezierCamera *bezierCamera);

class MainScene
{
public:
    int selected_scene;
    bool START_E2E_DEMO;
    Scene *scene;

    // IntroScene *introScene;
    DemoScene0 *scene0;
    DemoScene1 *scene1;
    DemoScene2 *scene2;
    DemoScene3 *scene3;
    DemoScene4 *scene4;
    OutroScene *outroScene;
    TintEffect *tintEffect;

    MainScene()
    {
        // // introScene = new IntroScene();
        scene0 = new DemoScene0();
        scene1 = new DemoScene1();
        scene2 = new DemoScene2();
        scene3 = new DemoScene3();
        scene4 = new DemoScene4();
        outroScene = new OutroScene();
        tintEffect = new TintEffect();

        START_E2E_DEMO = false;
        // selected_scene = SCENE_INTRO;
        selected_scene = SCENE_00;
        scene = nullptr;
    }

    // Initialize only scenes that will actually be used (avoids loading
    // duplicate terrains/models/water FBOs for every scene at once).
    void initializeScenesUpTo(int maxScene)
    {
        if (maxScene >= SCENE_00 && !scene0->isInitialized) scene0->initialize();
        if (maxScene >= SCENE_01 && !scene1->isInitialized) scene1->initialize();
        if (maxScene >= SCENE_02 && !scene2->isInitialized) scene2->initialize();
        // if (maxScene >= SCENE_03 && !scene3->isInitialized) scene3->initialize();
        // if (maxScene >= SCENE_04 && !scene4->isInitialized) scene4->initialize();
        // if (maxScene >= SCENE_OUTRO && !outroScene->isInitialized) outroScene->initialize();
    }

    void initializeActiveSceneOnly()
    {
        switch (selected_scene)
        {
        case SCENE_00: if (!scene0->isInitialized) scene0->initialize(); break;
        case SCENE_01: if (!scene1->isInitialized) scene1->initialize(); break;
        case SCENE_02: if (!scene2->isInitialized) scene2->initialize(); break;
        case SCENE_03: if (!scene3->isInitialized) scene3->initialize(); break;
        case SCENE_04: if (!scene4->isInitialized) scene4->initialize(); break;
        case SCENE_OUTRO: if (!outroScene->isInitialized) outroScene->initialize(); break;
        default: break;
        }
    }

    BOOL initialize()
    {
        if (START_E2E_DEMO)
        {
            // E2E: only load the starting scene now; later scenes lazy-init on transition.
            initializeScenesUpTo(selected_scene);
        }
        else
        {
            initializeActiveSceneOnly();
        }

        if (!tintEffect)
        {
            PrintLog("Failed to cretae tintEffect object \n");
            return FALSE;
        }
        tintEffect->initialize();

        switch (selected_scene)
        {
        case SCENE_INTRO:
            // scene = introScene;
            break;
        case SCENE_00:
            scene = scene0;
            break;
        case SCENE_01:
            scene = scene1;
            break;
        case SCENE_02:
            scene = scene2;
            break;
        case SCENE_03:
            scene = scene3;
            break;
        case SCENE_04:
            scene = scene4;
            break;
        case SCENE_OUTRO:
            scene = outroScene;
            break;
        default:
            // scene = introScene;
            break;
        }

        return TRUE;
    }

    void display()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        if (!scene)
            return;

        modelMatrix = mat4::identity();
        viewMatrix = mat4::identity();

        if (selected_scene != SCENE_INTRO)
        {
            setGlobalBezierCamera(scene->sceneCamera);

            if (scene->sceneCamera && scene->sceneCamera->bezierPoints.size() > 0)
            {
                if (!USE_FPV_CAM)
                    scene->sceneCamera->update();

                updateGlobalViewMatrix();
            }
        }

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        setGlobalBezierCamera(scene->sceneCamera);

        if (scene->sceneCamera && scene->sceneCamera->bezierPoints.size() > 0)
        {
            if (!USE_FPV_CAM)
                scene->sceneCamera->update();

            updateGlobalViewMatrix(); // uncomment this to run simultaniuously scnee
        }

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        
        // if (gEnableTint)
        // {
            
        //     tintEffect->bindTintFBO();
        //     scene->display();
        //     tintEffect->unbindTintFBO();
        //     tintEffect->display();
        // }
        // else
        // {
            //     scene->display();
        // }
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if (selected_scene == SCENE_INTRO)
        {
            scene->display();
        }
        else if (gEnableTint)
        {
            tintEffect->bindTintFBO();
            scene->display();
            tintEffect->unbindTintFBO();
            // tintEffect->display(gTintMode, 1.8f, 0.85f); main line without i am gui
            tintEffect->display(gTintMode, gTintVignettePower, gTintStrength);
            
            /*
                gTintMode = 0; // No tint
                gTintMode = 1; // Brown yellow vintage
                gTintMode = 2; // Grey black-white
                gTintMode = 3; // Pastel happy
                gTintMode = 4; // Warm film

                proper 4 corners wala -> 
                tintEffect->display(gTintMode, 2.4f, 0.9f);

                even bit light ->
                gTintMode = 1;
                tintEffect->display(gTintMode, 2.2f, 0.88f);    
            */
        }
        else
        {
            scene->display();
        }
    }

    void update()
    {   
        
        scene->update();

        // if (START_E2E_DEMO && scene->isSceneComplete)
        // {
        //     selected_scene++;

        //     // Lazy-init the next scene on first transition (keeps RAM low at startup).
        //     initializeScenesUpTo(selected_scene);

        //     switch (selected_scene)
        //     {
        //     case SCENE_00:
        //         scene = scene0;
        //         break;
        //     case SCENE_01:
        //         scene = scene1;
        //         break;
        //     case SCENE_02:
        //         scene = scene2;
        //         break;
        //     case SCENE_03:
        //         scene = scene3;
        //         break;
        //     case SCENE_04:
        //         scene = scene4;
        //         break;
        //     case SCENE_OUTRO:
        //         scene = outroScene;
        //         break;
        //     default:
        //         scene = outroScene;
        //         break;
        //     }
        // }
    }

    void uninitialize()
    {
        // if (introScene)
        {
            // introScene->uninitialize();
            // delete introScene;
            // introScene = nullptr;
        }

        if (tintEffect)
        {
            tintEffect->uninitialize();
            delete tintEffect;
            tintEffect = NULL;
        }

        if (scene0)
        {
            scene0->uninitialize();
            delete scene0;
            scene0 = nullptr;
        }

        if (scene1)
        {
            scene1->uninitialize();
            delete scene1;
            scene1 = nullptr;
        }

        if (scene2)
        {
            scene2->uninitialize();
            delete scene2;
            scene2 = nullptr;
        }

        if (scene3)
        {
            scene3->uninitialize();
            delete scene3;
            scene3 = nullptr;
        }

        if (scene4)
        {
            scene4->uninitialize();
            delete scene4;
            scene4 = nullptr;
        }

        if (outroScene)
        {
            outroScene->uninitialize();
            delete outroScene;
            outroScene = nullptr;
        }

        scene = nullptr;
    }
};
