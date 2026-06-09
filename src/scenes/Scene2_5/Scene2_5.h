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
        krishna->LoadModel("./assets/models/scene3_models/KrishnaTandav/1stPosition/Krishna_1st.glb");
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

        // Default free-camera preset: looking along -Z at the two characters.
        camera.position = vmath::vec3(0.0f, 120.0f, 300.0f);
        camera.yaw = 270.0f;
        camera.pitch = -5.0f;
        camera.updateCameraVectors();

        isInitialized = true;
        isSceneComplete = false;
        return TRUE;
    }

    void setupCamera()
    {
        std::vector<std::vector<float>> pts = {
            {220.0f, 170.0f, 430.0f},
            {120.0f, 160.0f, 460.0f},
            {0.0f,   150.0f, 470.0f},
            {-120.0f,160.0f, 460.0f},
            {-220.0f,170.0f, 430.0f},
        };
        std::vector<float> yaw   = {250.0f, 262.0f, 270.0f, 278.0f, 290.0f};
        std::vector<float> pitch = {-8.0f, -8.0f, -8.0f, -8.0f, -8.0f};
        std::vector<float> fov   = {45.0f, 45.0f, 45.0f, 45.0f, 45.0f};

        sc1.initialize();
        sc1.setBezierPoints(pts, yaw, pitch, fov);
        sc1.update();
        sceneCamera = &sc1;
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
                vmath::translate(110.0f, 70.0f, 0.0f) *
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
                vmath::translate(-110.0f, 20.0f, 0.0f) *
                vmath::rotate(krishnaYaw, 0.0f, 1.0f, 0.0f) *
                vmath::scale(48.0f, 48.0f, 48.0f);

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
        sceneEvents->increment();

        if (sceneEvents->isEventInProgress(SC_T1))
        {
            sceneCamera = &sc1;
            sceneCamera->time = sceneEvents->getEventTime(SC_T1);
        }

        if (sceneEvents->isEventComplete(END_T))
            isSceneComplete = true;
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
