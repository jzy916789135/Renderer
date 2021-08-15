#include "test_shader.h"
Shader::Shader()
{
	program = glCreateProgram();
}

unsigned int  Shader::CreateVS(const char* vertexpath)
{
	std::string vertexCode;
	std::ifstream vShaderFile;
	//Sets the status mask of the current stream 
	//When the state of the flow changes to the currently set state, an exception is thrown
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertexpath);
		std::stringstream vShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		vShaderFile.close();
		vertexCode = vShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	unsigned int vertex;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");

	shaders.push_back(vertex);

	return vertex;
}

unsigned int Shader::CreateFS(const char* fragmentpath)
{
	std::string fragmentCode;
	std::ifstream fShaderFile;
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		fShaderFile.open(fragmentpath);
		std::stringstream fShaderStream;
		fShaderStream << fShaderFile.rdbuf();
		fShaderFile.close();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* fShaderCode = fragmentCode.c_str();
	unsigned int fragment;
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");

	shaders.push_back(fragment);

	return fragment;
}


unsigned int Shader::CreateGS(const char* geometrypath)
{
	std::string geometryCode;
	std::ifstream gShaderFile;
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		gShaderFile.open(geometrypath);
		std::stringstream gShaderStream;
		gShaderStream << gShaderFile.rdbuf();
		gShaderFile.close();
		geometryCode = gShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* gShaderCode = geometryCode.c_str();
	unsigned int geometry;
	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &gShaderCode, NULL);
	glCompileShader(geometry);
	CheckCompileErrors(geometry, "GEOMOTER");

	shaders.push_back(geometry);

	return geometry;
}
void Shader::CheckCompileErrors(GLuint shade_or_program, std::string type)
{
	GLint sucess;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shade_or_program, GL_COMPILE_STATUS, &sucess);
		if (!sucess)
		{
			glGetShaderInfoLog(shade_or_program, 1024, NULL, infoLog);
			std::cout<<"ERROR::SHADER_COMPILATION_ERROR of type"<< type<<"\n"<< infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shade_or_program, GL_LINK_STATUS, &sucess);
		if (!sucess)
		{
			glGetProgramInfoLog(shade_or_program, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;

		}
	}
}

void Shader::Finalize()
{
	glDeleteProgram(program);
}