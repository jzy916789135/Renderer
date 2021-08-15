#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
class Shader
{
public:
	Shader();

	unsigned int CreateVS(const char* vertexpath);
	unsigned int CreateFS(const char* fragmentpath);
	unsigned int CreateGS(const char* geometrypath);
	//attach shader
	inline void Attach()
	{
		for (auto shader : shaders)
		{
			glAttachShader(program, shader);
		}
	}

	// link shader to program
	inline void Link()
	{
		glLinkProgram(program);
		CheckCompileErrors(program, "PROGRAM");
		for (auto shader : shaders)
			glDeleteShader(shader);
		shaders.clear();
	}
	inline void use()
	{
		glUseProgram(program);
	}
	inline void SetBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
	}
	inline void SetInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(program, name.c_str()), value);
	}
	inline void SetFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(program, name.c_str()), value);
	}
	inline void SetVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
	}
	inline void SetVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(program, name.c_str()), x, y);
	}

	inline void SetVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
	}

	inline void SetVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z);
	}

	inline void SetVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
	}

	inline void SetVec4(const std::string& name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w);
	}

	inline void SetMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	inline void SetMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	inline void SetMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	//free all the reource about shader
	void Finalize();

private:
	void CheckCompileErrors(GLuint shade_or_program,std::string type);

private:
	//program ID
	unsigned int program;
	std::vector<unsigned int> shaders;
};

#endif