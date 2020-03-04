#include "Window.h"

std::string bunnyFilename = ".\\Objects\\bunny.obj";
std::string dragonFilename = ".\\Objects\\dragon.obj";
std::string bearFilename = ".\\Objects\\bear.obj";

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

	Geometry* bunny;
	Transform* robot;
	BezierCurve* curve[5];
	int curvePointCount;

	//PointCloud* objectPoints;
	//Object* currentObj, * controlledObject;
	//Light* currentLight;

	glm::vec3 eye(0, 0, 10); // Camera position.
	glm::vec3 center(0, 0, 0); // The point we are looking at.
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

	bool mouseLeftPressed = false;
	bool mouseRightPressed = false;
	double prevX, prevY;
	bool dirLightOn, pointLightOn;

	bool rotateCamera = true;
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
	robot = buildRobotArmy();
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
	glUniform1i(drawSkyboxLoc, (GLuint) 0);
	glm::mat4 robotArmyTranslation = glm::mat4(1.0f);
	robotArmyTranslation[3] = glm::vec4(curve[curvePointCount / (pointCount + 1)]->points[curvePointCount % (pointCount + 1)], 1.0f);
	robot->draw(robotArmyTranslation);
	for (BezierCurve* c : curve) {
		c->draw(glm::mat4(1.0f));
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
		//case GLFW_KEY_ESCAPE:
		//	// Close the window. This causes the program to also terminate.
		//	glfwSetWindowShouldClose(window, GL_TRUE);
		//	break;

		case GLFW_KEY_0:
			rotateCamera = !rotateCamera;
			break;

		//case GLFW_KEY_1:
		//	if (currentProgram != lightProgram) break;
		//	glUniform1i(dirLightOnLoc, (GLuint)1);
		//	glUniform1i(pointLightOnLoc, (GLuint)0);
		//	dirLightOn = true;
		//	pointLightOn = false;
		//	controlledObject = currentObj;
		//	break;

		//case GLFW_KEY_2:
		//	if (currentProgram != lightProgram) break;
		//	glUniform1i(dirLightOnLoc, (GLuint)1);
		//	glUniform1i(pointLightOnLoc, (GLuint)1);
		//	dirLightOn = true;
		//	pointLightOn = true;
		//	break;

		//case GLFW_KEY_3:
		//	if (currentProgram != lightProgram) break;
		//	break;

		//case GLFW_KEY_4:
		//	if (currentProgram != lightProgram) break;
		//	dirLightOn = ! dirLightOn;
		//	glUniform1i(dirLightOnLoc, (GLuint) dirLightOn);
		//	break;

		//case 'N':
		//	programInd = (programInd + 1) % PROGRAM_COUNT;
		//	currentProgram = programs[programInd];

		//	setShaderLoc();
		//	glUniform1i(dirLightOnLoc, (GLuint)1);
		//	glUniform1i(pointLightOnLoc, (GLuint)0);
		//	dirLightOn = true;
		//	pointLightOn = false;
		//	break;

		//case 'P':
		//	current = head;
		//	while (current->curr) {
		//		PointCloud* pcObject = (PointCloud*)current->curr;
		//		if (mods) {
		//			pcObject->updatePointSize(pcObject->getPointSize() + 1);
		//		}
		//		else {
		//			pcObject->updatePointSize(pcObject->getPointSize() - 1);
		//		}
		//		current = current->next;
		//	}
		//	break;

		//case 'A':
		//	current = head;
		//	while (current->curr) {
		//		PointCloud* pcObject = (PointCloud*)current->curr;
		//		pcObject->translate(-1, 0, 0);
		//		current = current->next;
		//	}
		//	break;

		//case 'D':
		//	current = head;
		//	while (current->curr) {
		//		PointCloud* pcObject = (PointCloud*)current->curr;
		//		pcObject->translate(1, 0, 0);
		//		current = current->next;
		//	}
		//	break;

		//case 'W':
		//	current = head;
		//	while (current->curr) {
		//		PointCloud* pcObject = (PointCloud*)current->curr;
		//		pcObject->translate(0, 1, 0);
		//		current = current->next;
		//	}
		//	break;

		//case 'X':
		//	current = head;
		//	while (current->curr) {
		//		PointCloud* pcObject = (PointCloud*)current->curr;
		//		pcObject->translate(0, -1, 0);
		//		current = current->next;
		//	}
		//	break;

		//case 'Z':
		//	current = head;
		//	while (current->curr) {
		//		PointCloud* pcObject = (PointCloud*)current->curr;
		//		if (mods) {
		//			pcObject->translate(0, 0, 1);
		//		}
		//		else {
		//			pcObject->translate(0, 0, -1);
		//		}
		//		current = current->next;
		//	}
		//	break;

		case 'S':
			if (mods) {
				robot->M = glm::scale(robot->M, glm::vec3(1.1, 1.1, 1.1));
			}
			else {
				robot->M = glm::scale(robot->M, glm::vec3(.9, .9, .9));
			}
			break;

		//case 'R':
		//	current = head;
		//	while (current->curr) {
		//		PointCloud* pcObject = (PointCloud*)current->curr;
		//		if (mods) {
		//			pcObject->cancelScaleAndRot();
		//		}
		//		else {
		//			pcObject->cancelTranslate();
		//		}
		//		current = current->next;
		//	}
		//	break;

		default:
			break;
		}
	}
}

void Window::cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
	// Based on movement of cursor positions, generate rotate axis or translation axis
	if (mouseLeftPressed) {
		glm::vec3 previousPos = trackBallMapping(prevX, prevY);
		glm::vec3 currentPos = trackBallMapping(xPos, yPos);
		glm::vec3 rotateAxis = glm::vec4(glm::cross(previousPos, currentPos), .0f);
		prevX = xPos;
		prevY = yPos;

		float radianAngle = glm::acos(std::min(std::abs(glm::dot(previousPos, currentPos)), 1.0f));

		if (rotateCamera) {
			// Control the camera, make it rotate slower
			eye = glm::rotate(radianAngle / 5, rotateAxis) * glm::vec4(eye, 1.0f);
			view = glm::lookAt(eye, center, up);
		}
		else {
			robot->M = glm::rotate(robot->M, radianAngle, rotateAxis);
		}
	}

	if (mouseRightPressed) {
		// Translate x & y
		//((PointCloud*)controlledObject)->translate((xPos - prevX) * 25 / width, (prevY - yPos) * 25 / height, 0);
		prevX = xPos;
		prevY = yPos;
	}
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	// Set mouseLeftPressed / mouseRightPressed when being pressed
	// Record prevX and prevY for mouseLeftPressed with initial positions
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			mouseLeftPressed = true;

			glfwGetCursorPos(window, &prevX, &prevY);
		}
		else if (action == GLFW_RELEASE) {
			mouseLeftPressed = false;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			mouseRightPressed = true;

			glfwGetCursorPos(window, &prevX, &prevY);
		}
		else if (action == GLFW_RELEASE) {
			mouseRightPressed = false;
		}
	}
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