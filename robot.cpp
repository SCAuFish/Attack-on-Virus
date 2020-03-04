#include "robot.h"

std::string bodyFilename = ".\\Objects\\body_s.obj";
std::string limbFilename = ".\\Objects\\limb_s.obj";
std::string headFilename = ".\\Objects\\head_s.obj";
std::string eyeFilename = ".\\Objects\\eyeball_s.obj";
std::string antennaFilename = ".\\Objects\\antenna_s.obj";

Geometry* torsoGeo, *limbGeo, *headGeo, *eyeGeo, *antennaGeo;

int period = 200;
int updateCount = 0;

Transform* buildRobotArmy()
{
	Transform* root = new Transform();

	for (int i = -2; i <= 2; i++) {
		for (int j = -2; j <= 2; j++) {
			Transform* singleRobot = buildRobot();
			singleRobot->M = glm::translate(glm::vec3(4*i, 0, 4*j)) * singleRobot->M;

			root->addChild(singleRobot);
		}
	}

	return root;
}

Transform* buildRobot() {
	// Load basic geometries
	torsoGeo = new Geometry();
	torsoGeo->loadObjFile(bodyFilename);
	limbGeo = new Geometry();
	limbGeo->loadObjFile(limbFilename);
	headGeo = new Geometry();
	headGeo->loadObjFile(headFilename);
	eyeGeo = new Geometry();
	eyeGeo->loadObjFile(eyeFilename);
	antennaGeo = new Geometry();
	antennaGeo->loadObjFile(antennaFilename);

	Transform* root = new Transform();

	// build torso
	Transform* torso = new Transform();
	torso->addChild(torsoGeo);
	torso->M = glm::scale(torso->M, glm::vec3(1.f, .9f,1.f));

	// build limb
	Transform* left_arm = new Transform();
	left_arm->M = glm::translate(glm::vec3(-1.2, 0, 0)) * left_arm->M;
	left_arm->M = glm::scale(left_arm->M, glm::vec3(0.6f, 0.7f, 0.6f));
	left_arm->M = glm::rotate(left_arm->M, 0.5f, glm::vec3(1.f, .0f, .0f));
	left_arm->setUpdateFunc(leftArmSwing);
	left_arm->addChild(limbGeo);

	Transform* right_arm = new Transform();
	right_arm->M = glm::translate(glm::vec3(1.2, 0, 0)) * right_arm->M;
	right_arm->M = glm::scale(right_arm->M, glm::vec3(0.6f, 0.7f, 0.6f));
	right_arm->M = glm::rotate(right_arm->M, -0.5f, glm::vec3(1.f, .0f, .0f));
	right_arm->setUpdateFunc(rightArmSwing);
	right_arm->addChild(limbGeo);

	Transform* left_leg = new Transform();
	left_leg->M = glm::translate(glm::vec3(-0.4, -.9, 0)) * left_leg->M;
	left_leg->M = glm::scale(left_leg->M, glm::vec3(1.2f, 0.5f, 1.2f));
	left_leg->M = glm::rotate(left_leg->M, -0.25f, glm::vec3(1.f, .0f, .0f));
	left_leg->setUpdateFunc(leftLegSwing);
	left_leg->addChild(limbGeo);

	Transform* right_leg = new Transform();
	right_leg->M = glm::translate(glm::vec3(0.4, -.9, 0)) * right_leg->M;
	right_leg->M = glm::scale(right_leg->M, glm::vec3(1.2f, 0.5f, 1.2f));
	right_leg->M = glm::rotate(right_leg->M, 0.25f, glm::vec3(1.f, .0f, .0f));
	right_leg->setUpdateFunc(rightLegSwing);
	right_leg->addChild(limbGeo);

	// build head
	Transform* head = new Transform();

	Transform* mainHead = new Transform();
	mainHead->addChild(headGeo);

	Transform* left_eye = new Transform();
	left_eye->addChild(eyeGeo);
	left_eye->M = glm::translate(glm::vec3(-.4, -.1, .8)) * left_eye->M;
	left_eye->M = glm::scale(left_eye->M, glm::vec3(.1f, .1f, .1f));

	Transform* right_eye = new Transform();
	right_eye->addChild(eyeGeo);
	right_eye->M = glm::translate(glm::vec3(.4, -.1, .8)) * right_eye->M;
	right_eye->M = glm::scale(right_eye->M, glm::vec3(.1f, .1f, .1f));

	Transform* left_antenna = new Transform();
	left_antenna->addChild(antennaGeo);
	left_antenna->M = glm::scale(left_antenna->M, glm::vec3(1.f, .3f, 1.f));
	left_antenna->M = glm::rotate(0.5f, glm::vec3(0, 0, 1)) * left_antenna->M;
	left_antenna->M = glm::translate(glm::vec3(-.4, .5, 0)) * left_antenna->M;

	Transform* right_antenna = new Transform();
	right_antenna->addChild(antennaGeo);
	right_antenna->M = glm::scale(right_antenna->M, glm::vec3(1.f, .3f, 1.f));
	right_antenna->M = glm::rotate(-0.5f, glm::vec3(0, 0, 1)) * right_antenna->M;
	right_antenna->M = glm::translate(glm::vec3(.4, .5, 0)) * right_antenna->M;

	head->addChild(mainHead);
	head->addChild(left_eye);
	head->addChild(right_eye);
	head->addChild(left_antenna);
	head->addChild(right_antenna);

	head->M = glm::translate(glm::vec3(0, 1.4, 0)) * head->M;
	head->setUpdateFunc(headShake);

	root->addChild(torso);
	root->addChild(left_arm);
	root->addChild(right_arm);
	root->addChild(left_leg);
	root->addChild(right_leg);
	root->addChild(head);

	return root;
}

glm::mat4 leftArmSwing(int currentCount, glm::mat4& model)
{
	// If greater than half of the period, swing forward
	float radianAngle = updateCount >= (period / 2.0f) ? .01f : -.01f;
	// Else swing backward
	model = glm::rotate(model, radianAngle, glm::vec3(1.f, .0f, .0f));

	return glm::mat4(1.0f);
}

glm::mat4 rightArmSwing(int currentCount, glm::mat4& model)
{
	// If greater than half of the period, swing forward
	float radianAngle = updateCount >= (period / 2.0f) ? -.01f : .01f;
	// Else swing backward
	model = glm::rotate(model, radianAngle, glm::vec3(1.f, .0f, .0f));

	return glm::mat4(1.0f);
}

glm::mat4 leftLegSwing(int currentCount, glm::mat4& model)
{
	// If greater than half of the period, swing forward
	float radianAngle = updateCount >= (period / 2.0f) ? -.005f : .005f;
	// Else swing backward
	model = glm::rotate(model, radianAngle, glm::vec3(1.f, .0f, .0f));

	return glm::mat4(1.0f);
}

glm::mat4 rightLegSwing(int currentCount, glm::mat4& model)
{
	// If greater than half of the period, swing forward
	float radianAngle = updateCount >= (period / 2.0f) ? .005f : -.005f;
	// Else swing backward
	model = glm::rotate(model, radianAngle, glm::vec3(1.f, .0f, .0f));

	return glm::mat4(1.0f);
}

glm::mat4 headShake(int currentCount, glm::mat4& model) {
	// If greater than half of the period, swing forward
	float radianAngle = (updateCount >= (period / 4.0f) && updateCount < (period * 3 / 4.0f) ) ? .005f : -.005f;
	// Else swing backward
	model = glm::rotate(model, radianAngle, glm::vec3(.0f, .1f, .0f));

	return glm::mat4(1.0f);
}