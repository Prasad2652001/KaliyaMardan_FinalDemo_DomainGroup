#pragma once
#include "../utils/model_using_assim/model.h"
#include "../utils/common.h"

class CommonModels
{
public:
    // INTRO

    // Model *model_shloka1;
    // Model *model_shloka2;
    // Model *model_shloka3;
    // Model *model_shloka4;
    // Model *model_body;
    // Model *model_end;
    // Model *model_Title_Emerges;
    // Model *model_Title_From;

    Model *ModelShoes;

    
 

    CommonModels()
    {
        // INTRO
        // model_shloka1 = new Model();    // akashad vayoho 
        // model_shloka2 = new Model();   // avayor agnehe
        // model_shloka3 = new Model();   // agner aapah
        // model_shloka4 = new Model();   // adhya prithvi
        // model_end = new Model();   // adhya prithvi
        ModelShoes = new Model();

        // model_body = new Model();   // adhya prithvi

        // model_Title_From = new Model();
        // model_Title_Emerges = new Model();
    }

    // Member Functions
    BOOL initialize(int selected_scene, bool isE2E)
    {
        ModelShoes->initializeModel(".\\assets\\models\\boat\\boat.obj");
        // model_shloka1->initializeModel(".\\assets\\models\\toyroom\\toyroom.obj");
        // model_shloka1->initializeModel(".\\assets\\models\\sloka1\\untitled.obj");
        // model_shloka2->initializeModel(".\\assets\\models\\sloka2\\untitled.obj");
        // model_end->initializeModel(".\\assets\\models\\Sat\\untitled1.obj");
        // model_shloka3->initializeModel(".\\assets\\models\\sloka3\\untitled.obj");
        // model_shloka4->initializeModel(".\\assets\\models\\sloka4\\untitled.obj");
        
        // model_body->initializeModel(".\\assets\\models\\Body\\CompleteBody1.obj");
        
        // model_Title_From->initializeModel(".\\assets\\models\\intro\\intro.obj");
        // model_Title_Emerges->initializeModel(".\\assets\\models\\intro\\intro1.obj");
        
        return TRUE;
    }

    // ================================= Scene Specific Loading

    void uninitialize(void)
    {
        if (ModelShoes)
        {
            ModelShoes->uninitialize();
            delete ModelShoes;
            ModelShoes = NULL;
        }
        // if (model_shloka1)
        // {
        //     model_shloka1->uninitialize();
        //     delete model_shloka1;
        //     model_shloka1 = NULL;
        // }

        // if (model_shloka2)
        // {
        //     model_shloka2->uninitialize();
        //     delete model_shloka2;
        //     model_shloka2 = NULL;
        // }

        
        // if (model_end)
        // {
        //     model_end->uninitialize();
        //     delete model_end;
        //     model_end = NULL;
        // }

        // if (model_shloka3)
        // {
        //     model_shloka3->uninitialize();
        //     delete model_shloka3;
        //     model_shloka3 = NULL;
        // }

        // if (model_shloka4)
        // {
        //     model_shloka4->uninitialize();
        //     delete model_shloka4;
        //     model_shloka4 = NULL;
        // }
        
        // if (model_body)
        // {
        //     model_body->uninitialize();
        //     delete model_body;
        //     model_body = NULL;
        // }

        // if (model_Title_Emerges)
        // {
        //     model_Title_Emerges->uninitialize();
        //     delete model_Title_Emerges;
        //     model_Title_Emerges = NULL;
        // }

        // if (model_Title_From)
        // {
        //     model_Title_From->uninitialize();
        //     delete model_Title_From;
        //     model_Title_From = NULL;
        // }
    }
};
