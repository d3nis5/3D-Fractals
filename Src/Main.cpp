/**
 * PGPa, GMU - Visualization of 3D fractals
 * VUT FIT, 2020/2021
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Main.cpp
 *
 */

#include "Main.h"

Camera mainCamera = Camera(glm::vec3(0.0f, 2.5f, 5.0f), glm::vec3(0.0f, -0.5f, -1.0f), 45.0f);

Renderer* renderer;

int main()
{
	try
	{
		renderer = new Renderer();
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
		return -1;
	}

	if (renderer->initialize() == false)
	{
		delete renderer;
		return -1;
	}
	renderer->setMainCamera(&mainCamera);

	// set callback for mouse movement
	glfwSetCursorPosCallback(renderer->window, mouseCallback);

	// set callback for mouse button
	glfwSetMouseButtonCallback(renderer->window, mouseButtonCallback);


	glfwSetTime(0.0);
	GLfloat lastTime = (GLfloat)glfwGetTime();
	GLfloat lastFPS = 0.0;

	// string showing fps
	GLchar fps[64];

	while (!glfwWindowShouldClose(renderer->window))
	{
		GLfloat currentTime = (GLfloat)glfwGetTime();
		GLfloat frameTime = currentTime - lastTime;

		lastTime = currentTime;

		if (currentTime - lastFPS >= 1.0f)
		{
			snprintf(fps, 64, "Visualization of 3D fractals %f FPS", 1.0f / frameTime);

			glfwSetWindowTitle(renderer->window, fps);
			lastFPS = currentTime;
		}
		processInput(renderer->window, &mainCamera, frameTime);
		
		// draw frame
		renderer->draw();
			
		glfwSwapBuffers(renderer->window);
		
		
		glfwPollEvents();
	}

	delete renderer;
	return 0;
}

void processInput(GLFWwindow* window, Camera* camera, GLfloat deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		(*camera).updatePosition(forward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		(*camera).updatePosition(backward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		(*camera).updatePosition(left, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		(*camera).updatePosition(right, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		(*camera).updatePosition(up, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		(*camera).updatePosition(down, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		renderer->setGUIvisibility();
}

void mouseCallback(GLFWwindow* window, GLdouble xpos, GLdouble ypos)
{
	// flag if this function is called for the first time
	static bool first = true;
	// last position of the mouse on x-axis
	static GLfloat lastX = (GLfloat)(renderer->resolution.x) / 2.0f;
	// last position of the mouse on y-axis
	static GLfloat lastY = (GLfloat)(renderer->resolution.y) / 2.0f;

	xpos = -xpos;

	// this ensures that camera doesn't change direction suddenly, when the function is called for the first time
	if (first)
	{
		lastX = (GLfloat)xpos;
		lastY = (GLfloat)ypos;
		first = false;
	}

	GLfloat xOffset = 0.0f;
	GLfloat yOffset = 0.0f;

	if (mouseDown)
	{
		// difference in the mouse position
		xOffset = (GLfloat)xpos - lastX;
		yOffset = lastY - (GLfloat)ypos;
		mainCamera.cameraChanged = true;
	}

	lastX = (GLfloat)xpos;
	lastY = (GLfloat)ypos;

	processMouse(xOffset, yOffset);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		mouseDown = true;

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		mouseDown = false;
}

void processMouse(GLfloat xOffset, GLfloat yOffset)
{
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;
	
	mainCamera.rotate(xOffset, yOffset);
}