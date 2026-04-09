#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include "../../utils/common.h"


#define DL_SHADER_ES 100
#define DL_SHADER_CORE 101

class glshaderprogram
{
private:
	static std::unordered_map<std::string, GLuint> shaderMap;
	std::unordered_map<std::string, GLint> uniforms;

public:
	GLuint programObject;
	glshaderprogram(std::initializer_list<std::string> shaderList, int version = 460, int profile = DL_SHADER_CORE);
	void use(void);
	GLint getUniformLocation(std::string uniformName);

	~glshaderprogram(void);
};
