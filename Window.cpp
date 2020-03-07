#include "Window.h"

std::string virusFileName = ".\\Objects\\Coronavirus_processed.obj";
std::string fighterFileName = ".\\Objects\\fighter_processed.obj";

// This order of cubemap texture should not be changed
std::vector<char*> textureFiles = { ".\\skybox\\right.ppm", ".\\skybox\\left.ppm", ".\\skybox\\top.ppm",
	".\\skybox\\bottom.ppm", ".\\skybox\\back.ppm", ".\\skybox\\front.ppm" };
ObjectPointerNode* Window::head, * Window::tail;

//PointCloud* bunny, * dragon, * bear;
//Light* directionalLight, * pointLight;
/*
 * Declare your variables below. Unnamed namespace is used here to avoid
 * declaring global or static variables.
 */
namespace
{
	int width, height;
	std::string windowTitle("GLFW Starter Project");

	Skybox* skybox;

	Geometry* virus, * fighter;
	Transform* robot;
	BezierCurve* curve[5];
	int curvePointCount;

	//PointCloud* objectPoints;
	//Object* currentObj, * controlledObject;
	//Light* currentLight;

	glm::vec3 eye(0, 0, 0); // Camera position.
	glm::vec3 center(0, 0, 1); // The point we are looking at.
	glm::vec3 up(0, 1, 0); // The up direction of the camera.
	float fovy = 60;
	float near = 1;
	float far = 1000;
	glm::mat4 view = glm::lookAt(eye, center, up); // View matrix, defined by eye, center and up.
	glm::mat4 projection; // Projection matrix.

	GLuint currentProgram, skyboxProgram; // The shader program id.

	GLuint projectionLoc; // Location of projection in shader.
	GLuint viewLoc; // Location of view in shader.
	GLuint modelLoc; // Location of model in shader.
	GLuint drawSkyboxLoc;

	double prevX, prevY;
};

void setShaderLoc() {
	glUseProgram(currentProgram);
	// Get the locations of uniform variables.
	projectionLoc = glGetUniformLocation(currentProgram, "projection");
	viewLoc = glGetUniformLocation(currentProgram, "view");
	modelLoc = glGetUniformLocation(currentProgram, "model");

	drawSkyboxLoc = glGetUniformLocation(currentProgram, "drawSkybox");

}

bool Window::initializeProgram()
{
	skyboxProgram = LoadShaders("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");

	// Create cube for the skybox
	skybox = new Skybox(1024);

	// Load texture
	unsigned int textureID = loadTexture(textureFiles);
	skybox->setTextureID(textureID);

	// Check the shader programs.
	if (!skyboxProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	currentProgram = skyboxProgram;
	setShaderLoc();
	return true;
}

void Window::addObjects(Object* toAdd) {
	if (head == nullptr) {
		head = new ObjectPointerNode();
		head->curr = toAdd;
		tail = head;
	}

	tail->next = new ObjectPointerNode();
	tail->curr = toAdd;

	tail = tail->next;
}

bool Window::initializeObjects()
{
	virus = new Geometry();
	virus->loadObjFile(virusFileName);
	virus->setModelLoc(modelLoc);

	fighter = new Geometry();
	fighter->loadObjFile(fighterFileName);
	fighter->setModelLoc(modelLoc);

	// Use this robot and curve to debug
	robot = buildRobot();
	robot->setModelLoc(modelLoc);

	curve[0] = new BezierCurve(glm::vec3(-3, -3, 1), glm::vec3(-2, -1, 3), glm::vec3(0, 1, 2), glm::vec3(2, 2, 4));
	curve[1] = new BezierCurve(glm::vec3(2, 2, 4), glm::vec3(4, 3, 6), glm::vec3(3, 4, 2), glm::vec3(2, 2, 0));
	curve[2] = new BezierCurve(glm::vec3(2, 2, 0), glm::vec3(1, 0, -2), glm::vec3(-3, -1, -4), glm::vec3(-4, -3, -5));
	curve[3] = new BezierCurve(glm::vec3(-4, -3, -5), glm::vec3(-5, -5, -6), glm::vec3(-7, -4, -3), glm::vec3(-4, -2, -1));
	curve[4] = new BezierCurve(glm::vec3(-4, -2, -1), glm::vec3(-1, 0, 1), glm::vec3(-4, -5, -1), glm::vec3(-3, -3, 1));
	for (BezierCurve* c : curve) {
		c->setModelLoc(modelLoc);
	}

	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete skybox;
	glDeleteProgram(skyboxProgram);
}

GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);


	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int w, int h)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height);
#endif
	width = w;
	height = h;

	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	projection = glm::perspective(glm::radians(fovy),
		(float)width / (float)height, near, far);
}

void Window::idleCallback()
{
	// Perform any updates as necessary.
	/*ObjectPointerNode* current = head;
	while (current->curr) {
		current->curr->update();
		current = current->next;
	}*/
}

void Window::displayCallback(GLFWwindow* window)
{
	// Update count to count period
	updateCount = (updateCount + 1) % period;
	curvePointCount = (curvePointCount + 1) % (5 * (pointCount + 1));
	robot->update();

	glUseProgram(skyboxProgram);
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw skybox first
	glUniform1i(drawSkyboxLoc, (GLuint)1);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(skybox->model));
	skybox->draw();

	// Draw objects
	glUniform1i(drawSkyboxLoc, (GLuint)0);
	glm::mat4 robotArmyTranslation = glm::translate(glm::vec3(.0f, .0f, 10.f));
	//robot->draw(robotArmyTranslation);
	virus->draw(robotArmyTranslation);
	fighter->draw(glm::translate(glm::vec3(.0f, -2.0f, 5.f)));

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);

}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press.
	ObjectPointerNode* current;
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		}
	}
}

void Window::cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
	// Based on movement of cursor positions, generate rotate axis or translation axis
	glm::mat4 matRoll = glm::mat4(1.f);
	glm::mat4 matPitch = glm::mat4(1.f);
	matRoll = glm::rotate(-glm::radians((float)(xPos - prevX)) / 10.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	matPitch = glm::rotate(-glm::radians((float)(yPos - prevY)) / 10.0f, glm::cross(center, up));
	center = glm::normalize(matRoll * matPitch * glm::vec4(center, 1.f));
	up = glm::normalize(matRoll * matPitch * glm::vec4(up, 1.f));
	prevX = xPos;
	prevY = yPos;

	view = glm::lookAt(eye, center, up);
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	// Set mouseLeftPressed / mouseRightPressed when being pressed
	// Record prevX and prevY for mouseLeftPressed with initial positions
	//if (button == GLFW_MOUSE_BUTTON_LEFT) {
	//	if (action == GLFW_PRESS) {
	//		mouseLeftPressed = true;

	//		glfwGetCursorPos(window, &prevX, &prevY);
	//	}
	//	else if (action == GLFW_RELEASE) {
	//		mouseLeftPressed = false;
	//	}
	//}
	//else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
	//	if (action == GLFW_PRESS) {
	//		mouseRightPressed = true;

	//		glfwGetCursorPos(window, &prevX, &prevY);
	//	}
	//	else if (action == GLFW_RELEASE) {
	//		mouseRightPressed = false;
	//	}
	//}
}

void Window::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	//if (yOffset > 0) {
	//	((PointCloud*)currentObj)->translate(.0f, .0f, .3f);
	//}
	//else if (yOffset < 0) {
	//	((PointCloud*)currentObj)->translate(.0f, .0f, -.3f);
	//}
}

glm::vec3 Window::trackBallMapping(double xPos, double yPos)
{
	// Need a normalized result
	glm::vec3 result(.0f, .0f, .0f);
	float d;
	result[0] = (2.0 * xPos - width) / width;
	result[1] = (height - 2.0 * yPos) / height;
	d = result.length();
	d = (d < 1.0f) ? d : 1.0f;
	result[2] = sqrtf(1.001 - ((double)d) * d);

	return glm::normalize(result);
}