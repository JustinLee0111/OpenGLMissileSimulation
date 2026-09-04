#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Object.h"
#include "PhysicsData.h"
#include "World.h"
#include "camera.h"
#include "InputHandler.h"

// Description: Unfinished missile simulation, currently a custom physics simulator for bouncing sphere on sphere, sphere on static bounded plane. Uses Continuous Collision Detection (CCD) to detect collisions.
// The benefit of CCD is the ability to prevent high-speed tunneling (objects passing through one another) and calculating precise collisions.
// The biggest challenge of CCD is calculating the Time of Impact (TOI). Two methods used is Analytical and Conservative Advancement.
// Analyical solving is much faster but not as reliable and flexible as Conservative Advancement.
// 
// <-- FEATURES -->
// - Continuous Collision Detection solver with both conservative advancement and analytical solver.
// - Bounded plane collisions with dynamic normals for edge collisions (only for sphere and plane collisions).
// - Sphere on sphere collisions using impulses
// 
// --- CONTROLS ---
// Left Alt - Lock/Unlock Camera to Mouse
// Escape - Exit Program
// W - Add up velocity
// A - Add left velocity
// S - Add down velocity
// D - Add right velicty

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
	glfwSwapInterval(1); // VSYNC on
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwTerminate();
		return -1;
	}

	World world; // Main rendering manager and world manager
	Camera* mainCamera = world.createCamera(windowWidth, windowHeight);
	mainCamera->cameraPos += glm::vec3{ 0.0f, 2.0f, 10.0f };
	mainCamera->cameraRotate(0.0f, 0.0f);

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Object* sphere = world.spawnObject("models/sphere.obj", true);
	sphere->position = glm::vec3{ -1.5f, 2.0f, 0.0f };
	sphere->physicsProperties->collider = ColliderType::Sphere;
	sphere->physicsProperties->isStatic = false;
	sphere->physicsProperties->restitution = 1.0;
	sphere->physicsProperties->mass = 1.0f;

	Object* sphere2 = world.spawnObject("models/sphere.obj", true);
	sphere2->position = glm::vec3{ 0.5f, 1.0f, 0.0f };
	sphere2->physicsProperties->collider = ColliderType::Sphere;
	sphere2->physicsProperties->isStatic = false;
	sphere2->physicsProperties->restitution = 1.0f;

	/*Object* sphere3 = world.spawnObject("models/sphere.obj", true);
	sphere3->position = glm::vec3{ 0.0f, 1.0f, 0.0f };
	sphere3->physicsProperties->collider = ColliderType::Sphere;
	sphere3->physicsProperties->isStatic = false;
	sphere3->physicsProperties->restitution = 1.0f;*/

	Object* ground = world.spawnObject("models/plane.obj", true, false, true, false); // Bottom right floor
	ground->position = glm::vec3{1.5f, 0.0f, 0.0f};
	ground->physicsProperties->collider = ColliderType::Plane;
	ground->physicsProperties->restitution = 1.0f;
	ground->rotate({0,0,1}, 5.0f);

	Object* ground2 = world.spawnObject("models/plane.obj", true, false, true, false); // Bottom left floor
	ground2->position = glm::vec3{ -1.5f, 0.0f, 0.0f };
	ground2->physicsProperties->collider = ColliderType::Plane;
	ground2->physicsProperties->restitution = 1.0f;
	ground2->rotate({ 0,0,1 }, -5.0f);

	Object* ground3 = world.spawnObject("models/plane.obj", true, false, true, false); // Bottom right wall
	ground3->position = glm::vec3{ 3.0f, 1.0f, 0.0f };
	ground3->physicsProperties->collider = ColliderType::Plane;
	ground3->physicsProperties->restitution = 1.0f;
	ground3->rotate({ 0,0,1 }, 90.0f);

	Object* ground4 = world.spawnObject("models/plane.obj", true, false, true, false); // Bottom left wall
	ground4->position = glm::vec3{ -3.0f, 1.0f, 0.0f };
	ground4->physicsProperties->collider = ColliderType::Plane;
	ground4->physicsProperties->restitution = 1.0f;
	ground4->rotate({ 0,0,1 }, -90.0f);

	Object* ground5 = world.spawnObject("models/plane.obj", true, false, true, false); // Top left roof
	ground5->position = glm::vec3{ -1.5f, 5.0f, 0.0f };
	ground5->physicsProperties->collider = ColliderType::Plane;
	ground5->physicsProperties->restitution = 1.0f;
	ground5->rotate({ 0,0,1 }, 180.0f);

	Object* ground6 = world.spawnObject("models/plane.obj", true, false, true, false); // Top right roof
	ground6->position = glm::vec3{ 1.5f, 5.0f, 0.0f };
	ground6->physicsProperties->collider = ColliderType::Plane;
	ground6->physicsProperties->restitution = 1.0f;
	ground6->rotate({ 0,0,1 }, 180.0f);

	Object* ground7 = world.spawnObject("models/plane.obj", true, false, true, false); // Top left wall
	ground7->position = glm::vec3{ -3.0f, 4.0f, 0.0f };
	ground7->physicsProperties->collider = ColliderType::Plane;
	ground7->physicsProperties->restitution = 1.0f;
	ground7->rotate({ 0,0,1 }, -90.0f);

	Object* ground8 = world.spawnObject("models/plane.obj", true, false, true, false); // Top right wall
	ground8->position = glm::vec3{ 3.0f, 4.0f, 0.0f };
	ground8->physicsProperties->collider = ColliderType::Plane;
	ground8->physicsProperties->restitution = 1.0f;
	ground8->rotate({ 0,0,1 }, 90.0f);

	// Fixed physics rate of 60hz
	const float fixedDeltaTime = world.getPhysicsRate();
	float accumulator = 0.0f;
	float lastFrame = (float)glfwGetTime();

	InputHandler::init(mainWindow);

	while (!glfwWindowShouldClose(mainWindow)) {
		glfwPollEvents();

		// PUT THESE IN APPLICATION / WINDOW CLASS
		if (InputHandler::isKeyPressed(GLFW_KEY_LEFT_ALT)) {
			glfwSetCursorPos(mainWindow, windowWidth / 2, windowHeight / 2);
			Camera::mouseControl = !Camera::mouseControl;
		}
		if (InputHandler::isKeyPressed(GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(mainWindow, true);
		}
		if (InputHandler::isKeyPressed(GLFW_KEY_W)) {
			sphere->physicsProperties->velocity += glm::vec3{ 0.0f, 4.0f, 0.0f };
		}
		if (InputHandler::isKeyPressed(GLFW_KEY_A)) {
			sphere->physicsProperties->velocity += glm::vec3{ -4.0f, 0.0f, 0.0f };
		}
		if (InputHandler::isKeyPressed(GLFW_KEY_S)) {
			sphere->physicsProperties->velocity += glm::vec3{ 0.0f, -4.0f, 0.0f };
		}
		if (InputHandler::isKeyPressed(GLFW_KEY_D)) {
			sphere->physicsProperties->velocity += glm::vec3{ 4.0f, 0.0f, 0.0f };
		}

		if (Camera::mouseControl) {
			InputHandler::cameraController();
		}

		float currentFrame = (float)glfwGetTime();
		float frameTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		

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
		InputHandler::keysUpdate();
	}
	glfwTerminate();
    return 0;
}