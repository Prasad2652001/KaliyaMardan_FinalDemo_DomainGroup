#pragma once

#include "./Common.h"

namespace Core
{	
	class Shader
	{	
	public:
		enum class Type
		{
			VERTEX = GL_VERTEX_SHADER,
			FRAGMENT = GL_FRAGMENT_SHADER,
			GEOMETRY = GL_GEOMETRY_SHADER,
			TESS_CONTROL = GL_TESS_CONTROL_SHADER,
			TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
			COMPUTE = GL_COMPUTE_SHADER
		};

		Shader();
		~Shader();

		void CompileShader(const std::string&, Type);
		void BindAttribLocation(GLuint, const std::string&) const;
		void Link();
		void Use() const noexcept;
		
		void SetUniform(const std::string&, GLfloat);
		void SetUniform(const std::string&, GLint);
		void SetUniform(const std::string&, GLuint);
		void SetUniform(const std::string&, const vmath::vec2&);
		void SetUniform(const std::string&, const vmath::vec3&);
		void SetUniform(const std::string&, const vmath::vec4&);
		void SetUniform(const std::string&, const vmath::mat4&);
		void SetUniform(const std::string&, const vmath::mat2&);

		void SetSampler2D(const std::string&, GLuint, GLint);
		void SetSampler3D(const std::string&, GLuint, GLint);
		void SetSamplerCubeMap(const std::string&, GLuint, GLint);

		GLuint GetShaderProgram();
		void DeleteShader();
        float exposure = 1;
		
	private:
		GLuint mProgram;
		bool mbLinked;
		std::unordered_map<std::string, int> uniformLocations;
		
		void GetUniformLocations();
		void PrintActiveAttribs() const;

		GLint getUniformLocation(const std::string&);
		std::string getTypeString(GLenum) const;
	};
	
	inline GLint Core::Shader::getUniformLocation(const std::string& name)
	{
		if (uniformLocations.find(name) != uniformLocations.end())
		{
			return uniformLocations[name];
		}

		GLint location = glGetUniformLocation(mProgram, name.c_str());
		uniformLocations[name] = location;
		return location;
	}
}
