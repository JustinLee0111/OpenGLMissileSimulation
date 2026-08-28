#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Object.h"
#include "PhysicsData.h"
#include "World.h"
#include "camera.h"

using namespace std;

int main() {
	if (!glfwInit()) {
		cerr << "Failed to initialize GLFW" << endl;
		return -1;
	}
	ifstream file;
	stringstream bufferedLines;
	string line;
    GLFWwindow* mainWindow;

	float windowWidth = 1920.0f;
	float windowHeight = 1440.0f;
	mainWindow = glfwCreateWindow((int)windowWidth, (int)windowHeight, "Physics Sim", nullptr, nullptr);
	glfwMakeContextCurrent(mainWindow);
	glfwSwapInterval(1);// VSYNC on
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwTerminate();
		return -1;
	}

	World world; // Main rendering manager and world manager
	Camera* mainCamera = world.createCamera(windowWidth, windowHeight);

	mainCamera->rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Object* sphere = world.spawnObject("models/sphere.obj", true);
	sphere->position.y = 1.0f;
	sphere->position.z = 5.0f;
	sphere->position.x = 0.0f;
	sphere->physicsProperties->collider = ColliderType::Sphere;
	sphere->physicsProperties->restitution = 1.0f;

	Object* ground = world.spawnObject("models/plane.obj", true, false, true, false);
	ground->position.y = -2.0f;
	ground->position.z = 5.0f;
	ground->physicsProperties->collider = ColliderType::Plane;
	ground->physicsProperties->restitution = 1.0f;
	ground->rotate({0,0,1}, 10.0f);

	Object* ground2 = world.spawnObject("models/plane.obj", true, false, true, false);
	ground2->position.y = -2.0f;
	ground2->position.z = 5.0f;
	ground2->position.x = -2.0f;
	ground2->physicsProperties->collider = ColliderType::Plane;
	ground2->physicsProperties->restitution = 1.0f;
	ground2->rotate({ 0,0,1 }, -10.0f);

	Object* ground3 = world.spawnObject("models/plane.obj", true, false, true, false);
	ground3->position.y = 0.0f;
	ground3->position.z = 5.0f;
	ground3->position.x = 1.0f;
	ground3->physicsProperties->collider = ColliderType::Plane;
	ground3->physicsProperties->restitution = 1.0f;
	ground3->rotate({ 0,0,1 }, 90.0f);

	Object* ground4 = world.spawnObject("models/plane.obj", true, false, true, false);
	ground4->position.y = 0.0f;
	ground4->position.z = 5.0f;
	ground4->position.x = -3.0f;
	ground4->physicsProperties->collider = ColliderType::Plane;
	ground4->physicsProperties->restitution = 1.0f;
	ground4->rotate({ 0,0,1 }, -90.0f);

	Object* ground5 = world.spawnObject("models/plane.obj", true, false, true, false);
	ground5->position.y = 2.0f;
	ground5->position.z = 5.0f;
	ground5->position.x = -1.0f;
	ground5->physicsProperties->collider = ColliderType::Plane;
	ground5->physicsProperties->restitution = 1.0f;
	ground5->rotate({ 0,0,1 }, 180.0f);

	// Fixed physics rate of 60hz
	const float fixedDeltaTime = world.getPhysicsRate();
	float accumulator = 0.0f;
	float lastFrame = (float)glfwGetTime();

	double xPos = windowWidth / 2.0;
	double yPos = windowHeight / 2.0;

	glfwGetCursorPos(mainWindow, &xPos, &yPos);

	while (!glfwWindowShouldClose(mainWindow)) {
		glfwPollEvents();

		float currentFrame = (float)glfwGetTime();
		float frameTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		double newXMousePos;
		double newYMousePos;
		glfwGetCursorPos(mainWindow, &newXMousePos, &newYMousePos);
		mainCamera->cameraUpdate(newXMousePos - xPos, newYMousePos - yPos);
		glfwSetCursorPos(mainWindow, windowWidth / 2, windowHeight / 2);
		glfwGetCursorPos(mainWindow, &xPos, &yPos);
		
		if (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(mainWindow, true);

		// Extreme low fps frametime clamp
		if (frameTime > 0.25f) frameTime = 0.25f;
		accumulator += frameTime;

		// Ensures physics simulation does fixed time steps regardless of fps
		while (accumulator >= fixedDeltaTime) {
			world.update();
			accumulator -= fixedDeltaTime;
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		world.draw();
		glfwSwapBuffers(mainWindow);	
	}
	glfwTerminate();
    return 0;
}