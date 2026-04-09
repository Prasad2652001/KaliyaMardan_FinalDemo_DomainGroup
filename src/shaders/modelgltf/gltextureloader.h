#pragma once

#include<string>
#include "../../utils/common.h"


void initTextureLoader();
GLuint createTexture2D(std::string filename, GLint minFilter = GL_NEAREST_MIPMAP_NEAREST, GLint magFilter = GL_NEAREST, GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT);
GLuint createTextureCubemap(std::string filename, GLint minFilter = GL_NEAREST_MIPMAP_NEAREST, GLint magFilter = GL_NEAREST, GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT);
unsigned int LoadEmbeddedTexture(const aiTexture *texture);

bool isTexturePresent(std::string filename);
