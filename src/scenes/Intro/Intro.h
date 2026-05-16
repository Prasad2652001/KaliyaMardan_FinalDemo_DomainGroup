#pragma once

#include "../../utils/common.h"
#include "../Scene.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define __STDC_CONSTANT_MACROS
#define __STDC_FORMAT_MACROS
#define __STDC_LIMIT_MACROS

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class IntroScene : public Scene
{
public:
    GLuint shaderProgramObject = 0;

    GLuint vao_square = 0;
    GLuint vbo_position_square = 0;
    GLuint vbo_texcoord_square = 0;

    GLuint mvpMatrixUniform = 0;
    GLuint textureSamplerUniform = 0;
    GLuint alphaUniform = 0;

    GLuint texture_video = 0;

    AVFormatContext* fmt = nullptr;
    AVCodecContext* dec = nullptr;
    SwsContext* sws = nullptr;
    AVFrame* frame = nullptr;
    AVPacket* pkt = nullptr;
    int videoStream = -1;

    uint8_t* rgbaBuffer = nullptr;
    uint8_t* rgbaData[4] = { 0 };
    int rgbaLinesize[4] = { 0 };

    int vidW = 0;
    int vidH = 0;

    LARGE_INTEGER freq;
    LARGE_INTEGER prev;
    double acc = 0.0;
    double frameTime = 1.0 / 30.0;

    enum sceneEventIds
    {
        START_T,
        FADE_IN,
        VIDEO_T,
        FADE_OUT,
        END_T,
    };

    IntroScene()
    {
        sceneCamera = new BezierCamera();
        sceneEvents = nullptr;
    }

    void setupCamera()
    {
        if (!sceneCamera)
            sceneCamera = new BezierCamera();

        sceneCamera->initialize();

        std::vector<std::vector<float>> pts =
        {
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}
        };

        std::vector<float> yaw =
        {
            0.0f,
            0.0f
        };

        std::vector<float> pitch =
        {
            0.0f,
            0.0f
        };

        std::vector<float> fov =
        {
            0.0f,
            0.0f
        };

        sceneCamera->setBezierPoints(pts, yaw, pitch, fov);
        sceneCamera->update();
    }

    bool initialize()
    {
        const GLchar* vertexShaderSourceCode =
            "#version 460 core\n"
            "in vec4 aPosition;\n"
            "in vec2 aTexCoord;\n"
            "uniform mat4 uMVPMatrix;\n"
            "out vec2 oTexCoord;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = uMVPMatrix * aPosition;\n"
            "    oTexCoord = aTexCoord;\n"
            "}\n";

        GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderObject, 1, &vertexShaderSourceCode, NULL);
        glCompileShader(vertexShaderObject);

        GLint status = 0;
        glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            PrintLog("IntroScene : Vertex shader compile failed\n");
            return FALSE;
        }

        const GLchar* fragmentShaderSourceCode =
            "#version 460 core\n"
            "in vec2 oTexCoord;\n"
            "uniform sampler2D uTextureSampler;\n"
            "uniform float uAlpha;\n"
            "out vec4 FragColor;\n"
            "void main(void)\n"
            "{\n"
            "    vec4 tex = texture(uTextureSampler, oTexCoord);\n"
            "    FragColor = vec4(tex.rgb, tex.a * uAlpha);\n"
            "}\n";

        GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderObject, 1, &fragmentShaderSourceCode, NULL);
        glCompileShader(fragmentShaderObject);

        glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            PrintLog("IntroScene : Fragment shader compile failed\n");
            return FALSE;
        }

        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "aPosition");
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_TEXTURE0, "aTexCoord");

        glLinkProgram(shaderProgramObject);
        glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            PrintLog("IntroScene : Shader program link failed\n");
            return FALSE;
        }

        glDeleteShader(vertexShaderObject);
        glDeleteShader(fragmentShaderObject);

        mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");
        textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "uTextureSampler");
        alphaUniform = glGetUniformLocation(shaderProgramObject, "uAlpha");

        const GLfloat square_position[] =
        {
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f
        };

        const GLfloat square_texcoords[] =
        {
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f
        };

        glGenVertexArrays(1, &vao_square);
        glBindVertexArray(vao_square);

        glGenBuffers(1, &vbo_position_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
        glBufferData(GL_ARRAY_BUFFER, sizeof(square_position), square_position, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_texcoord_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord_square);
        glBufferData(GL_ARRAY_BUFFER, sizeof(square_texcoords), square_texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        if (!InitVideoFFmpeg("./assets/video/amcVideo.mp4"))
        {
            PrintLog("IntroScene : InitVideoFFmpeg failed\n");
            return FALSE;
        }

        CreateVideoTexture(vidW, vidH);
        DecodeNextFrameRGBA();

        sceneEvents = new EventManager(
        {
            {START_T,  {0.0f, 10.0f}},
            {FADE_IN,  {0.0f, 1.5f}},
            {VIDEO_T,  {0.0f, 10.0f}},
            {FADE_OUT, {8.5f, 1.5f}},
            {END_T,    {10.0f, 0.0f}}
        }, true);

        setupCamera();
        // sceneCamera->initialize();
        // sceneCamera->setBezierPoints(
        //     { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },
        //     { 0.0f, 0.0f },
        //     { 0.0f, 0.0f },
        //     { -45.0f, -45.0f }
        // );
        // sceneCamera->update();

        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&prev);

        isInitialized = true;
        isSceneComplete = false;

        return TRUE;
    }

    void display()
    {
        if (!shaderProgramObject || !texture_video || !vao_square)
            return;

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(shaderProgramObject);

        mat4 modelViewMatrix = mat4::identity();
        mat4 modelViewProjectionMatrix = mat4::identity();
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

        float alpha = 1.0f;

        if (sceneEvents->isEventInProgress(FADE_IN))
        {
            alpha = lerp(sceneEvents->getEventTime(FADE_IN), 0.0f, 1.0f);
        }
        else if (sceneEvents->isEventInProgress(FADE_OUT))
        {
            alpha = lerp(sceneEvents->getEventTime(FADE_OUT), 1.0f, 0.0f);
        }

        glUniform1f(alphaUniform, alpha);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_video);
        glUniform1i(textureSamplerUniform, 0);

        glBindVertexArray(vao_square);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glUseProgram(0);

        glEnable(GL_DEPTH_TEST);
    }

    void update()
    {
        if (!sceneEvents)
            return;

        sceneEvents->increment();

        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);

        double dt = (double)(now.QuadPart - prev.QuadPart) / (double)freq.QuadPart;
        prev = now;
        acc += dt;

        if (acc >= frameTime)
        {
            DecodeNextFrameRGBA();
            acc = 0.0;
        }

        if (sceneEvents->isEventComplete(END_T))
        {
            isSceneComplete = true;
        }
    }

    void uninitialize()
    {
        CleanupVideoFFmpeg();

        if (texture_video)
        {
            glDeleteTextures(1, &texture_video);
            texture_video = 0;
        }

        if (vbo_texcoord_square)
        {
            glDeleteBuffers(1, &vbo_texcoord_square);
            vbo_texcoord_square = 0;
        }

        if (vbo_position_square)
        {
            glDeleteBuffers(1, &vbo_position_square);
            vbo_position_square = 0;
        }

        if (vao_square)
        {
            glDeleteVertexArrays(1, &vao_square);
            vao_square = 0;
        }

        if (shaderProgramObject)
        {
            glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }

        if (sceneEvents)
        {
            delete sceneEvents;
            sceneEvents = nullptr;
        }

        if (sceneCamera)
        {
            delete sceneCamera;
            sceneCamera = nullptr;
        }

        isInitialized = false;
        isSceneComplete = false;
    }

    bool InitVideoFFmpeg(const char* path)
    {
        if (avformat_open_input(&fmt, path, nullptr, nullptr) != 0)
            return false;

        if (avformat_find_stream_info(fmt, nullptr) < 0)
            return false;

        videoStream = -1;
        for (unsigned int i = 0; i < fmt->nb_streams; i++)
        {
            if (fmt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                videoStream = (int)i;
                break;
            }
        }

        if (videoStream < 0)
            return false;

        AVCodecParameters* cp = fmt->streams[videoStream]->codecpar;
        const AVCodec* codec = avcodec_find_decoder(cp->codec_id);
        if (!codec)
            return false;

        dec = avcodec_alloc_context3(codec);
        if (!dec)
            return false;

        if (avcodec_parameters_to_context(dec, cp) < 0)
            return false;

        if (avcodec_open2(dec, codec, nullptr) < 0)
            return false;

        vidW = dec->width;
        vidH = dec->height;

        frame = av_frame_alloc();
        pkt = av_packet_alloc();

        if (!frame || !pkt)
            return false;

        int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, vidW, vidH, 1);
        rgbaBuffer = (uint8_t*)av_malloc(numBytes);
        if (!rgbaBuffer)
            return false;

        av_image_fill_arrays(rgbaData, rgbaLinesize, rgbaBuffer, AV_PIX_FMT_RGBA, vidW, vidH, 1);

        sws = sws_getContext(
            vidW, vidH, dec->pix_fmt,
            vidW, vidH, AV_PIX_FMT_RGBA,
            SWS_BILINEAR, nullptr, nullptr, nullptr
        );

        if (!sws)
            return false;

        return true;
    }

    bool CreateVideoTexture(int w, int h)
    {
        glGenTextures(1, &texture_video);
        glBindTexture(GL_TEXTURE_2D, texture_video);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }

    bool DecodeNextFrameRGBA()
    {
        while (av_read_frame(fmt, pkt) >= 0)
        {
            if (pkt->stream_index == videoStream)
            {
                if (avcodec_send_packet(dec, pkt) == 0)
                {
                    av_packet_unref(pkt);

                    while (true)
                    {
                        int ret = avcodec_receive_frame(dec, frame);
                        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                            break;

                        if (ret < 0)
                            return false;

                        sws_scale(
                            sws,
                            frame->data, frame->linesize,
                            0, vidH,
                            rgbaData, rgbaLinesize
                        );

                        glBindTexture(GL_TEXTURE_2D, texture_video);
                        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                        glTexSubImage2D(
                            GL_TEXTURE_2D, 0, 0, 0, vidW, vidH,
                            GL_RGBA, GL_UNSIGNED_BYTE, rgbaData[0]
                        );
                        glBindTexture(GL_TEXTURE_2D, 0);

                        return true;
                    }
                }
            }
            else
            {
                av_packet_unref(pkt);
            }
        }

        av_seek_frame(fmt, videoStream, 0, AVSEEK_FLAG_BACKWARD);
        avcodec_flush_buffers(dec);
        return false;
    }

    void CleanupVideoFFmpeg()
    {
        if (sws) { sws_freeContext(sws); sws = nullptr; }
        if (frame) { av_frame_free(&frame); }
        if (pkt) { av_packet_free(&pkt); }

        if (dec) { avcodec_free_context(&dec); }
        if (fmt) { avformat_close_input(&fmt); }

        if (rgbaBuffer) { av_free(rgbaBuffer); rgbaBuffer = nullptr; }
    }
};
