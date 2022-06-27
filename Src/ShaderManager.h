/**
 * PGPa, GMU - Visualization of 3D fractals
 * VUT FIT, 2020/2021
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	ShaderManager.h
 *
 */

#pragma once

#ifndef SHADER_H
#define SHADER_H

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

class ShaderManager
{
public:
	ShaderManager();

	/**
	 * @brief Creates shader program with vertex and fragment shader
	 * @param vertexFile Path to vertex shader source file
	 * @param fragmentFile Path to fragment shader source file
	 * @return ID of created shader program or 0 if something went wrong
	 */
	GLuint createQuadProgram(fs::path vertexFile, fs::path fragmentFile);

	/**
	 * @brief Creates VAO for quad program
	 * @return ID of created VAO
	 */
	GLuint createQuadVAO();

	/**
	 * @brief Creates texture for writing
	 * @return ID of created texture
	 */
	GLuint createTexture(GLint textureWidth, GLint textureHeight, GLint location, GLenum access);

	/**
	 * @brief Creates copmpute shader program
	 * @param computeFile Path to compute shader source file
	 * @return ID of created shader program or 0 if something went wrong
	 */
	GLuint createComputeProgram(fs::path computeFile);

	/**
	 * @brief Sets uniform of a given location
	 * @param uniformLocation Location of a uniform
	 * @param value Value to be set
	 */
	void setUniformVec2(GLuint uniformLocation, glm::vec2 value);

	void setUniformVec3(GLuint uniformLocation, glm::vec3 value);
															   
	void setUniformMat4(GLuint uniformLocation, glm::mat4 value);

	void setUniformInt(GLuint uniformLocation, GLint value);

	void setUniformFloat(GLuint uniformLocation, GLfloat value);

private:
	/**
	 * @brief Loads shader from file and compiles it
	 * @param shaderFile Path to shader source file
	 * @param shaderType Type of the shader
	 * @return ID of created shader or 0 if something went wrong
	 */
	GLuint createShader(fs::path shaderFile, GLenum shaderType);

	/**
	 * @brief Converts shader type to string
	 * @return Name of shader type
	 */
	std::string shaderTypeToString(GLenum shaderType);

	/**
	 * @brief Checks if there were any errors during shader compilation
	 * @param shader	ShaderManager whose compilation errors to check
	 * @param type		Type of the shader whose compilation to check
	 */
	void checkShaderCompilation(GLuint shader, std::string type);

	/**
	 * @brief Checks if there were any errors during shader program linking
	 * @param program	ShaderManager program whose linking errors to check
	 */
	void checkProgramLinking(GLuint program);
};

#endif // !SHADER_H
