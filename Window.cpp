#include "Window.h"

std::string virusFileName = ".\\Objects\\Coronavirus_processed.obj";
std::string fighterFileName = ".\\Objects\\fighter_processed.obj";
std::string sphereFileName = ".\\Objects\\sphere.obj";


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
	int hasBullet = 0, progress = 0;
	std::string windowTitle("GLFW Starter Project");

	Skybox* skybox;

	Geometry* virus, * fighter, * bullet;
	Transform* world;
	Transform* robot;
	BezierCurve* curve[5];
	int curvePointCount;

	glm::vec3 eye(0, 0, 0); // Camera position.
	glm::vec3 center(0, -2, 5); // The point we are looking at.
	glm::vec3 prev_center(0, 0, 1); // The point we are looking at.
	glm::vec3 up(0, 1, 0); // The up direction of the camera.
	float fovy = 60;
	float near = 1;
	float far = 1000;
	glm::mat4 view = glm::lookAt(eye, center, up); // View matrix, defined by eye, center and up.
	glm::mat4 projection; // Projection matrix.
	glm::mat4 rot = glm::mat4(1.0f);

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
	virus->loadObjFile(virusFileName, 1);
	virus->setModelLoc(modelLoc);

	fighter = new Geometry();
	fighter->loadObjFile(fighterFileName, 1);
	fighter->setModelLoc(modelLoc);
	fighter->changeModel(glm::translate(glm::vec3(0.0f, -2.0f, 5.0f)));

	world = new Transform();
	world->addChild(virus);
	
	bullet = new Geometry();
	bullet->loadObjFile(sphereFileName, 0);
	printf("here\n");
	bullet->setModelLoc(modelLoc);
	
	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	//delete skybox;
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
	if (hasBullet) {
		progress++;
		bullet->object->model = glm::translate(bullet->object->model, glm::vec3(.0f, .0f, 0.6f));
		if (progress > 300) hasBullet = 0;
	}
}

void Window::launchBullet() {
	hasBullet = 1;
	progress = 0;
	bullet = new Geometry();
	bullet->loadObjFile(sphereFileName, 0);
	bullet->setModelLoc(modelLoc);
	bullet->object->model = fighter->object->model;
	bullet->object->model = glm::scale(bullet->object->model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniform1i(drawSkyboxLoc, (GLuint)0);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bullet->object->model));
	bullet->draw(glm::mat4(1.0f));
}

void Window::displayCallback(GLFWwindow* window)
{
	// Update count to count period
	updateCount = (updateCount + 1) % period;
	curvePointCount = (curvePointCount + 1) % (5 * (pointCount + 1));
	//robot->update();

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
	glm::mat4 robotArmyTranslation = glm::translate(glm::vec3(.0f, 0.0f, 10.f));
	//bullet->draw(robotArmyTranslation);
	virus->draw(robotArmyTranslation);
	fighter->draw(glm::mat4(1.0f));

	// Draw bullet 
	if (hasBullet) {
		bullet->draw(glm::mat4(1.0f));
	}

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
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GL_TRUE);
				break;
			case GLFW_KEY_W:
				glm::vec3 dir = glm::normalize(center - eye);
				dir = rot * glm::vec4(dir, 1.0f);
				skybox->model = glm::translate(glm::vec3(-10 * dir[0], -10 * dir[1], -10 * dir[2])) * skybox->model;
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(skybox->model));
				skybox->draw();
				world->changeModel(glm::translate(glm::vec3(-1 * dir[0], -1 * dir[1], -1 * dir[2])));
				world->draw(glm::mat4(1.0f));
				break;
			case GLFW_KEY_ENTER:
				launchBullet();
				break;
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
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		fighter->object->model = glm::rotate(fighter->object->model, -glm::radians((float)(xPos - prevX)) / 10.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		fighter->object->model = glm::rotate(fighter->object->model, -glm::radians((float)(yPos - prevY)) / 10.0f, glm::cross(center, up));
		rot = matRoll * matPitch * rot;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		fighter->changeModel(matRoll * matPitch);
		center = glm::normalize(matRoll * matPitch * glm::vec4(center, 1.f));
		up = glm::normalize(matRoll * matPitch * glm::vec4(up, 1.f));
		view = glm::lookAt(eye, center, up);
	}
	prevX = xPos;
	prevY = yPos;
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