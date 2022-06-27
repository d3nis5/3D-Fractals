/**
 * PGPa, GMU - Visualization of 3D fractals
 * VUT FIT, 2020/2021
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Camera.h
 *
 */

#include "Camera.h"

Camera::Camera(glm::vec3 initPosition, glm::vec3 lookAt, GLfloat vFov)
{
	position = initPosition;
	frontVector = glm::normalize(lookAt - position);//glm::vec3(0.0f, 0.0f, -1.0f);
	rightVector = glm::normalize(glm::cross(frontVector, worldUp));
	upVector = glm::normalize(glm::cross(rightVector, frontVector));

	yaw = glm::degrees(atan2(frontVector.x, frontVector.z));
	pitch = glm::degrees(asin(frontVector.y));
	cameraChanged = true;

	this->vFov = tan(glm::radians(vFov) /2.0f);
}

void Camera::updatePosition(Direction direction, float deltaTime)
{
	GLfloat cameraSpeed = speed * deltaTime;

	switch (direction)
	{
	case forward:
		position += frontVector * cameraSpeed;
		break;
	case backward:
		position -= frontVector * cameraSpeed;
		break;
	case left:
		position -= rightVector * cameraSpeed;
		break;
	case right:
		position += rightVector * cameraSpeed;
		break;
	case up:
		position += upVector * cameraSpeed;
		break;
	case down:
		position -= upVector * cameraSpeed;
		break;
	}

	cameraChanged = true;
}

void Camera::rotate(GLfloat xOffset, GLfloat yOffset)
{
	// new camera angles
	yaw += xOffset;
	pitch += yOffset;

	const GLfloat pitchLimit = 89.0f;

	// constraint camera angles
	if (pitch > pitchLimit)
		pitch = pitchLimit;

	if (pitch < -pitchLimit)
		pitch = -pitchLimit;

	updateVectors();
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::inverse(glm::lookAt(position, position + frontVector, upVector));
}

void Camera::setView(int id)
{
	const glm::vec3 position1 = glm::vec3(0.0f, 2.5f, 5.0f);
	const glm::vec3 position2 = glm::vec3(-2.66311f, 0.435458f, 2.02129f);
	const glm::vec3 position3 = glm::vec3(-1.12841f, 0.292396f, 0.930504f);
	const glm::vec3 position4 = glm::vec3(0.0534707f, -1.12837f, 1.69462f);
	const glm::vec3 position[] = { position1, position2, position3, position4 };
	const float pitch[] = { -26.5651f, 0.0347265f, 23.8349f, 35.435f };
	const float yaw[] = { 180.0f, 127.399f, 129.3f, 198.4f };

	if ((id < 0) || (id > 3)) id = 0;

	this->position = position[id];
	this->pitch = pitch[id];
	this->yaw = yaw[id];
	updateVectors();
	cameraChanged = true;
}

void Camera::updateVectors()
{
	glm::vec3 front;

	// compute new camera direction vectors
	front.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(yaw)) * cos(glm::radians(pitch));

	this->frontVector = glm::normalize(front);
	this->rightVector = glm::normalize(glm::cross(frontVector, worldUp));
	this->upVector = glm::normalize(glm::cross(rightVector, frontVector));
}
