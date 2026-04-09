#pragma once
#include "../../utils/common.h"
#include "../../utils/opencv.hpp"

// OpenCL / Video
#include <opencv2/core/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "../Scene.h"
#pragma comment(lib, "opencv_world470.lib")

// Video
using namespace cv;
void toggleFrameLimit(bool enable);

class Scene1 : public Scene
{
public:
    TextureShader *textureShader;
    GLuint vao_Quad;
    GLuint vbo_Quad_Position;
    GLuint vbo_Quad_Texture;

    VideoCapture cap;
    char *prevVideo = NULL;

    // Video Related
    // Texture
    GLuint texture_Video;
    unsigned char *image;
    int videoHeight;
    int videoWidth;
    Mat frame;

    // EVENT
    // EVENT
    enum sceneEventIds
    {
        START_T,
        FADE_IN,
        FADE_OUT,
        END_T,
    };

    Scene1()
    {
        sceneCamera = new BezierCamera();
    }

    bool initialize()
    {
        textureShader = new TextureShader();
        if (!textureShader->initialize())
        {
            PrintLog("Error initializing textureShadeer\n");
            return FALSE;
        }

        const GLfloat quadPosition[] =
            {
                // top
                1.0f,
                1.0f,
                0.0f,
                -1.0f,
                1.0f,
                0.0f,
                -1.0f,
                -1.0f,
                0.0f,
                1.0f,
                -1.0f,
                0.0f};

        GLfloat quadTexcoord[] =
            {
                1.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f};

        // Event System
        sceneEvents = new EventManager(
            {{START_T, {0.0f, 27.0f}},
             {FADE_IN, {0.0f, 3.0f}},
             {FADE_OUT, {25.5f, 1.5f}},
             {END_T, {27.0f, 0.0f}}},
            true);

        // QUAD
        glGenVertexArrays(1, &vao_Quad);
        glBindVertexArray(vao_Quad);

        // Position
        glGenBuffers(1, &vbo_Quad_Position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Quad_Position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadPosition), quadPosition, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Texture
        glGenBuffers(1, &vbo_Quad_Texture);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Quad_Texture);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadTexcoord), quadTexcoord, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        // Enabling the texture
        if (open_video("./assets/video/amcVideo.mp4") == -1)
        {
            uninitialize();
            return FALSE;
        }

        sceneCamera->initialize();
        sceneCamera->handlePerspective = true;
        sceneCamera->setBezierPoints(bezierPoints, yawGlobal, pitchGlobal);

        isInitialized = true;
        toggleFrameLimit(true);
        return TRUE;
    }
    void setupCamera() {}
    void display()
    {
        glUseProgram(textureShader->shaderProgramObject);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // TRANSFORMATIONS

        // Center Quad
        pushMatrix(modelMatrix);
        {
            // viewMatrix = mat4::identity();
            modelMatrix = modelMatrix * vmath::scale(5.5f, 3.0f, 1.0f);
            modelMatrix = modelMatrix * vmath::translate(0.0f, 0.0f, -7.0f + 9.6f);
            glUniformMatrix4fv(textureShader->modelMatrixUniform, 1, GL_FALSE, vmath::scale(1.0f + 0.01f, 1.0f, 1.0f));
            glUniformMatrix4fv(textureShader->viewMatrixUniform, 1, GL_FALSE, mat4::identity());
            glUniformMatrix4fv(textureShader->projectionMatrixUniform, 1, GL_FALSE, mat4::identity());

            // Bind with texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_Video);
            glBindVertexArray(vao_Quad);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        modelMatrix = popMatrix();

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

        glDisable(GL_BLEND);

        glUseProgram(0);
    }
    bool areFramesOver = false;
    void update()
    {
        // Video
        sceneEvents->increment();

        if (!areFramesOver && !cap.read(frame))
        {
            areFramesOver = true;
            return;
        }

        if (sceneEvents->isEventComplete(END_T))
        {
            toggleFrameLimit(false);
            isSceneComplete = TRUE;
            return;
        }

        if (!areFramesOver)
        {

            if (frame.empty())
                PrintLog("Empty Frame\n");
            else
                image = cvMat2TexInput(frame);

            if (image)
            {
                glBindTexture(GL_TEXTURE_2D, texture_Video);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoWidth, videoHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            else
                PrintLog("Failed to load video texture\n");
        }
    }

    void uninitialize()
    {
        if (texture_Video)
        {
            glDeleteTextures(1, &texture_Video);
            texture_Video = 0;
        }

        if (vbo_Quad_Texture)
        {
            glDeleteBuffers(1, &vbo_Quad_Texture);
            vbo_Quad_Texture = 0;
        }

        if (vbo_Quad_Position)
        {
            glDeleteBuffers(1, &vbo_Quad_Position);
            vbo_Quad_Position = 0;
        }

        if (vao_Quad)
        {
            glDeleteVertexArrays(1, &vao_Quad);
            vao_Quad = 0;
        }
    }

    // video
    unsigned char *cvMat2TexInput(Mat &img)
    {
        cvtColor(img, img, COLOR_BGR2RGB);
        return img.data;
    }

    int open_video(std::string videoName)
    {
        cap.open(videoName);
        if (!cap.isOpened())
        {
            printf("Failed to open Video\n");
            return -1;
        }

        cap >> frame;
        videoHeight = frame.rows;
        videoWidth = frame.cols;
        image = cvMat2TexInput(frame);

        glGenTextures(1, &texture_Video);
        glBindTexture(GL_TEXTURE_2D, texture_Video);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        return 0;
    }
};
