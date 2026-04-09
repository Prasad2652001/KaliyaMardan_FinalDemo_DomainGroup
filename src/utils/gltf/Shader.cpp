#include "Shader.h"

Core::Shader::Shader() : mProgram(0), mbLinked(false) {}

Core::Shader::~Shader() = default;

void Core::Shader::CompileShader(const std::string& filename, Type shaderType)
{
	std::ifstream infs(filename);
	if (!infs)
	{
		PrintLogFunction(__FUNCTION__,"\nFailed to load shader file %s\n", filename.c_str());
		return;
	}
#ifdef _DEBUG
	PrintLogFunction(__FUNCTION__, "Shader File: %s\n", filename.c_str());
#endif
	std::stringstream filestream;
	filestream << infs.rdbuf();
	infs.close();

	if (mProgram <= 0)
	{
		mProgram = glCreateProgram();
	}

	std::string sourceCode = filestream.str();
	const GLchar* shaderSourceCode = sourceCode.c_str();

	GLuint shaderObject = glCreateShader(static_cast<GLenum>(shaderType));
	glShaderSource(shaderObject, 1, &shaderSourceCode, nullptr);
	glCompileShader(shaderObject);

	GLint status = 0;

	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength = 0;

		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			GLsizei written = 0;
			std::string errorMessage(infoLogLength, ' ');

			glGetShaderInfoLog(shaderObject, infoLogLength, &written, &errorMessage[0]);
			glDeleteShader(shaderObject);

			PrintLogFunction(__FUNCTION__,"Shader Compilation Failed %s\n", errorMessage.c_str());
		}
	}

	glAttachShader(mProgram, shaderObject);
	glDeleteShader(shaderObject);

}

void Core::Shader::BindAttribLocation(GLuint location, const std::string& name) const
{
	glBindAttribLocation(mProgram, location, name.c_str());
}

void Core::Shader::Link()
{
	if (mbLinked)
		return;

	if (mProgram <= 0)
	{
		PrintLogFunction(__FUNCTION__,"Program has been not compiled\n");
		return;
	}

	glLinkProgram(mProgram);
	GLint status = 0;

	glGetProgramiv(mProgram, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength = 0;

		glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			GLsizei written = 0;
			std::string errorMessage(infoLogLength, ' ');

			glGetProgramInfoLog(mProgram, infoLogLength, &written, &errorMessage[0]);
			PrintLogFunction(__FUNCTION__,"Program Link Failed %s\n", errorMessage.c_str());
		}
	}

	GetUniformLocations();
	PrintActiveAttribs();
	mbLinked = true;

}

void Core::Shader::GetUniformLocations()
{
	uniformLocations.clear();

	GLint numUniforms = 0;
	glGetProgramInterfaceiv(mProgram, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };
#ifdef _DEBUG	
	PrintLogFunction(__FUNCTION__,"\n\t\t\t\t\t\t\t********* Active Uniforms for program: %d *********\n", mProgram);
#endif
	for (GLint i = 0; i < numUniforms; i++)
	{
		GLint results[4];
		glGetProgramResourceiv(mProgram, GL_UNIFORM, i, 4, properties, 4, nullptr, results);

		if (results[3] != -1) continue;

		GLint nameBuffSize = results[0] + 1;
		std::string name(nameBuffSize, ' ');
		glGetProgramResourceName(mProgram, GL_UNIFORM, i, nameBuffSize, nullptr, &name[0]);
		uniformLocations[name] = results[2];
#ifdef _DEBUG	
		PrintLogFunction(__FUNCTION__,"Location: %-2d %s (%s)\n", results[2], name.c_str(), getTypeString(results[1]).c_str());
#endif 	

	}
}

void Core::Shader::PrintActiveAttribs() const
{
	GLint numAttribs = 0;
	glGetProgramInterfaceiv(mProgram, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };
#ifdef _DEBUG	
	PrintLogFunction(__FUNCTION__,"\n\t\t\t\t\t\t\t********* Active attributes for program: %d *********\n", mProgram);
#endif	
	for (int i = 0; i < numAttribs; ++i) {
		GLint results[3];
		glGetProgramResourceiv(mProgram, GL_PROGRAM_INPUT, i, 3, properties, 3, nullptr, results);

		GLint nameBufSize = results[0] + 1;
		std::string name(nameBufSize, ' ');
		glGetProgramResourceName(mProgram, GL_PROGRAM_INPUT, i, nameBufSize, nullptr, &name[0]);
#ifdef _DEBUG	
		PrintLogFunction(__FUNCTION__,"Location: %-2d %s (%s)\n", results[2], name.c_str(), getTypeString(results[1]).c_str());
#endif	
	}
}

void Core::Shader::Use() const noexcept
{

	if (mProgram <= 0 || (!mbLinked))
	{
		PrintLogFunction(__FUNCTION__,"Shader has been not linked\n");
	}
	glUseProgram(mProgram);
}

void Core::Shader::SetUniform(const std::string& name, GLfloat value)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform1f(loc, value);
}

void Core::Shader::SetUniform(const std::string& name, GLint value)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform1i(loc, value);
}

void Core::Shader::SetUniform(const std::string& name, GLuint value)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform1ui(loc, value);
}

void Core::Shader::SetUniform(const std::string& name, const vmath::vec2& v)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform2fv(loc, 1, v);
}

void Core::Shader::SetUniform(const std::string& name, const vmath::vec3& v)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform3fv(loc, 1, v);
}

void Core::Shader::SetUniform(const std::string& name, const vmath::vec4& v)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform4fv(loc, 1, v);
}

void Core::Shader::SetUniform(const std::string& name, const vmath::mat4& m)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniformMatrix4fv(loc, 1, false, m);
}

void Core::Shader::SetUniform(const std::string& name, const vmath::mat2& m)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniformMatrix2fv(loc, 1, false, m);
}

void Core::Shader::SetSampler2D(const std::string& name, GLuint texture, GLint id)
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, texture);
	SetUniform(name, id);
}

void Core::Shader::SetSampler3D(const std::string& name, GLuint texture, GLint id)
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_3D, texture);
	SetUniform(name, id);
}

void Core::Shader::SetSamplerCubeMap(const std::string& name, GLuint texture, GLint id)
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	SetUniform(name, id);
}

GLuint Core::Shader::GetShaderProgram()
{
	return mProgram;
}

void Core::Shader::DeleteShader()
{
	glUseProgram(mProgram);

	GLint numAttachedShaders = 0;
	glGetProgramiv(mProgram, GL_ATTACHED_SHADERS, &numAttachedShaders);

	std::vector<GLuint> shaderNames(numAttachedShaders);

	glGetAttachedShaders(mProgram, numAttachedShaders, &numAttachedShaders, shaderNames.data());
	for (GLuint shader : shaderNames)
	{
		glDetachShader(mProgram, shader);
		glDeleteShader(shader);
	}

	glUseProgram(0);
	glDeleteProgram(mProgram);
	
	PrintLogFunction(__FUNCTION__, "Shader program object deleted. \n");
	mProgram = 0;
	mbLinked = false;
}

std::string Core::Shader::getTypeString(GLenum type) const
{
	switch (type) 
	{
	case GL_INT_VEC2: return "ivec2";
	case GL_INT_VEC3: return "ivec3";
	case GL_INT_VEC4: return "ivec4";
	case GL_FLOAT: return "float";
	case GL_FLOAT_VEC2: return "vec2";
	case GL_FLOAT_VEC3: return "vec3";
	case GL_FLOAT_VEC4: return "vec4";
	case GL_DOUBLE: return "double";
	case GL_INT: return "int";
	case GL_UNSIGNED_INT: return "unsigned int";
	case GL_BOOL: return "bool";
	case GL_FLOAT_MAT2: return "mat2";
	case GL_FLOAT_MAT3: return "mat3";
	case GL_FLOAT_MAT4: return "mat4";
	case GL_SAMPLER_2D: return "sampler2D";
	case GL_SAMPLER_3D: return "sampler3D";
	case GL_SAMPLER_CUBE: return "samplerCube";
	default: return "?";
	}
}

