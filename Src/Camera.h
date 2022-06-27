/**
 * PGPa, GMU - Visualization of 3D fractals
 * VUT FIT, 2020/2021
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Camera.h
 *
 */

#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <iostream>
#include <cmath>	// atan

 // vector pointing up in world coordinates
const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

// speed of camera
const GLfloat speed = 2.0f;

// movement direction
enum Direction {forward, backward, left, right, up, down};

class Camera
{
public:
	/**
	 * @brief Camera constructor
	 * @param initPosition Initial position of the camera in the world
	 * @param lookAt Direction vector of the camera
	 * @param vFov Vertical FOV - top to bottom of a viewing frustum in degrees
	 */
	Camera(glm::vec3 initPosition, glm::vec3 lookAt, GLfloat vFov);

	/**
	 * @brief Updates camera position based on user input
	 * @param direction	Direction of the movement
	 * @param deltaTime	Time difference between last frame and current frame
	 */
	void updatePosition(Direction direction, float deltaTime);

	/**
	 * @brief Rotates camera
	 * @param xOffset How much to rotate on x axis
	 * @param xOffset How much to rotate on y axis
	 */
	void rotate(GLfloat xOffset, GLfloat yOffset);

	glm::mat4 getViewMatrix();

	/**
	 * @brief Sets view of the camera
	 * @param id Id of the view to set
	 */
	void setView(int id);

	// position of the camera in world coordinates
	glm::vec3 position;
	// vector pointing forward from camera
	glm::vec3 frontVector;
	// vector pointing to the right from camera
	glm::vec3 rightVector;
	// vector pointing up from camera
	glm::vec3 upVector;
	// vertical FOV of camera
	GLfloat vFov;

	// camera angles
	GLfloat yaw, pitch;

	// indicates whether camera position or rotation changed 
	bool cameraChanged;

private:
	/**
	 * @brief Updates camera direction vectors
	 */
	void updateVectors();
};

#endif

