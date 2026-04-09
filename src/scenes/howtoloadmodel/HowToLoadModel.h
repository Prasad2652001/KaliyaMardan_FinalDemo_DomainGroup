#pragma once
#include "../../../utils/common.h"
#include "../../../utils/camera/Camera.h"
#include "../../../utils/camera/BezierCamera.h"

#include "../../../shaders/modelgltf/glshaderloader.h"
#include "../../../shaders/modelgltf/glLight.h"
#include "../../../shaders/modelgltf/glmodelloader.h"

#define _DEBUG

extern Camera camera;
extern BezierCamera *globalBezierCamera;

class HowToLoadModel
{

public:
    bool isInitialized = false;
    BezierCamera sceneCamera;

    glshaderprogram *programStaticPBR;
    glmodel *churchModel;
    glmodel *vehicleModel;
    glmodel *roadModel;
    SceneLight *lightManager;

    // member functions
    HowToLoadModel()
    {
    }

    BOOL initialize()
    {
        // Initializing GLB Model
        programStaticPBR = new glshaderprogram({"./src/shaders/modelgltf/pbrStatic.vert", "./src/shaders/modelgltf/pbrMain.frag"});
        churchModel = new glmodel("./assets/models/scene2/church.glb", aiProcessPreset_TargetRealtime_Quality, true);
        roadModel = new glmodel("./assets/models/scene2/road.glb", aiProcessPreset_TargetRealtime_Quality, true);
        vehicleModel = new glmodel("./assets/models/scene2/vehicle.glb", aiProcessPreset_TargetRealtime_Quality, true);
        
   
        lightManager = new SceneLight();
        lightManager->addDirectionalLights({DirectionalLight(vec3(0.55f), 10.0f, vec3(0.0, -1.0, -1.0f)),
                                            DirectionalLight(vec3(0.55f), 10.0f, vec3(0.0, -1.0, 1.0f)),
                                            DirectionalLight(vec3(0.3f), 10.0f, vec3(0.0, -1.0, 0.0f))});
        lightManager->addSpotLights({SpotLight(vec3(1.0f), 10.0f, vec3(0.0f, 10.0f, 0.0f), 100.0f, vec3(0, -0.9, -0.3), 20.0f, 22.0f)});
        lightManager->setAmbient(vec3(0.05f));

        sceneCamera.initialize();
        isInitialized = true;
        return TRUE;
    }

    void display()
{
    // Camera
    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();

    updateGlobalViewMatrix();

    // Update light directions
    lightManager->setDirectionalLightDirection(2, vec3(scaleX, scaleY, scaleZ));

    // Use the shader program
    programStaticPBR->use();

    // Common uniforms
    glUniformMatrix4fv(programStaticPBR->getUniformLocation("pMat"), 1, GL_FALSE, perspectiveProjectionMatrix);
    glUniformMatrix4fv(programStaticPBR->getUniformLocation("vMat"), 1, GL_FALSE, viewMatrix);
    glUniform3fv(programStaticPBR->getUniformLocation("viewPos"), 1, (USE_FPV_CAM ? camera.getEye() : globalBezierCamera->getEye()));
    glUniform1i(programStaticPBR->getUniformLocation("specularGloss"), GL_FALSE);
    glUniform1f(programStaticPBR->getUniformLocation("clipy"), 0);

    // Set lights
    lightManager->setLightUniform(programStaticPBR, false);

    // Model 1: Church Model
    pushMatrix(modelMatrix);
    {
        glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE,
                           vmath::translate(1.0f + 1410.0f, 1.0f + 37.0f + -10.0f , 1.0f + 85.0f) * 
                           vmath::rotate(-43.0f, 1.0f, 0.0f, 0.0f) *
                           vmath::rotate(-92.0f , 0.0f, 1.0f, 0.0f) * 
                           vmath::rotate(137.0f , 0.0f, 0.0f, 1.0f) * 
                           
                           vmath::scale(3.0f + 5.0f, 3.0f + 2.0f, 3.0f + 4.0f));
        churchModel->draw(programStaticPBR);
    }
    modelMatrix = popMatrix();

        // Model 2: 1st Road Model    
        pushMatrix(modelMatrix);
        {
            mat4 modelMatrix =  vmath::translate(-104.0f + 1250.0f + 17.0f + 224.0f,   1.0f + 7.0f + -2.0f + 3.0f  + 0.20f ,  -31.0f + -1.0f + -150.0f + -75.0f ) *
                                vmath::rotate( 89.0f + 1.0f + 1.1f , 1.0f, 0.0f, 0.0f) *
                                vmath::rotate( 179.0f  + -2.0f + 2.0f + 1.6f , 0.0f, 1.0f, 0.0f) * 
                                vmath::rotate( 0.0f + 115.0f   , 0.0f, 0.0f, 1.0f) * 

                                vmath::scale(140.0f + 1.0f , 10.0f + 28.0f + 40.0f   , 50.0f  );
            
            glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
            roadModel->draw(programStaticPBR);
        }
        modelMatrix = popMatrix();


        // Model 2: 2st Road Model
        pushMatrix(modelMatrix);
        {
            modelMatrix =  vmath::translate(-104.0f + 1250.0f + 17.0f + 224.0f + -48.49f ,   1.0f + 7.0f + -2.0f + 3.0f  + 0.20f + -0.9f ,  -31.0f + -1.0f + -150.0f + -75.0f + 22.60f) *
                                vmath::rotate( 89.0f + 1.0f + 1.1f , 1.0f, 0.0f, 0.0f) *
                                vmath::rotate( 179.0f  + -2.0f + 2.0f + 1.6f , 0.0f, 1.0f, 0.0f) * 
                                vmath::rotate( 0.0f + 115.0f   , 0.0f, 0.0f, 1.0f) * 

                                vmath::scale(140.0f + 1.0f , 10.0f + 28.0f + 40.0f   , 50.0f  );
            
            glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
            roadModel->draw(programStaticPBR);
        }
        modelMatrix = popMatrix();
        

        // Model 2: 3rd Road Model
        pushMatrix(modelMatrix);
        {
            modelMatrix =  vmath::translate(1338.51f + -50.29f ,   8.29999f + -1.0f ,  -234.4f + 23.50f) *
                                vmath::rotate( 89.0f + 1.0f + 1.1f , 1.0f, 0.0f, 0.0f) *
                                vmath::rotate( 179.0f  + -2.0f + 2.0f + 1.6f , 0.0f, 1.0f, 0.0f) * 
                                vmath::rotate( 0.0f + 115.0f   , 0.0f, 0.0f, 1.0f) * 

                                vmath::scale(140.0f + 1.0f , 10.0f + 28.0f + 40.0f   , 50.0f  );
            
            glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
            roadModel->draw(programStaticPBR);
        }
        modelMatrix = popMatrix();

        // Model 2: 4th Road Model
        pushMatrix(modelMatrix);
        {
            modelMatrix =  vmath::translate(1338.51f + -50.29f + -50.29f ,   8.29999f + -1.0f + -1.0f ,  -234.4f + 23.50f + 23.50f) *
                                vmath::rotate( 89.0f + 1.0f + 1.1f + -0.1f , 1.0f, 0.0f, 0.0f) *
                                vmath::rotate( 179.0f  + -2.0f + 2.0f + 1.6f + -0.3f , 0.0f, 1.0f, 0.0f) * 
                                vmath::rotate( 0.0f + 115.0f    , 0.0f, 0.0f, 1.0f) * 

                                vmath::scale(140.0f + 1.0f , 10.0f + 28.0f + 40.0f   , 50.0f  );
            
            glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
            roadModel->draw(programStaticPBR);
        }
        modelMatrix = popMatrix();

        // Model 2: 5th Road Model
        pushMatrix(modelMatrix);
        {
            modelMatrix =  vmath::translate(1338.51f + -50.29f + -50.29f + -49.59f,   8.29999f + -1.0f + -1.0f + -1.0f + 0.4f,  -234.4f + 23.50f + 23.50f + 23.30f) *
                                vmath::rotate( 89.0f + 1.0f + 1.1f + -0.6f , 1.0f, 0.0f, 0.0f) *
                                vmath::rotate( 179.0f  + -2.0f + 2.0f + 1.6f + -0.5f   , 0.0f, 1.0f, 0.0f) * 
                                vmath::rotate( 0.0f + 115.0f  + -0.1f  , 0.0f, 0.0f, 1.0f) *    

                                vmath::scale(140.0f + 1.0f , 10.0f + 28.0f + 40.0f   , 50.0f  );
            
            glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
            roadModel->draw(programStaticPBR);
        }
        modelMatrix = popMatrix();


        // Model 2: 6th Road Model
        pushMatrix(modelMatrix);
        {
            modelMatrix =  vmath::translate(1338.51f + -50.29f + -50.29f + -49.59f + -50.19f ,   8.29999f + -1.0f + -1.0f + -1.0f + 0.4f + -0.2f,  -234.4f + 23.50f + 23.50f + 23.30f + 23.40f) *
                                vmath::rotate( 89.0f + 1.0f + 1.1f + -0.6f + -0.5f , 1.0f, 0.0f, 0.0f) *
                                vmath::rotate( 179.0f  + -2.0f + 2.0f + 1.6f + -0.5f + -0.4f , 0.0f, 1.0f, 0.0f) * 
                                vmath::rotate( 0.0f + 115.0f  + -0.1f  , 0.0f, 0.0f, 1.0f) *    

                                vmath::scale(140.0f + 1.0f , 10.0f + 28.0f + 40.0f   , 50.0f  );
            
            glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
            roadModel->draw(programStaticPBR);
        }
        modelMatrix = popMatrix();


        // Model 2: 7th Road Model
        pushMatrix(modelMatrix);
        {
            modelMatrix =  vmath::translate(1338.51f + -50.29f + -50.29f + -49.59f + -50.19f + -50.0f ,   8.29999f + -1.0f + -1.0f + -1.0f + 0.4f + -0.2f ,  -234.4f + 23.50f + 23.50f + 23.30f + 23.40f + 23.0f) *
                                vmath::rotate( 89.0f + 1.0f + 1.1f + -0.6f + -0.8f + -0.3f  , 1.0f, 0.0f, 0.0f) *
                                vmath::rotate( 179.0f  + -2.0f + 2.0f + 1.6f + -0.5f +  -0.7f + 0.7f , 0.0f, 1.0f, 0.0f) * 
                                vmath::rotate( 0.0f + 115.0f  + -0.1f + -0.2f  , 0.0f, 0.0f, 1.0f) *    

                                vmath::scale(140.0f + 1.0f + 1.1f , 10.0f + 28.0f + 40.0f  , 50.0f   );
            
            glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
            roadModel->draw(programStaticPBR);
        }   
        modelMatrix = popMatrix();
  
        // Model 2: 8th Road Model
        pushMatrix(modelMatrix);
        {   
           modelMatrix = vmath::translate(1038.15f  , 5.49999f  , -141.7f + 47.0f ) *
                        vmath::rotate(89.4f + 0.1f , 1.0f, 0.0f, 0.0f) *
                        vmath::rotate(179.1f + 1.0f  , 0.0f, 1.0f, 0.0f) *
                        vmath::rotate(114.7f  , 0.0f, 0.0f, 1.0f) *
                        vmath::scale(142.1f, 78.0f, 50.0f);

                                
            glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
            roadModel->draw(programStaticPBR);
        }
        modelMatrix = popMatrix();


        // Model 2: 9th Road Model
        pushMatrix(modelMatrix);
        {   
           modelMatrix = vmath::translate(1038.15f  + -45.0f   , 5.49999f    , -141.7f + 47.0f + 21.0f  ) *
                        vmath::rotate(89.4f + 0.1f + 0.4f  , 1.0f, 0.0f, 0.0f) *
                        vmath::rotate(179.1f + 1.0f + -0.2f  , 0.0f, 1.0f, 0.0f) *
                        vmath::rotate(114.7f   , 0.0f, 0.0f, 1.0f) *
                        vmath::scale(142.1f, 78.0f, 50.0f);

                                
            glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
            roadModel->draw(programStaticPBR);
        }
        modelMatrix = popMatrix();

         // Model 2: 10th Road Model
        pushMatrix(modelMatrix);
        {   
           modelMatrix = vmath::translate(1038.15f  + -45.0f  + -50.0f , 5.49999f    , -141.7f + 47.0f + 21.0f + 23.0f ) *
                        vmath::rotate(89.4f + 0.1f + 0.4f  , 1.0f, 0.0f, 0.0f) *
                        vmath::rotate(179.1f + 1.0f + -0.2f  , 0.0f, 1.0f, 0.0f) *
                        vmath::rotate(114.7f   , 0.0f, 0.0f, 1.0f) *
                        vmath::scale(142.1f, 78.0f, 50.0f);

                                
            glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
            roadModel->draw(programStaticPBR);
        }
        modelMatrix = popMatrix();

         // Model 2: 10th Road Model
        pushMatrix(modelMatrix);
        {   
           modelMatrix = vmath::translate(1038.15f  + -45.0f  + -50.0f + -50.0f , 5.49999f    , -141.7f + 47.0f + 21.0f + 23.0f + 23.0f ) *
                        vmath::rotate(89.4f + 0.1f + 0.4f  , 1.0f, 0.0f, 0.0f) *
                        vmath::rotate(179.1f + 1.0f + -0.2f  , 0.0f, 1.0f, 0.0f) *
                        vmath::rotate(114.7f   , 0.0f, 0.0f, 1.0f) *
                        vmath::scale(142.1f, 78.0f, 50.0f);

                                
            glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
            roadModel->draw(programStaticPBR);
        }
        modelMatrix = popMatrix();
  
         // Model 2: 11th Road Model
        pushMatrix(modelMatrix);
        {   
           modelMatrix = vmath::translate(1038.15f  + -45.0f  + -50.0f + -50.0f + -50.0f  , 5.49999f    , -141.7f + 47.0f + 21.0f + 23.0f + 23.0f + 23.0f ) *
                        vmath::rotate(89.4f + 0.1f + 0.4f  , 1.0f, 0.0f, 0.0f) *
                        vmath::rotate(179.1f + 1.0f + -0.2f  , 0.0f, 1.0f, 0.0f) *
                        vmath::rotate(114.7f   , 0.0f, 0.0f, 1.0f) *
                        vmath::scale(142.1f, 78.0f, 50.0f);

                                
            glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
            roadModel->draw(programStaticPBR);
        }
        modelMatrix = popMatrix();

        // // Model 2 : 12th Road Model
        // pushMatrix(modelMatrix);
        // {   
        //     modelMatrix = vmath::translate(843.15f + -50.0f , 5.49999f, -4.7f + 23.0f) *
        //         vmath::rotate(89.9f, 1.0f, 0.0f, 0.0f) *
        //         vmath::rotate(179.9f, 0.0f, 1.0f, 0.0f) *
        //         vmath::rotate(114.7f, 0.0f, 0.0f, 1.0f) *
        //         vmath::scale(142.1f, 78.0f, 50.0f);

                                
        //     glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
        //     roadModel->draw(programStaticPBR);
        // }
        // modelMatrix = popMatrix();


        //  // Model 2 : 13th Road Model
        // pushMatrix(modelMatrix);
        // {   
        //     modelMatrix = vmath::translate(843.15f + -50.0f + -50.0f  , 5.49999f, -4.7f + 23.0f  + 23.0f) *
        //         vmath::rotate(89.9f, 1.0f, 0.0f, 0.0f) *
        //         vmath::rotate(179.9f, 0.0f, 1.0f, 0.0f) *
        //         vmath::rotate(114.7f, 0.0f, 0.0f, 1.0f) *
        //         vmath::scale(142.1f, 78.0f, 50.0f);

                                
        //     glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
        //     roadModel->draw(programStaticPBR);
        // }
        // modelMatrix = popMatrix();

        
        const int numModels = 30;  
        float translateX = 843.15f;  
        float translateZ = -4.7f;  

        for (int i = 0; i < numModels; ++i)
        {
            pushMatrix(modelMatrix);
            {   
                
                modelMatrix = vmath::translate(translateX + (-50.0f * i), 5.49999f, translateZ + (23.0f * i)) *
                            vmath::rotate(89.9f, 1.0f, 0.0f, 0.0f) *
                            vmath::rotate(179.9f, 0.0f, 1.0f, 0.0f) *
                            vmath::rotate(114.7f, 0.0f, 0.0f, 1.0f) *
                            vmath::scale(142.1f, 78.0f, 50.0f);

                glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
                roadModel->draw(programStaticPBR);
            }
            modelMatrix = popMatrix();
        }


        // Model 3 : Vehicle Model
        // pushMatrix(modelMatrix);
        // {   
        //     mat4 modelMatrix = vmath::translate(objX , objY , objZ ) *
        //         vmath::rotate(scaleX , 1.0f, 0.0f, 0.0f) *
        //         vmath::rotate(scaleY , 0.0f, 1.0f, 0.0f) *
        //         vmath::rotate(scaleZ , 0.0f, 0.0f, 1.0f) *
        //         vmath::scale(1.0f , 1.0f , 1.0f);

                                
        //     glUniformMatrix4fv(programStaticPBR->getUniformLocation("mMat"), 1, GL_FALSE, modelMatrix);
        //     vehicleModel->draw(programStaticPBR);
        // }
        // modelMatrix = popMatrix();

}

    void update()
    {
        sceneCamera.time = globalTime;
    }

    void uninitialize()
    {
    }
};
