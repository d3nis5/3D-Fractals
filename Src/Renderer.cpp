/**
 * PGPa, GMU - Visualization of 3D fractals
 * VUT FIT, 2020/2021
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Renderer.cpp
 *
 */

#include "Renderer.h"


Renderer::Renderer()
{
	showGUI = true;
	resolution = glm::ivec2(1280, 720);
	window = createWindowAndGLContext();

	if (window == NULL)
		throw std::runtime_error("Failed to create window.");

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD.");
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	GUIchanged = false;
	fullscreen = false;

	fractal.power = 8.0;
	fractal.iterations = 6;
	rendering.maxSteps = 80;
	rendering.detail = 4;
	rendering.detailPower = 1.5;
	rendering.shadows = false;
	rendering.shadowSoftness = 16.0;
	rendering.antialiasing = 1;
	rendering.lightPosition = glm::normalize(glm::vec3(0.0, 1.4, 1.7));

	coloring.bgColor[0] = 0.53f; coloring.bgColor[1] = 0.8f; coloring.bgColor[2] = 0.8f;
	coloring.fractalColor[0] = 0.334f; coloring.fractalColor[1] = 0.42f; coloring.fractalColor[2] = 0.184f;
	coloring.oTrapColor[0] = 0.741f; coloring.oTrapColor[1] = 0.718f; coloring.oTrapColor[2] = 0.42f;
	coloring.yTrapColor[0] = 0.58f; coloring.yTrapColor[1] = 0.313f; coloring.yTrapColor[2] = 0.0f;

	glViewport(0, 0, resolution.x, resolution.y);
}

Renderer::~Renderer()
{
	// cleanup imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void Renderer::setMainCamera(Camera* camera)
{
	mainCamera = camera;
}

bool Renderer::initialize()
{
	// root directory path of the program
	fs::path rootDir = fs::u8path(ROOT_DIR);

	// vertex shader path
	fs::path vsPath = rootDir;
	vsPath += fs::path("Shaders/vertexShader.vert");

	// fragment shader path
	fs::path qsPath = rootDir;
	qsPath += fs::path("Shaders/quadShader.frag");

	// compute shader path
	fs::path csPath = rootDir;
	csPath += fs::path("Shaders/compShader.comp");

	shaderManager = ShaderManager();

	quadProgram = shaderManager.createQuadProgram(vsPath, qsPath);
	if (quadProgram == 0)
	{
		std::cout << "Failed to create quad shader program" << std::endl;
		return false;
	}

	computeProgram = shaderManager.createComputeProgram(csPath);
	if (computeProgram == 0)
	{
		std::cout << "Failed to create compute shader program" << std::endl;
		return false;
	}

	getUniformLocations();

	// set rendering uniforms
	shaderManager.setUniformFloat(uniformLocations.detail, 1.0f / powf(10, rendering.detail));
	shaderManager.setUniformFloat(uniformLocations.detailPower, rendering.detailPower);
	shaderManager.setUniformInt(uniformLocations.maxSteps, rendering.maxSteps);
	shaderManager.setUniformInt(uniformLocations.shadows, rendering.shadows);
	shaderManager.setUniformFloat(uniformLocations.shadowSoftness, rendering.shadowSoftness);
	shaderManager.setUniformVec3(uniformLocations.light, rendering.lightPosition);

	// set fractal uniforms
	shaderManager.setUniformFloat(uniformLocations.fractalPower, fractal.power);
	shaderManager.setUniformInt(uniformLocations.fractalIter, fractal.iterations);

	// set coloring uniforms
	shaderManager.setUniformVec3(uniformLocations.bgColor, 
		glm::vec3(coloring.bgColor[0], coloring.bgColor[1], coloring.bgColor[2]));
	shaderManager.setUniformVec3(uniformLocations.fractalColor,
		glm::vec3(coloring.fractalColor[0], coloring.fractalColor[1], coloring.fractalColor[2]));
	shaderManager.setUniformVec3(uniformLocations.oTrapColor,
		glm::vec3(coloring.oTrapColor[0], coloring.oTrapColor[1], coloring.oTrapColor[2]));
	shaderManager.setUniformVec3(uniformLocations.yTrapColor,
		glm::vec3(coloring.yTrapColor[0], coloring.yTrapColor[1], coloring.yTrapColor[2]));

	vao = shaderManager.createQuadVAO();
	frameBuffer = shaderManager.createTexture(resolution.x, resolution.y, 0, GL_WRITE_ONLY);
	accumulationBuffer = shaderManager.createTexture(resolution.x, resolution.y, 1, GL_READ_WRITE);

	//shaderManager.printWorkGroupLimits();

	return true;
}

GLFWwindow* Renderer::createWindowAndGLContext()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(resolution.x, resolution.y, "Visualization of 3D fractals", NULL, NULL);
	if (window == NULL)
	{
		return NULL;
	}
	glfwMakeContextCurrent(window);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(0);		// VSync: 0 - disabled, 1 - enabled

	return window;
}

void Renderer::draw()
{
#ifndef CPU_RAYMARCH
	/*GLuint queryTime;
	glGenQueries(1, &queryTime);
	GLuint queryTimePassed = 0;*/

	int AA = rendering.antialiasing;
	static int AAsampleX = 0;
	static int AAsampleY = 0;
	bool render = false;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// start temporal AA from subframe 0
	if (mainCamera->cameraChanged || GUIchanged)
	{
		AAsampleX = 0;
		AAsampleY = 0;
		render = true;

		// set camera uniforms
		shaderManager.setUniformVec3(uniformLocations.origin, mainCamera->position);
		shaderManager.setUniformMat4(uniformLocations.viewMatrix, mainCamera->getViewMatrix());
		shaderManager.setUniformFloat(uniformLocations.vFov, mainCamera->vFov);
	}

	if (render || ((AAsampleX < AA) && (AAsampleY < AA)))
	{
		glm::vec2 subframeOffset;
		int subframeID = AAsampleX * AA + AAsampleY;

		if (AA == 1)
		{
			subframeOffset.x = 0.0;
			subframeOffset.y = 0.0;
		}
		else
		{
			subframeOffset.x = float(AAsampleX) / float(AA);
			subframeOffset.y = float(AAsampleY) / float(AA);
		}

		/*std::cout << "subframeID: " << subframeID << std::endl;
		std::cout << "X: " << AAsampleX << ", offsetX: " << subframeOffset.x << "\tY: " << AAsampleY << ", offsetY: " << subframeOffset.y << std::endl;
		std::cout << "============================\n";*/

		glActiveTexture(GL_TEXTURE0);
		glUseProgram(computeProgram);
		shaderManager.setUniformVec2(uniformLocations.subframeOffset, subframeOffset);
		shaderManager.setUniformInt(uniformLocations.subframeID, subframeID);

		//glBeginQuery(GL_TIME_ELAPSED, queryTime);

		glm::vec2 workGroupSize = glm::vec2(float(resolution.x) / tileDimensions.x, float(resolution.y) / tileDimensions.y);
		workGroupSize += 0.5f;

		// start compute shader
		// number of work groups is based on resolution and tile dimensions
		glDispatchCompute(GLuint(workGroupSize.x), GLuint(workGroupSize.y), 1);

		// wait for all invocations of compute shader to finish writing to an image
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		/*glEndQuery(GL_TIME_ELAPSED);

		int done = false;
		while (!done) {
			glGetQueryObjectiv(queryTime,
				GL_QUERY_RESULT_AVAILABLE,
				&done);
		}

		glGetQueryObjectuiv(queryTime, GL_QUERY_RESULT, &queryTimePassed);

		std::cout << "Time: " << queryTimePassed << "ns" << std::endl;*/

		AAsampleY++;
		if ((AAsampleY >= AA))
		{
			if (AAsampleX < AA-1)
			{
				AAsampleX++;
				AAsampleY = 0;
			}
		}

		mainCamera->cameraChanged = false;
		GUIchanged = false;
	}

#else
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (mainCamera->cameraChanged || GUIchanged)
	{
		Raymarcher raymarcher(resolution, mainCamera, &fractal, &rendering);
		int size = resolution.x * resolution.y;

		std::vector<glm::vec4> data(size, glm::vec4(0.0, 0.0, 0.0, 1.0));

		std::cout << "CPU rendering\nThis might take a while..." << std::endl;

		double startT = glfwGetTime();

		for (int y = 0; y < resolution.y; y++)
			for (int x = 0; x < resolution.x; x++)
			{
				data[y * resolution.x + x] = glm::vec4(raymarcher.getColor(glm::vec2(x, y)), 1.0f);
			}

		double endT = glfwGetTime();

		glBindTexture(GL_TEXTURE_2D, frameBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, data.data());

		std::cout << "Rendered in " << endT - startT << " seconds" << std::endl;

		mainCamera->cameraChanged = false;
		GUIchanged = false;
	}
#endif // !CPU_RAYMARCH

	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(quadProgram);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	renderGUI();
}

void Renderer::renderGUI()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(425, 500), ImGuiCond_Once);

	glUseProgram(computeProgram);	// because of setting uniforms

	if (showGUI)
	{
		ImGui::Begin("Menu", &showGUI);

		if (ImGui::CollapsingHeader("Help"))
		{
			ImGui::Text("CAMERA CONTROLS:");
			ImGui::BulletText("\'W\' to go forward.");
			ImGui::BulletText("\'S\' to go backward.");
			ImGui::BulletText("\'A\' to go left.");
			ImGui::BulletText("\'D\' to go right.");
			ImGui::BulletText("LShift to go up.");
			ImGui::BulletText("LCtrl to go down.");
			ImGui::BulletText("Right mouse+drag to rotate.");
			ImGui::Text("OTHER CONTROLS:");
			ImGui::BulletText("\'G\' to show/hide GUI.");
		}

		if (ImGui::CollapsingHeader("Rendering"))
		{
			const int viewValues[] = { 0, 1, 2, 3 };
			static int itemCurrentView = 0;
			if (ImGui::Combo("Camera View", &itemCurrentView, " View 1\0 View 2\0 View 3\0 View 4\0\0"))
			{
				mainCamera->setView(viewValues[itemCurrentView]);
				guiChanged();
			}

			const glm::ivec2 ResValues[] = { glm::ivec2(640, 480), glm::ivec2(1280, 720), glm::ivec2(1920, 1080), glm::ivec2(2560, 1440), glm::ivec2(3840, 2160) };
			static int itemCurrentRes = 1;
			if (ImGui::Combo("Resolution", &itemCurrentRes, " 640x480\0 1280x720\0 1920x1080\0 2560x1440\0 3840x2160\0\0"))
			{
				resolution = ResValues[itemCurrentRes];
				changeResolution();
				guiChanged();
			}

			if (ImGui::Checkbox("Fullscreen", &fullscreen))
			{
				setFullscreen();
				guiChanged();
			}

			if (ImGui::SliderFloat("Detail", &(rendering.detail), renderingDetailMin, renderingDetailMax, "%.2f"))
			{
				if (rendering.detail < renderingDetailMin)
					rendering.detail = renderingDetailMin;

				shaderManager.setUniformFloat(uniformLocations.detail, 1.0f / powf(10, rendering.detail));
				guiChanged();
			}

			if (ImGui::SliderFloat("Detail Power", &(rendering.detailPower), detailPowerMin, detailPowerMax, "%.2f"))
			{
				if (rendering.detailPower < detailPowerMin)
					rendering.detailPower = detailPowerMin;

				shaderManager.setUniformFloat(uniformLocations.detailPower, rendering.detailPower);
				guiChanged();
			}
			ImGui::SameLine(); HelpMarker("Controls how detail changes with distance.");

			if (ImGui::SliderInt("Marching Steps", &(rendering.maxSteps), maxStepsMin, maxStepsMax, "%d"))
			{
				if (rendering.maxSteps < maxStepsMin)
					rendering.maxSteps = maxStepsMin;

				shaderManager.setUniformInt(uniformLocations.maxSteps, rendering.maxSteps);
				guiChanged();
			}
			ImGui::SameLine(); HelpMarker("Maximal number of marching steps.");

			const int AAvalues[] = { 1, 2, 4, 8, 16 };
			static int itemCurrentAA = 0;
			if (ImGui::Combo("Antialiasing", &itemCurrentAA, " Off\0 2x\0 4x\0 8x\0 16x\0\0"))
			{
				rendering.antialiasing = AAvalues[itemCurrentAA];
				guiChanged();
			}

			if (ImGui::Checkbox("Shadows", &(rendering.shadows)))
			{
				shaderManager.setUniformInt(uniformLocations.shadows, rendering.shadows);
				guiChanged();
			}

			if (rendering.shadows)
			{
				if (ImGui::SliderFloat(
					"Shadow Softness", &(rendering.shadowSoftness), shadowSoftnessMin, shadowSoftnessMax, "%.2f"))
				{
					if (rendering.shadowSoftness < shadowSoftnessMin)
						rendering.shadowSoftness = shadowSoftnessMin;

					if (rendering.shadowSoftness > shadowSoftnessMax)
						rendering.shadowSoftness = shadowSoftnessMax;

					shaderManager.setUniformFloat(uniformLocations.shadowSoftness, rendering.shadowSoftness);
					guiChanged();
				}
			}
		}

		if (ImGui::CollapsingHeader("Fractal"))
		{
			if (ImGui::SliderFloat("Power", &(fractal.power), fractalPowerMin, fractalPowerMax, "%.2f"))
			{
				if (fractal.power < fractalPowerMin)
					fractal.power = fractalPowerMin;

				if (fractal.power > fractalPowerMax)
					fractal.power = fractalPowerMax;

				shaderManager.setUniformFloat(uniformLocations.fractalPower, fractal.power);
				guiChanged();
			}

			if (ImGui::SliderInt("Iterations", &(fractal.iterations), fractalIterationsMin, fractalIterationsMax, "%d"))
			{
				if (fractal.iterations < fractalIterationsMin)
				{
					fractal.iterations = fractalIterationsMin;
				}

				shaderManager.setUniformInt(uniformLocations.fractalIter, fractal.iterations);
				guiChanged();
			}
		}

		if (ImGui::CollapsingHeader("Coloring"))
		{
			if (ImGui::ColorEdit3("Background", coloring.bgColor))
			{
				shaderManager.setUniformVec3(uniformLocations.bgColor,
					glm::vec3(coloring.bgColor[0], coloring.bgColor[1], coloring.bgColor[2]));
				guiChanged();
			}

			if (ImGui::ColorEdit3("Fractal Color", coloring.fractalColor))
			{
				shaderManager.setUniformVec3(uniformLocations.fractalColor,
					glm::vec3(coloring.fractalColor[0], coloring.fractalColor[1], coloring.fractalColor[2]));
				guiChanged();
			}

			if (ImGui::ColorEdit3("O Trap Color", coloring.oTrapColor))
			{
				shaderManager.setUniformVec3(uniformLocations.oTrapColor,
					glm::vec3(coloring.oTrapColor[0], coloring.oTrapColor[1], coloring.oTrapColor[2]));
				guiChanged();
			}

			if (ImGui::ColorEdit3("Y Trap Color", coloring.yTrapColor))
			{
				shaderManager.setUniformVec3(uniformLocations.yTrapColor,
					glm::vec3(coloring.yTrapColor[0], coloring.yTrapColor[1], coloring.yTrapColor[2]));
				guiChanged();
			}
		}

		if (ImGui::CollapsingHeader("Light"))
		{
			static float xAngle = 0.0f;
			static float yAngle = 45.0f;
			bool lightChanged = false;

			if (ImGui::SliderFloat("X Angle", &xAngle, xAngleMin, xAngleMax, "%.2f"))
			{
				if (xAngle < xAngleMin)
					xAngle = xAngleMin;

				if (xAngle > xAngleMax)
					xAngle = xAngleMax;

				lightChanged = true;
			}

			if (ImGui::SliderFloat("Y Angle", &yAngle, yAngleMin, yAngleMax, "%.2f"))
			{
				if (yAngle < yAngleMin)
					yAngle = yAngleMin;

				if (yAngle > yAngleMax)
					yAngle = yAngleMax;

				lightChanged = true;
			}

			if (lightChanged)
			{
				glm::vec3 lightPosition;
				lightPosition.x = sin(glm::radians(xAngle)) * cos(glm::radians(yAngle));
				lightPosition.y = sin(glm::radians(yAngle));
				lightPosition.z = cos(glm::radians(xAngle)) * cos(glm::radians(yAngle));
				rendering.lightPosition = glm::normalize(lightPosition);

				shaderManager.setUniformVec3(uniformLocations.light, rendering.lightPosition);
				guiChanged();
			}
		}

		ImGui::End();

		// Render imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void Renderer::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void Renderer::getUniformLocations()
{
	glUseProgram(computeProgram);
	uniformLocations.origin = glGetUniformLocation(computeProgram, "Origin");
	uniformLocations.viewMatrix = glGetUniformLocation(computeProgram, "ViewMatrix");
	uniformLocations.vFov = glGetUniformLocation(computeProgram, "Vfov");
	uniformLocations.fractalIter = glGetUniformLocation(computeProgram, "Iterations");
	uniformLocations.fractalPower = glGetUniformLocation(computeProgram, "Power");
	uniformLocations.maxSteps = glGetUniformLocation(computeProgram, "MaxMarchingSteps");
	uniformLocations.detail = glGetUniformLocation(computeProgram, "MinDist");
	uniformLocations.detailPower = glGetUniformLocation(computeProgram, "DetailPower");
	uniformLocations.shadows = glGetUniformLocation(computeProgram, "Shadows");
	uniformLocations.shadowSoftness = glGetUniformLocation(computeProgram, "ShadowSoftness");
	uniformLocations.light = glGetUniformLocation(computeProgram, "Light");
	uniformLocations.bgColor = glGetUniformLocation(computeProgram, "BgColor");
	uniformLocations.fractalColor = glGetUniformLocation(computeProgram, "FractalColor");
	uniformLocations.oTrapColor = glGetUniformLocation(computeProgram, "O_TrapColor");
	uniformLocations.yTrapColor = glGetUniformLocation(computeProgram, "Y_TrapColor");
	uniformLocations.subframeOffset = glGetUniformLocation(computeProgram, "SubframeOffset");
	uniformLocations.subframeID = glGetUniformLocation(computeProgram, "SubframeID");
}

void Renderer::changeResolution()
{
	glfwSetWindowSize(window, resolution.x, resolution.y);
	glViewport(0, 0, resolution.x, resolution.y);
	glDeleteTextures(1, &frameBuffer);
	glDeleteTextures(1, &accumulationBuffer);
	frameBuffer = shaderManager.createTexture(resolution.x, resolution.y, 0, GL_WRITE_ONLY);
	accumulationBuffer = shaderManager.createTexture(resolution.x, resolution.y, 1, GL_READ_WRITE);
}

void Renderer::setFullscreen()
{
	if (fullscreen)
	{
		glfwGetWindowPos(window, &windowPos.x, &windowPos.y);
		auto monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		// switch to full screen
		glfwSetWindowMonitor(window, monitor, 0, 0, resolution.x, resolution.y, 0);
		//glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
	}
	else
	{
		glfwSetWindowMonitor(window, nullptr, windowPos.x, windowPos.y, resolution.x, resolution.y, 0);
	}
}

inline void Renderer::guiChanged()
{
	GUIchanged = true;
}

void Renderer::setGUIvisibility()
{
	static float lastChanged = 0.0f;

	float currentTime = glfwGetTime();

	if (currentTime - lastChanged > 0.2f)
	{
		if (showGUI)
		{
			showGUI = false;
		}
		else
		{
			showGUI = true;
		}
		lastChanged = currentTime;
	}
}
