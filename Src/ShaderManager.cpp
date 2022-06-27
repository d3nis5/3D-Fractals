/**
 * PGPa, GMU - Visualization of 3D fractals
 * VUT FIT, 2020/2021
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	ShaderManager.cpp
 *
 */

#include "ShaderManager.h"

ShaderManager::ShaderManager() {}

GLuint ShaderManager::createShader(fs::path shaderFile, GLenum shaderType)
{
	std::string shaderCode;
	std::ifstream file;

	// fstream object can throw exceptions
	file.exceptions(std::ifstream::badbit | std::ifstream::failbit);

	try
	{
		// open files
		file.open(shaderFile);

		std::stringstream stream;
		// read shader file into stream
		stream << file.rdbuf();
		file.close();

		// convert string stream into string
		shaderCode = stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Failed to read shader file!" << std::endl;
		return 0;
	}

	const GLchar* code = shaderCode.c_str();

	// create and compile shader
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);
	checkShaderCompilation(shader, shaderTypeToString(shaderType));

	return shader;
}

std::string ShaderManager::shaderTypeToString(GLenum shaderType)
{
	switch (shaderType)
	{
	case GL_VERTEX_SHADER:
		return "Vertex";
		break;
	case GL_FRAGMENT_SHADER:
		return "Fragment";
		break;
	case GL_COMPUTE_SHADER:
		return "Compute";
		break;
	default:
		return "Unknown";
		break;
	}
}

GLuint ShaderManager::createQuadProgram(fs::path vertexFile, fs::path fragmentFile)
{
	GLuint vertexShader = createShader(vertexFile, GL_VERTEX_SHADER);
	GLuint fragmentShader = createShader(fragmentFile, GL_FRAGMENT_SHADER);

	if ((vertexShader == 0) || (fragmentShader == 0))
		return 0;

	// create and link shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	checkProgramLinking(shaderProgram);

	// shader program is created, shaders are no longer needed
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

GLuint ShaderManager::createQuadVAO()
{
	// vertex information: position x, position y, texture x, texture y
	GLfloat quadVertices[] = { -1.0f, -1.0f, 0.0f, 0.0f, 
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 
		1.0f, 1.0f, 1.0f, 1.0f
	};

	GLuint vbo = 0, vao = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create buffer and fill it with vertex data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	GLintptr stride = 4 * sizeof(GLfloat);

	// vertex position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, NULL);
	glEnableVertexAttribArray(0);

	GLintptr offset = 2 * sizeof(float);

	// texture position attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
	glEnableVertexAttribArray(1);

	return vao;
}

GLuint ShaderManager::createTexture(GLint textureWidth, GLint textureHeight, GLint unit, GLenum access)
{
	GLuint texture = 0;

	// create texture
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0+unit);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set texture wrapping/filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// same internal format as in compute shader
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	// bind to image unit for writing
	glBindImageTexture(unit, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	return texture;
}


GLuint ShaderManager::createComputeProgram(fs::path computeFile)
{
	GLuint computeShader = createShader(computeFile, GL_COMPUTE_SHADER);

	if (computeShader == 0)
		return 0;

	// create and link shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, computeShader);
	glLinkProgram(shaderProgram);
	checkProgramLinking(shaderProgram);

	// shader program is created, shader is no longer needed
	glDeleteShader(computeShader);

	return shaderProgram;
}

void ShaderManager::setUniformVec2(GLuint uniformLocation, glm::vec2 value)
{
	glUniform2fv(uniformLocation, 1, glm::value_ptr(value));
}

void ShaderManager::setUniformVec3(GLuint uniformLocation, glm::vec3 value)
{
	glUniform3fv(uniformLocation, 1, glm::value_ptr(value));
}


void ShaderManager::setUniformMat4(GLuint uniformLocation, glm::mat4 value)
{
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderManager::setUniformInt(GLuint uniformLocation, GLint value)
{
	glUniform1i(uniformLocation, value);
}

void ShaderManager::setUniformFloat(GLuint uniformLocation, GLfloat value)
{
	glUniform1f(uniformLocation, value);
}

void ShaderManager::checkShaderCompilation(GLuint shader, std::string type)
{
	char infoLog[512];
	int success = 0;

	// check for compilation errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		// compilation failed, get info log
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << type << " shader compilation failed!\n" << infoLog << std::endl;
	}
}

void ShaderManager::checkProgramLinking(GLuint program)
{
	char infoLog[512];
	int success = 0;

	// check for linking errors
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		// linking failed, get info log
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "Shader program linking failed!\n" << infoLog << std::endl;
	}
}