#pragma once
#include "../../utils/common.h"
#include "../../utils/camera/Camera.h"
#include "../../utils/camera/BezierCamera.h"

// SCENES
// -------------- new update for kaliya mardan --------------
#include "../scene1/scene1.h"
/* #include "../scene2/scene2.h"
#include "../scene3/scene3.h"
#include "../scene4/scene4.h"
#include "../scene5/scene5.h"
#include "../scene6/scene6.h"
#include "../scene7/scene7.h"
#include "../scene8/scene8.h"
#include "../scene9/scene9.h"
#include "../scene10/scene10.h" */

BOOL bDoneAllInitialization = FALSE;
extern Camera camera;
void setGlobalBezierCamera(BezierCamera *bezierCamera);

class MainScene
{

public:
    // member variables
    int selected_scene;
    bool START_E2E_DEMO;

    // GLOBAL SCENE
    Scene *scene;

    // --------- new update particular scene class object declaration -------
    Scene1 *scene1;
    /* Scene2 *scene2;
    Scene3 *scene3;
    Scene4 *scene4;
    Scene5 *scene5;
    Scene6 *scene6;
    Scene7 *scene7;
    Scene8 *scene8;
    Scene9 *scene9;
    Scene10 *scene10; */

    // member functions
    MainScene()
    {
        // SCENE
        scene1 = new Scene1();
        /* scene2 = new Scene2();
        scene3 = new Scene3();
        scene4 = new Scene4();
        scene5 = new Scene5();
        scene6 = new Scene6();
        scene7 = new Scene7();
        scene8 = new Scene8();
        scene9 = new Scene9();
        scene10 = new Scene10(); */

        START_E2E_DEMO = FALSE;
        selected_scene = SCENE_1;
    }

    BOOL initialize()
    {
        // SCENES

        if (START_E2E_DEMO)
        {
            // INITIALIZE ALL SCENES
            scene1->initialize();
            /*scene2->initialize();
            scene3->initialize();
            scene4->initialize();
            scene5->initialize();
            scene6->initialize();
            scene7->initialize();
            scene8->initialize();
            scene9->initialize();
            scene10->initialize(); */

            switch (selected_scene)
            {
            case SCENE_1:
                scene = scene1;
                break;
          /*   case SCENE_2:
                scene = scene2;
                break;
            case SCENE_3:
                scene = scene3;
                break;
            case SCENE_4:
                scene = scene4;
                break;
            case SCENE_5:
                scene = scene5;
                break;
            case SCENE_6:
                scene = scene6;
                break;
            case SCENE_7:
                scene = scene7;
                break;
            case SCENE_8:
                scene = scene8;
                break;
            case SCENE_9:
                scene = scene9;
                break;
            case SCENE_10:
                scene = scene10;
                break; */
            default:
                break;
            }
        }
        else
        {
            // SET THE GLOBAL SCENE BASED ON SELECTED SCENE
            switch (selected_scene)
            {
            case SCENE_1:
                scene = scene1;
                break;
          /*   case SCENE_2:
                scene = scene2;
                break;
            case SCENE_3:
                scene = scene3;
                break;
            case SCENE_4:
                scene = scene4;
                break;
            case SCENE_5:
                scene = scene5;
                break;
            case SCENE_6:
                scene = scene6;
                break;
            case SCENE_7:
                scene = scene7;
                break;
            case SCENE_8:
                scene = scene8;
                break;
            case SCENE_9:
                scene = scene9;
                break;
            case SCENE_10:
                scene = scene10;
                break; */
            default:
                break;
            }
            
            if (selected_scene = SCENE_6)
            {
                scene->isSecondScene = true;
            }
            
            // INITIALIZE GLOBAL SCENE
            scene->initialize();
        }

        return TRUE;
    }

    void display()
    {
        // DISPLAY THE SELECTED SCENE
        modelMatrix = mat4::identity();
        viewMatrix = mat4::identity();
        setGlobalBezierCamera(scene->sceneCamera);
        if (scene->sceneCamera->bezierPoints.size() > 0)
        {
            if (!USE_FPV_CAM)
            {
                scene->sceneCamera->update();
            }
            updateGlobalViewMatrix();
        }
        scene->display();
    }

    void update()
    {
        // RUNNING ALL SCENES SCENE
        if (START_E2E_DEMO) // Switch scenes only if end to end demo is played
        {   
            
            if (selected_scene == SCENE_6 && scene->isSceneComplete && scene->isSecondScene == false)
            {
                scene->isSceneComplete = false;
                scene->isSecondScene = true;
            }

            // UPDATE CURRENT RUNNING SCENE
            if (scene->isSceneComplete)
            {
                selected_scene++;

                switch (selected_scene)
                {
                case SCENE_1:
                    scene = scene1;
                    break;
               /*  case SCENE_2:
                    scene = scene2;
                    break;
                case SCENE_3:
                    scene = scene3;
                    break;
                case SCENE_4:
                    scene = scene5;
                    break;
                case SCENE_5:
                    scene = scene5;
                    break;
                case SCENE_6:
                    scene = scene6;
                    break;
                case SCENE_7:
                    scene = scene7;
                    break;
                case SCENE_8:
                    scene = scene8;
                    break;
                case SCENE_9:
                    scene = scene9;
                    break;
                case SCENE_10:
                    scene = scene10;
                    break; */
                default:
                    break;
                }
            }
        }

        // UPDATE CURRENT RUNNING SCENE
        scene->update();
    }

    void uninitialize()
    {
        // Uninitialize only if START_E2E_DEMO is false
        if (!START_E2E_DEMO)
        {
            if (scene->isInitialized)
                scene->uninitialize();
        }
        else
        {
            // If START_E2E_DEMO is true, uninitialize everything as usual
            if (scene1->isInitialized)
                scene1->uninitialize();

           /*  if (scene2->isInitialized)
                scene2->uninitialize();

            if (scene3->isInitialized)
                scene3->uninitialize(); */
        }   
    }
};
