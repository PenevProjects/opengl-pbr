#include "Shader.h"
#include "Entity.h"
#include "Camera.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};


	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};


	// shader Program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertex);
	glAttachShader(shaderProgram, fragment);
	//Karsten's way of setting attribute locations: set layout locations for attributes (just before link program)
	//glBindAttribLocation(shaderProgram, 0, "in_Position");
	//glBindAttribLocation(shaderProgram, 1, "in_TexCoord");
	//glBindAttribLocation(shaderProgram, 2, "in_Color");
	glLinkProgram(shaderProgram);
	// print linking errors if any
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// delete the shaders as they're linked into our program now and no longer necessary
	glDetachShader(shaderProgram, vertex);
	glDeleteShader(vertex);
	glDetachShader(shaderProgram, fragment);
	glDeleteShader(fragment);
}
void Shader::Use()
{
	glUseProgram(shaderProgram);
}
void Shader::StopUsing()
{
	glUseProgram(0);
}
void Shader::setViewMatrix(const Camera& _cam, GLboolean perspective3D)
{
	setMat4("u_View", _cam.generateViewMatrix());
	if (perspective3D)
		setMat4("u_Projection", _cam.generateProjMatrixPersp());
	else
		setMat4("u_Projection", _cam.generateProjMatrixOrtho());
}

void Shader::RenderObject(const Entity& _toRender)
{
	setMat4("u_Model", _toRender.modelMatrix);
	glBindVertexArray(_toRender.m_vao);
	glDrawArrays(GL_TRIANGLES, 0, _toRender.m_numVerts);
}
