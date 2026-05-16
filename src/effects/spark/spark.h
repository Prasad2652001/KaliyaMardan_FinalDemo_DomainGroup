#pragma once
#include "SparkParticle.h"
#include "../../utils/OpenGL/GLHeadersAndMacros.h"
#include "../../utils/OpenGL/GLLog.h"
#include "../../utils/common.h"

#define MAX_SPARKS 500

class SparkEffect {
public:
    SparkParticle sparks[MAX_SPARKS];
    GLuint vao, vbo;

    void initialize() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SparkParticle) * MAX_SPARKS, sparks, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(SparkParticle), (void*)offsetof(SparkParticle, position));
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);

        glVertexAttribPointer(MATRIX_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(SparkParticle), (void*)offsetof(SparkParticle, color));
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_COLOR);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        for (int i = 0; i < MAX_SPARKS; i++) {
            resetSpark(i);
        }
    }

    void resetSpark(int i) {
        sparks[i].position[0] = 0.0f;
        sparks[i].position[1] = 0.0f;
        sparks[i].position[2] = 0.0f;

        float theta = random_float() * 2.0f * M_PI;
        float phi = acos(2.0f * random_float() - 1.0f);
        float speed = 10.0f + random_float() * 40.0f;

        sparks[i].velocity[0] = speed * sin(phi) * cos(theta);
        sparks[i].velocity[1] = speed * sin(phi) * sin(theta);
        sparks[i].velocity[2] = speed * cos(phi);

        sparks[i].color[0] = 1.0f;
        sparks[i].color[1] = 0.8f + random_float() * 0.2f;
        sparks[i].color[2] = 0.3f + random_float() * 0.2f;

        sparks[i].life = 1.0f;
        sparks[i].size = 3.0f + random_float() * 3.0f;
    }

    void update(float deltaTime) {
        for (int i = 0; i < MAX_SPARKS; i++) {
            if (sparks[i].life <= 0.0f) {
                resetSpark(i);
            }

            sparks[i].position[0] += sparks[i].velocity[0] * deltaTime;
            sparks[i].position[1] += sparks[i].velocity[1] * deltaTime;
            sparks[i].position[2] += sparks[i].velocity[2] * deltaTime;

            sparks[i].velocity[1] -= 9.8f * deltaTime * 0.2f; // gravity

            sparks[i].life -= deltaTime * 1.5f; // faster decay
        }
    }

    void render(GLuint shaderProgram, GLuint textureID, const float* modelMatrix, const float* viewMatrix, const float* projMatrix) {
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_modelMatrix"), 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_viewMatrix"), 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_projectionMatrix"), 1, GL_FALSE, projMatrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(shaderProgram, "u_textureSampler"), 0);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SparkParticle) * MAX_SPARKS, sparks);
        glDrawArrays(GL_POINTS, 0, MAX_SPARKS);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        glDisable(GL_PROGRAM_POINT_SIZE);
        glUseProgram(0);
    }

    void uninitialize() {
        if (vbo) {
            glDeleteBuffers(1, &vbo);
            vbo = 0;
        }
        if (vao) {
            glDeleteVertexArrays(1, &vao);
            vao = 0;
        }
    }

private:
    float random_float() {
        return (float)rand() / (float)RAND_MAX;
    }
};
