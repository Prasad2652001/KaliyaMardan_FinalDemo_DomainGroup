#pragma once
#include "../utils/model_using_assim/model.h"
#include "../utils/common.h"

class CommonModels
{
public:
    // INTRO
    Model *model_room;
    Model *model_tree;
    
 

    CommonModels()
    {
        // INTRO
        model_room = new Model();
        model_tree = new Model();
    
       
    }

    // Member Functions
    BOOL initialize(int selected_scene, bool isE2E)
    {
        // model_room->initializeModel(".\\assets\\models\\toyroom\\toyroom.obj");
        model_room->initializeModel(".\\assets\\models\\scene2\\Pine\\pineTrees.obj");
       
        return TRUE;
    }

    // ================================= Scene Specific Loading

    void uninitialize(void)
    {
        if (model_room)
        {
            model_room->uninitialize();
            delete model_room;
            model_room = NULL;
        }

        if (model_tree)
        {
            model_tree->uninitialize();
            delete model_tree;
            model_tree = NULL;
        }
    }
};
