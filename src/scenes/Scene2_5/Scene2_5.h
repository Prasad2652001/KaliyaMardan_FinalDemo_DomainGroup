// ***********************************************************************
// Scene 2.5 - Krishna and Kaliya face to face, submerged underwater.
// The whole scene is rendered into an offscreen FBO and then run through
// an underwater post-process (color grade + caustics + god rays + ripple).
// ***********************************************************************
#pragma once
#include "../../utils/common.h"
#include "../../utils/camera/BezierCamera.h"
#include "../../utils/camera/Camera.h"

// model related headers
#include "../../utils/gltf/Model.h"
#include "../../utils/gltf/AnimatedModel.h"
#include "../../shaders/modelgltf/glshaderloader.h"
#include "../../shaders/modelgltf/glLight.h"

#include "../../utils/EventManager.h"
#include "../Scene.h"

extern Camera camera;
extern BezierCamera *globalBezierCamera;
extern DOUBLE gDeltaTime;

class DemoScene2_5 : public Scene
{
public:
    // Models facing each other
    std::unique_ptr<Core::AnimatedModel> kaliya;
    std::unique_ptr<Core::AnimatedModel> krishna;

    SceneLight *lightManager = nullptr;

    // ---- underwater post-process resources ----
    glshaderprogram *underwaterShader = nullptr;
    GLuint quadVao = 0, quadVbo = 0;
    GLuint fbo = 0, fboColor = 0, fboDepth = 0;
    int fboW = 0, fboH = 0;

    float sceneTime = 0.0f;

    // Placement knobs (kept here so they are easy to tweak / iterate).
    float krishnaYaw = 90.0f;   // turn Krishna to face Kaliya
    float kaliyaYaw  = 270.0f;  // turn Kaliya to face Krishna (rotated 180)

    BezierCamera sc1;
    BezierCamera sc2;
    BezierCamera sc3;

    enum sceneEventIds
    {
        START_T,
        SC_T1,
        END_T,
    };

    DemoScene2_5()
    {
        sceneCamera = new BezierCamera();
    }

    bool initialize()
    {
        if (isInitialized)
            return TRUE;

        // Underwater post-process shader (loaded from disk at runtime).
        underwaterShader = new glshaderprogram(
            {"./src/shaders/underwater/underwater.vert",
             "./src/shaders/underwater/underwater.frag"});

        // Fullscreen quad (NDC) with texcoords.
        const GLfloat quad[] = {
            //  x      y     z     u     v
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        };
        glGenVertexArrays(1, &quadVao);
        glBindVertexArray(quadVao);
        glGenBuffers(1, &quadVbo);
        glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);

        // ---- Kaliya (the serpent) ----
        kaliya = std::make_unique<Core::AnimatedModel>();
        kaliya->LoadModel("./assets/models/scene3_models/Kaliya.glb");

        // ---- Krishna (Meshy AI dancing pose) ----
        // NOTE: the 13.5 MB Meshy FBX intermittently fails Assimp with a
        // "bad allocation" under memory pressure (same issue as Kaliya.fbx).
        // We load a GLB converted from that FBX, which Assimp parses cheaply.
        krishna = std::make_unique<Core::AnimatedModel>();
        krishna->LoadModel("./assets/models/scene3_models/Krishna_Fighting.glb");
        krishna->SetBaseColorTexture("./assets/models/scene3_models/KrishnaTandav/1stPosition/Meshy_AI_Dancing_Krishna_0608195926_texture.png");

        lightManager = new SceneLight();
        lightManager->addDirectionalLights({
            DirectionalLight(vec3(0.55f), 10.0f, vec3(0.0f, -1.0f, -1.0f)),
            DirectionalLight(vec3(0.55f), 10.0f, vec3(0.0f, -1.0f,  1.0f)),
            DirectionalLight(vec3(0.30f), 10.0f, vec3(0.0f, -1.0f,  0.0f))
        });
        lightManager->setAmbient(vec3(0.08f));

        sceneEvents = new EventManager(
            {{START_T, {0.0f, 30.0f}},
             {SC_T1, {0.0f, 28.0f}},
             {END_T, {30.0f, 0.0f}}},
            true);

        setupCamera();

        // sceneCamera->initialize();
        // sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal, fovGlobal);
        // sceneCamera->handlePerspective = true;
       
        isInitialized = true;
        isSceneComplete = false;
        return TRUE;
    }

    void setupCamera()
    {
        std::vector<std::vector<float>> bezierPointsSC1 = {
            {96.399918f, 85.500046f, -1.499983f},
            {75.399918f, 85.500046f, -1.499983f},
            {60.399918f, 85.500046f, -1.499983f},
            {59.399918f, 85.500046f, -1.499983f},
            {58.399918f, 85.500046f, -1.499983f},
            {57.399918f, 85.500046f, -1.499983f},
            {56.399918f, 85.500046f, -1.499983f},
            {55.399918f, 85.500046f, -1.499983f},
            {54.399918f, 85.500046f, -1.499983f},
            {53.399918f, 85.500046f, -1.499983f},
            {44.399918f, 77.500046f, -1.499983f},
            {31.399918f, 77.500046f, -1.499983f},
            {30.399918f, 77.500046f, -1.499983f},
            {8.399918f, 77.500046f, -1.499983f},
            {8.399918f, 77.500046f, -5.499983f},
            {8.399918f, 77.500046f, -8.499983f},
            {8.399918f, 77.500046f, -9.499983f},
            {8.399918f, 77.500046f, -16.499983f},
            {8.399918f, 77.500046f, -25.499983f},
            {8.399918f, 77.500046f, -35.499985f},
            {-9.600082f, 77.500046f, -35.499985f},
            {-16.600082f, 77.500046f, -35.499985f},
            {-24.600082f, 77.500046f, -35.499985f},
            {-32.600082f, 77.500046f, -35.499985f},
            {-42.600082f, 77.500046f, -35.499985f},
            {-42.600082f, 77.500046f, -35.499985f},
            {-134.600082f, 77.500046f, -35.499985f},
            {-135.600082f, 77.500046f, -35.499985f},
            {-136.600082f, 77.500046f, -35.499985f},
            {-137.600082f, 77.500046f, -35.499985f},
            {-138.600082f, 77.500046f, -35.499985f},
            {-139.600082f, 77.500046f, -35.499985f},
            {-140.600082f, 77.500046f, -35.499985f},
            {-141.600082f, 77.500046f, -35.499985f},
            {-142.600082f, 77.500046f, -35.499985f},
            {-143.600082f, 77.500046f, -35.499985f},
            {-144.600082f, 77.500046f, -35.499985f},
            {-145.600082f, 50.500046f, -35.499985f},
            {-154.600082f, 50.500046f, -35.499985f},
            {-162.600082f, 50.500046f, -35.499985f},
            {-171.600082f, 50.500046f, -35.499985f},
            {-181.600082f, 50.500046f, -35.499985f},
            {-181.600082f, 50.500046f, -35.499985f},
            {-226.600082f, 50.500046f, -35.499985f},
            {-228.600082f, 50.500046f, -35.499985f},
            {-229.600082f, 50.500046f, -35.499985f},
            {-230.600082f, 50.500046f, -35.499985f},
            {-231.600082f, 50.500046f, -35.499985f},
            {-232.600082f, 50.500046f, -35.499985f},
            {-233.600082f, 50.500046f, -35.499985f},
            {-234.600082f, 50.500046f, -35.499985f},
            {-235.600082f, 50.500046f, -35.499985f},
            {-236.600082f, 50.500046f, -35.499985f},
            {-237.600082f, 50.500046f, -35.499985f},
            {-238.600082f, 50.500046f, -35.499985f},
            {-239.600082f, 50.500046f, -35.499985f},
            {-240.600082f, 50.500046f, -35.499985f},
            {-241.600082f, 50.500046f, -35.499985f},
            {-242.600082f, 50.500046f, -35.499985f},
            {-242.600082f, 50.500046f, -35.499985f},
            {-243.600082f, 50.500046f, 12.500015f},
            {-243.600082f, 50.500046f, 12.500015f},
            {-243.600082f, 50.500046f, 30.500015f},
            {-243.600082f, 50.500046f, 46.500015f},
            {-243.600082f, 50.500046f, 80.500015f},
            {-243.600082f, 50.500046f, 172.500015f},
            {-203.600082f, 43.500046f, 172.500015f},
            {-147.600082f, 43.500046f, 172.500015f},
            {-176.600082f, 38.500046f, 234.500015f},
            {-132.600082f, 38.500046f, 299.500000f},
            {-112.600082f, 38.500046f, 299.500000f},
            {-1.600082f, 52.500046f, 253.500000f},
            {27.399918f, 52.500046f, 253.500000f},
            {27.399918f, 52.500046f, 253.500000f},
            };
            
            
            // YAW GLOBAL
            std::vector<float> yawGlobalSC1 = {
            -1.000000f,
            -1.000000f,
            -1.000000f,
            -1.000000f,
            -1.000000f,
            -1.000000f,
            -1.000000f,
            -1.000000f,
            -1.000000f,
            -1.000000f,
            -1.000000f,
            -1.000000f,
            -1.000000f,
            -1.000000f,
            -1.000000f,
            1.000000f,
            2.000000f,
            11.000000f,
            18.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            22.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            13.000000f,
            14.000000f,
            1.000000f,
            -9.000000f,
            -14.000000f,
            -26.000000f,
            -39.000000f,
            -46.000000f,
            -57.000000f,
            -57.000000f,
            -66.000000f,
            -73.000000f,
            -87.000000f,
            -94.000000f,
            -94.000000f,
            };
            
            // PITCH GLOBAL
            std::vector<float> pitchGlobalSC1 = {
            11.000000f,
                11.000000f,
                11.000000f,
                11.000000f,
                11.000000f,
                11.000000f,
                11.000000f,
                11.000000f,
                11.000000f,
                11.000000f,
                11.000000f,
                11.000000f,
                3.000000f,
                3.000000f,
                3.000000f,
                3.000000f,
                3.000000f,
                3.000000f,
                3.000000f,
                3.000000f,
                3.000000f,
                3.000000f,
                3.000000f,
                3.000000f,
                -2.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                -4.000000f,
                };
            
            
            // FOV GLOBAL
            std::vector<float> fovGlobalSC1 = {
            -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                -120.000000f,
                };

        sc1.initialize();
        sc1.setBezierPoints(bezierPointsSC1, yawGlobalSC1, pitchGlobalSC1, fovGlobalSC1);
        sc1.update();
    }

    void ensureFBO(int w, int h)
    {
        if (w == fboW && h == fboH && fbo != 0)
            return;

        if (fboColor) { glDeleteTextures(1, &fboColor); fboColor = 0; }
        if (fboDepth) { glDeleteRenderbuffers(1, &fboDepth); fboDepth = 0; }
        if (fbo)      { glDeleteFramebuffers(1, &fbo); fbo = 0; }

        fboW = w; fboH = h;

        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glGenTextures(1, &fboColor);
        glBindTexture(GL_TEXTURE_2D, fboColor);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColor, 0);

        glGenRenderbuffers(1, &fboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            PrintLog("Scene2_5: underwater FBO incomplete\n");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void display()
    {
        // Advance post-process time here (MainScene does not call update()).
        sceneTime += (float)gDeltaTime;

        int w = (int)giWindowWidth  > 0 ? (int)giWindowWidth  : 1920;
        int h = (int)giWindowHeight > 0 ? (int)giWindowHeight : 1080;
        ensureFBO(w, h);

        modelMatrix = mat4::identity();
        perspectiveProjectionMatrix = vmath::perspective(
            45.0f, (GLfloat)w / (GLfloat)h, 1.0f, 10000000.0f);

        // sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal);
        // sceneCamera->update();


        // -------- 1) render the 3D scene into the offscreen FBO -----------
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, w, h);
        // Deep teal water as the background the post-process grades from.
        glClearColor(0.02f, 0.10f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        drawKrishna();
        drawKaliya();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // -------- 2) underwater post-process to the screen ---------------
        glViewport(0, 0, w, h);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        underwaterShader->use();
        glUniform1f(underwaterShader->getUniformLocation("u_time"), sceneTime);
        glUniform1f(underwaterShader->getUniformLocation("u_strength"), 1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fboColor);
        glUniform1i(underwaterShader->getUniformLocation("u_scene"), 0);

        glBindVertexArray(quadVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);

        // sceneCamera->displayBezierCurve();
    }

    void drawKaliya()
    {
        if (!kaliya)
            return;

        kaliya->Update((float)gDeltaTime);

        pushMatrix(modelMatrix);
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            vmath::mat4 m =
                vmath::translate(110.0f , 70.0f, 0.0f) *
                vmath::rotate(kaliyaYaw, 0.0f, 1.0f, 0.0f) *
                vmath::scale(55.0f, 55.0f, 55.0f);

            kaliya->mShader->Use();
            kaliya->mShader->SetUniform("isBlack", false);
            kaliya->mShader->SetUniform("u_model", m);
            kaliya->mShader->SetUniform("u_view", viewMatrix);
            kaliya->mShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            kaliya->mShader->SetUniform("u_LightPosition", vec4(0.0f, 200.0f, 200.0f, 1.0f));
            kaliya->mShader->SetUniform("u_ApplyToon", false);
            kaliya->mShader->SetSampler2D("u_GGXLUT", 0, 5);
            kaliya->mShader->SetUniform("u_DebugMode", 0);
            kaliya->mShader->SetUniform("u_Alpha", 1.0f);

            kaliya->Draw(kaliya->mShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void drawKrishna()
    {
        if (!krishna)
            return;

        krishna->Update((float)gDeltaTime);

        pushMatrix(modelMatrix);
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // GLB export is Y-up (already upright); just yaw to face Kaliya.
            vmath::mat4 m =
                vmath::translate(-110.0f + 10.399918f, 20.0f + 23.500046f, 0.0f + 15.500017f) *
                vmath::rotate(krishnaYaw, 0.0f, 1.0f, 0.0f) *
                vmath::scale(28.0f, 28.0f, 28.0f);

            krishna->mShader->Use();
            krishna->mShader->SetUniform("isBlack", false);
            krishna->mShader->SetUniform("u_model", m);
            krishna->mShader->SetUniform("u_view", viewMatrix);
            krishna->mShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            krishna->mShader->SetUniform("u_LightPosition", vec4(0.0f, 200.0f, 200.0f, 1.0f));
            krishna->mShader->SetUniform("u_ApplyToon", false);
            krishna->mShader->SetSampler2D("u_GGXLUT", 0, 5);
            krishna->mShader->SetUniform("u_DebugMode", 0);
            krishna->mShader->SetUniform("u_Alpha", 1.0f);

            krishna->Draw(krishna->mShader);

            glDisable(GL_BLEND);
        }
        modelMatrix = popMatrix();
    }

    void update()
    {
        sceneCamera->time = globalTime;

        // CAMERA UPDATE
        sceneCamera->time = sceneEvents->getEventTime(START_T);
        sceneEvents->increment();

        // CAMERA UPDATE
        if (sceneEvents->isEventInProgress(SC_T1))
        {
            sceneCamera = &sc1;
            sceneCamera->time = sceneEvents->getEventTime(SC_T1);
        }

        if (sceneEvents->isEventComplete(END_T))
            isSceneComplete = true;
        
        // sceneCamera->update();
    }

    void uninitialize()
    {
        kaliya.reset();
        krishna.reset();

        if (lightManager) { delete lightManager; lightManager = nullptr; }
        if (underwaterShader) { delete underwaterShader; underwaterShader = nullptr; }

        if (fboColor) { glDeleteTextures(1, &fboColor); fboColor = 0; }
        if (fboDepth) { glDeleteRenderbuffers(1, &fboDepth); fboDepth = 0; }
        if (fbo)      { glDeleteFramebuffers(1, &fbo); fbo = 0; }
        if (quadVbo)  { glDeleteBuffers(1, &quadVbo); quadVbo = 0; }
        if (quadVao)  { glDeleteVertexArrays(1, &quadVao); quadVao = 0; }
    }
};
