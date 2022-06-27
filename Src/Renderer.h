/**
 * PGPa, GMU - Visualization of 3D fractals
 * VUT FIT, 2020/2021
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Renderer.h
 *
 */

#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include "ShaderManager.h"

#include "helpers/RootDir.h"

//#define CPU_RAYMARCH
#include "Raymarcher.h"

const glm::ivec2 tileDimensions = glm::ivec2(16, 16);

// max and min parameters
const float renderingDetailMin = 2.0f;
const float renderingDetailMax = 6.0f;
const float detailPowerMin = 1.0f;
const float detailPowerMax = 4.0f;
const int maxStepsMin = 1;
const int maxStepsMax = 1024;
const float shadowSoftnessMin = 1.0f;
const float shadowSoftnessMax = 256.0f;
const float fractalPowerMin = 1.0f;
const float fractalPowerMax = 16.0f;
const int fractalIterationsMin = 1;
const int fractalIterationsMax = 32;
const float xAngleMax = 360.0f;
const float xAngleMin = 0.0f;
const float yAngleMax = 90.0f;
const float yAngleMin = -90.0f;


class Renderer
{
public:
	// main window
	GLFWwindow* window;
	// window resolution
	glm::ivec2 resolution;

	Renderer();

	~Renderer();

	/**
	 * @brief Sets given camera as main camera of the screen
	 * @param camera Camera to be set as main camera
	 */
	void setMainCamera(Camera* camera);

	/**
	 * @brief Draws image created by ray tracing to the screen
	 */
	void draw();

	/**
	 * @brief Initializes vertex, fragment and compute shader, sets initial values of uniforms
	 * @return TRUE if initialization succeeded, else FALSE
	 */
	bool initialize();

	void setGUIvisibility();
private:
	// reference to main camera
	Camera* mainCamera;

	// Object managing shaders
	ShaderManager shaderManager;

	// shader program for drawing
	GLuint quadProgram;

	// shader program for compute shader
	GLuint computeProgram;

	// VAO for quadProgram
	GLuint vao;

	// quad texture that is rendered to screen
	GLuint frameBuffer;

	// texture in which color values from all subframes are accumulated
	GLuint accumulationBuffer;

	// indicates whether values in GUI were changed and fractal needs to be re-rendered
	bool GUIchanged;

	// indicates whether GUI is to be rendered
	bool showGUI;

	// indicates whether window is in fullscreen mode
	bool fullscreen;

	// stores window position when switching to fullscreen
	glm::ivec2 windowPos;

	struct UniformLocations
	{
		GLuint origin;
		GLuint viewMatrix;
		GLuint vFov;
		GLuint fractalPower;
		GLuint fractalIter;
		GLuint maxSteps;
		GLuint detail;
		GLuint detailPower;
		GLuint shadows;
		GLuint shadowSoftness;
		GLuint bgColor;
		GLuint fractalColor;
		GLuint oTrapColor;
		GLuint yTrapColor;
		GLuint subframeOffset;
		GLuint subframeID;
		GLuint light;
	} uniformLocations;

	Fractal fractal;

	Rendering rendering;

	struct Coloring
	{
		float bgColor[3];
		float fractalColor[3];
		float oTrapColor[3];
		float yTrapColor[3];
	} coloring;

	/**
	 * @brief Creates window and OpenGL context
	 * @return Created window
	 */
	GLFWwindow* createWindowAndGLContext();

	/**
	 * @brief Draws ImGui GUI  
	 */
	void renderGUI();

	/**
	 * @brief Helper to display a little (?) mark which shows a tooltip when hovered. 
	 */
	void HelpMarker(const char* desc);

	/**
	 * @brief Gets locations of all uniforms and stores them in uniformLocations structure
	 */
	void getUniformLocations();

	/**
	 * @brief Changes resolution of a window to the value that is stored in resolution class field 
	 */
	void changeResolution();

	void setFullscreen();

	inline void guiChanged();
};

#endif // !RENDERER_H