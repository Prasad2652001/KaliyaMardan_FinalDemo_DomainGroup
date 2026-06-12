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

    // Krishna is animated by cross-fading three static dance poses
    // (fade in / fade out between poses, sequenced 1 -> 2 -> 3 -> 1 on a loop).
    static const int KRISHNA_POSE_COUNT = 3;
    std::unique_ptr<Core::AnimatedModel> krishnaPoses[KRISHNA_POSE_COUNT];
    float krishnaFadeTime = 0.0f;

    // Combined Krishna+Kaliya finale model: after the three poses have played,
    // it replaces both the standalone Kaliya and the dancing Krishna.
    std::unique_ptr<Core::AnimatedModel> krishnaKaliya;

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

    // Push every character back in -Z so the shot feels viewed from a distance.
    float sceneDepthZ = -250.0f;

    // Expand the camera path outward from the subject so the larger (3x) scaled
    // scene fits fully in frame (1.0 = original size, bigger = wider/further).
    float cameraPathScale = 2.4f;

    // ---- finale zoom-in -------------------------------------------------
    // Once the combined Krishna_Kaliya model appears, we slowly narrow the
    // field of view. A smaller FOV magnifies the subject, giving a "push in"
    // zoom without moving the camera. The zoom follows a table of control
    // points (see currentFov) so it can be shaped gradually instead of one
    // single ramp.
    static constexpr float KR_HOLD_DUR = 0.933f;            // pose hold time
    static constexpr float KR_FADE_DUR = 0.667f;            // cross-fade time
    static constexpr float KR_SLOT_DUR = KR_HOLD_DUR + KR_FADE_DUR;
    float finaleTime = 0.0f;    // real seconds elapsed since the finale began
    float fovStart   = 45.0f;   // normal field of view (degrees)

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

        // ---- Krishna (three Meshy AI dance poses, cross-faded) ----
        // These GLBs embed their textures, so no base-color override is needed.
        const char *krishnaPosePaths[KRISHNA_POSE_COUNT] = {
            "./assets/models/scene2_5_models/Krishna_pos1.glb",
            "./assets/models/scene2_5_models/Krishna_pos2.glb",
            "./assets/models/scene2_5_models/Krishna_pos3.glb",
        };
        for (int i = 0; i < KRISHNA_POSE_COUNT; ++i)
        {
            krishnaPoses[i] = std::make_unique<Core::AnimatedModel>();
            krishnaPoses[i]->LoadModel(krishnaPosePaths[i]);
        }

        // Combined Krishna+Kaliya model for the finale (shown after the 3 poses).
        krishnaKaliya = std::make_unique<Core::AnimatedModel>();
        krishnaKaliya->LoadModel("./assets/models/scene2_5_models/Krishna_Kaliya.glb");

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

        // Shift the whole camera path back in -Z (to follow the moved models),
        // then scale it outward from the subject centre so the larger scaled
        // scene fits fully in the visible frame.
        const float cx = 0.0f, cy = 50.0f, cz = sceneDepthZ; // subject centre
        for (auto &p : bezierPointsSC1)
        {
            if (p.size() < 3)
                continue;
            p[2] += sceneDepthZ; // follow the -Z model push
            p[0] = cx + (p[0] - cx) * cameraPathScale;
            p[1] = cy + (p[1] - cy) * cameraPathScale;
            p[2] = cz + (p[2] - cz) * cameraPathScale;
        }

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
            currentFov(), (GLfloat)w / (GLfloat)h, 1.0f, 10000000.0f);

        // sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal);
        // sceneCamera->update();


        // -------- 1) render the 3D scene into the offscreen FBO -----------
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, w, h);
        // Deep teal water as the background the post-process grades from.
        glClearColor(0.02f, 0.10f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        drawCharacters();

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

    // Field of view for the current frame. Stays at fovStart during the dance,
    // then eases through a series of control points once the combined finale
    // model is on screen, producing a slow, gradual zoom-in.
    //
    // To shape the zoom, edit the (time, fov) points below: time is seconds
    // since the finale began, fov is degrees (smaller = more zoomed in). Times
    // must stay in ascending order. More closely spaced points = gentler,
    // more gradual motion.
    float currentFov()
    {
        float finaleStart = KRISHNA_POSE_COUNT * KR_SLOT_DUR;
        if (krishnaFadeTime < finaleStart)
            return fovStart;

        // (time in seconds, fov in degrees). The fov values are spaced so the
        // *magnification* (proportional to 1/tan(fov/2)) grows by an equal
        // amount every 2s. Linearly dropping fov would make the zoom appear to
        // accelerate (slow, then a sudden rush); equal-magnification steps make
        // the perceived zoom constant and gradual instead.
        struct FovKey { float t; float fov; };
        static const FovKey keys[] = {
            {  0.0f, 45.0f },
            {  2.0f, 40.8f },
            {  4.0f, 37.4f },
            {  6.0f, 34.4f },
            {  8.0f, 31.9f },
            { 10.0f, 29.7f },
            { 12.0f, 27.8f },
            { 14.0f, 26.1f },
            { 16.0f, 24.6f },
            { 18.0f, 23.3f },
            { 20.0f, 22.1f },
            { 22.0f, 21.0f },
            { 24.0f, 20.0f },
        };
        const int n = (int)(sizeof(keys) / sizeof(keys[0]));

        float tt = finaleTime;
        if (tt <= keys[0].t)     return keys[0].fov;
        if (tt >= keys[n - 1].t) return keys[n - 1].fov;

        for (int i = 0; i < n - 1; ++i)
        {
            if (tt >= keys[i].t && tt <= keys[i + 1].t)
            {
                float span = keys[i + 1].t - keys[i].t;
                float p = span > 0.0f ? (tt - keys[i].t) / span : 1.0f;
                // smootherstep within each segment so the joints between
                // control points have matching velocity/acceleration.
                p = p * p * p * (p * (p * 6.0f - 15.0f) + 10.0f);
                return keys[i].fov + (keys[i + 1].fov - keys[i].fov) * p;
            }
        }
        return keys[n - 1].fov;
    }

    // Kaliya's placement (scaled 3x). Shared by the standalone Kaliya and the
    // combined Krishna_Kaliya finale model so they sit in the same spot.
    vmath::mat4 kaliyaMatrix()
    {
        return vmath::translate(110.0f, 70.0f, sceneDepthZ) *
               vmath::rotate(kaliyaYaw, 0.0f, 1.0f, 0.0f) *
               vmath::scale(165.0f, 165.0f, 165.0f);
    }

    // Combined finale model: centered in the scene (x = 0), turned +90 degrees
    // about Y.
    vmath::mat4 krishnaKaliyaMatrix()
    {
        return vmath::translate(0.0f, 70.0f, sceneDepthZ) *
               vmath::rotate(kaliyaYaw + 90.0f, 0.0f, 1.0f, 0.0f) *
               vmath::scale(165.0f, 165.0f, 165.0f);
    }

    // Krishna's X position at each step of his approach. He starts far from
    // Kaliya and takes one clear step forward on every cross-fade.
    float krishnaStepX(int step)
    {
        const float xs[KRISHNA_POSE_COUNT] = { -110.0f, -50.0f, 10.0f };
        if (step < 0) step = 0;
        if (step >= KRISHNA_POSE_COUNT) step = KRISHNA_POSE_COUNT - 1;
        return xs[step];
    }

    // Generic textured draw with optional transparency for cross-fading.
    void drawAnimModel(Core::AnimatedModel *model, const vmath::mat4 &m, float alpha)
    {
        if (!model || alpha <= 0.001f)
            return;

        bool opaque = alpha >= 0.999f;
        if (opaque) { glDisable(GL_BLEND); glDepthMask(GL_TRUE); }
        else        { glEnable(GL_BLEND);  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glDepthMask(GL_FALSE); }

        pushMatrix(modelMatrix);
        {
            model->mShader->Use();
            model->mShader->SetUniform("isBlack", false);
            model->mShader->SetUniform("u_model", m);
            model->mShader->SetUniform("u_view", viewMatrix);
            model->mShader->SetUniform("u_projection", perspectiveProjectionMatrix);
            model->mShader->SetUniform("u_LightPosition", vec4(0.0f, 200.0f, 200.0f, 1.0f));
            model->mShader->SetUniform("u_ApplyToon", false);
            model->mShader->SetSampler2D("u_GGXLUT", 0, 5);
            model->mShader->SetUniform("u_DebugMode", 0);
            model->mShader->SetUniform("u_UseAlpha", !opaque);
            model->mShader->SetUniform("u_Alpha", alpha);

            model->Draw(model->mShader);
        }
        modelMatrix = popMatrix();

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    void drawPose(int idx, float x, float alpha)
    {
        if (idx < 0 || idx >= KRISHNA_POSE_COUNT)
            return;
        vmath::mat4 m =
            vmath::translate(x, 20.0f, sceneDepthZ) *
            vmath::rotate(krishnaYaw, 0.0f, 1.0f, 0.0f) *
            vmath::scale(48.0f, 48.0f, 48.0f);
        drawAnimModel(krishnaPoses[idx].get(), m, alpha);
    }

    // Orchestrates Scene 2.5's characters:
    //  - Krishna holds pose 1 until the camera transition is done.
    //  - Then he cross-fades pose1 -> pose2 -> pose3, stepping forward toward
    //    Kaliya on every fade. The dance runs at 1/3 speed.
    //  - After pose3 fades out (the 3rd fade), both the standalone Kaliya and
    //    Krishna are replaced by the combined Krishna_Kaliya model.
    void drawCharacters()
    {
        if (kaliya) kaliya->Update((float)gDeltaTime);

        // (1) Freeze on pose 1 until the camera has finished its move.
        bool cameraReady = sceneEvents && sceneEvents->isEventComplete(SC_T1);
        if (!cameraReady)
            krishnaFadeTime = 0.0f;
        else
            krishnaFadeTime += (float)gDeltaTime / 3.0f; // (3) 3x slower

        const float holdDur = KR_HOLD_DUR;
        const float fadeDur = KR_FADE_DUR;
        const float slotDur = KR_SLOT_DUR;

        int   stage = (int)(krishnaFadeTime / slotDur); // 0,1,2 dance; >=3 finale
        float local = krishnaFadeTime - stage * slotDur;

        // Finale: combined Krishna+Kaliya model replaces everything.
        if (stage >= KRISHNA_POSE_COUNT)
        {
            finaleTime += (float)gDeltaTime; // drives the FOV zoom-in
            if (krishnaKaliya) krishnaKaliya->Update((float)gDeltaTime);
            drawAnimModel(krishnaKaliya.get(), krishnaKaliyaMatrix(), 1.0f);
            return;
        }

        // Cross-dissolve factor within the current stage (0 during the hold).
        float f = 0.0f;
        bool  fading = local >= holdDur;
        if (fading)
        {
            f = (local - holdDur) / fadeDur;
            f = f < 0.0f ? 0.0f : (f > 1.0f ? 1.0f : f);
            f = f * f * (3.0f - 2.0f * f); // smoothstep ease
        }

        if (stage < KRISHNA_POSE_COUNT - 1)
        {
            // Stages 0 and 1: pose[stage] -> pose[stage+1], Kaliya stays solid.
            drawAnimModel(kaliya.get(), kaliyaMatrix(), 1.0f);
            drawPose(stage, krishnaStepX(stage), fading ? (1.0f - f) : 1.0f);
            if (fading)
                drawPose(stage + 1, krishnaStepX(stage + 1), f);
        }
        else
        {
            // Stage 2 (last pose): hold pose3, then its fade-out cross-dissolves
            // into the combined model while the standalone Kaliya fades away.
            if (!fading)
            {
                drawAnimModel(kaliya.get(), kaliyaMatrix(), 1.0f);
                drawPose(KRISHNA_POSE_COUNT - 1, krishnaStepX(KRISHNA_POSE_COUNT - 1), 1.0f);
            }
            else
            {
                drawAnimModel(kaliya.get(), kaliyaMatrix(), 1.0f - f);
                drawPose(KRISHNA_POSE_COUNT - 1, krishnaStepX(KRISHNA_POSE_COUNT - 1), 1.0f - f);
                if (krishnaKaliya) krishnaKaliya->Update((float)gDeltaTime);
                drawAnimModel(krishnaKaliya.get(), krishnaKaliyaMatrix(), f);
            }
        }
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
        for (int i = 0; i < KRISHNA_POSE_COUNT; ++i)
            krishnaPoses[i].reset();
        krishnaKaliya.reset();

        if (lightManager) { delete lightManager; lightManager = nullptr; }
        if (underwaterShader) { delete underwaterShader; underwaterShader = nullptr; }

        if (fboColor) { glDeleteTextures(1, &fboColor); fboColor = 0; }
        if (fboDepth) { glDeleteRenderbuffers(1, &fboDepth); fboDepth = 0; }
        if (fbo)      { glDeleteFramebuffers(1, &fbo); fbo = 0; }
        if (quadVbo)  { glDeleteBuffers(1, &quadVbo); quadVbo = 0; }
        if (quadVao)  { glDeleteVertexArrays(1, &quadVao); quadVao = 0; }
    }
};
