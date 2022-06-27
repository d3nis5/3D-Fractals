/**
 * PGPa, GMU - Visualization of 3D fractals
 * VUT FIT, 2020/2021
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Main.h
 *
 */

#pragma once

#ifndef MAIN_H
#define MAIN_H

#include "Camera.h"
#include "Renderer.h"
#include "ShaderManager.h"

const GLfloat mouseSensitivity = 0.1f;
static bool mouseDown = false;

/**
 * @brief Processes keyboard input
 * @param window Window for which to process input
 * @param camera Camera that is being rendered to window
 * @param deltaTime Time that passed between last and actual rendered frame
 */
void processInput(GLFWwindow* window, Camera* camera, GLfloat deltaTime);


/**
 * @brief Mouse callback function for processing mouse input
 */
void mouseCallback(GLFWwindow* window, GLdouble xpos, GLdouble ypos);

/**
 * @brief Mouse callback function for processing mouse input
 */
void mouseButtonCallback(GLFWwindow* window, GLdouble xpos, GLdouble ypos);

/**
 * @brief Mouse button callback 
 */
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

/**
 * @brief Responds to mouse movement by rotating camera
 * @param xOffset Amount of mouse movement change in x axis
 * @param yOffset Amount of mouse movement change in y axis
 */
void processMouse(GLfloat xOffset, GLfloat yOffset);


#endif // !MAIN_H
