#include <iostream>
#include <glad/glad.h>

#include "core/Application.h"
#include "missile/MissileSeeker.h"

int Application::appInit() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	
	mainWindow = glfwCreateWindow(getWidth(), getHeight(), "Missile Sim", nullptr, nullptr);
	glfwMakeContextCurrent(Application::mainWindow);
	glfwWindowHint(GLFW_SAMPLES, 4); // MSAA 4x sampling

	glfwSwapInterval(1); // VSYNC on
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwTerminate();
		return -1;
	}

	appRenderer.rendererInit(windowWidth, windowHeight);
	if (debugEnabled) {
		debug.Init(&world, getAspectRatio());
	}

	glfwSetWindowUserPointer(mainWindow, this); // Sets this window context to the user pointer, can be retreived to pass window context to functions

	glfwSetKeyCallback(mainWindow, key_callback); // Where to send key stroke information once detected
	glfwSetFramebufferSizeCallback(mainWindow, window_resize); // Where to send window information once resize detected

	inputs.init(mainWindow); 

	world.worldInit();

	return 0;
}

void Application::runApp(){
	float lastFrame = (float)glfwGetTime();
	const float fixedDeltaTime = PhysicsEngine::getPhysicsRate();
	while (!glfwWindowShouldClose(Application::mainWindow)) {

		glfwPollEvents();
		processKeyBindings();
		inputs.update(mainWindow);
		float currentFrame = (float)glfwGetTime();
		float frameTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Extreme low fps frametime clamp
		if (frameTime > 0.25f) frameTime = 0.25f;
		accumulator += frameTime;
		// Ensures physics simulation does fixed time steps regardless of fps
		while (accumulator >= fixedDeltaTime) {
			world.fixedUpdate(gen);
			accumulator -= fixedDeltaTime;
		}

		world.update();

		if (world.currentCamera) {
			appRenderer.draw(world, windowWidth, windowHeight);
			world.getParticleSystem().draw(world, getAspectRatio());
		}
		if (debugEnabled) {
			debugging();
		}
		inputs.keysUpdate();

		glfwSwapBuffers(mainWindow);
	}
}

void Application::debugging() {
	for (auto& missile : world.missiles) { // For quick debugging, will make cleaner
		glm::vec4 gimbalColor = glm::vec4{ 1.0f, 1.0f, 1.0f, 0.05f };
		Debugging::cone(missile->getSeeker().gimbalLimit, 5.0f, missile->position, missile->front, gimbalColor); // Gimbal limit visual

		glm::vec4 aeroColor = glm::vec4{ 0.0f, 0.0f, 1.0f, 0.1f };
		Debugging::cone(glm::radians(2.0f), glm::sqrt(glm::length(missile->getAeroForce())), missile->position, missile->getAeroForce(), aeroColor); // Aero force

		glm::vec4 velocityColor = glm::vec4{ 0.0f, 1.0f, 0.0f, 0.1f };
		glm::vec3 velocityDebug = (glm::length(missile->physicsProperties->velocity) > 0.0f) ? (missile->physicsProperties->velocity) : missile->front; 
		Debugging::cone(glm::radians(2.0f), glm::sqrt(glm::length(missile->physicsProperties->velocity)), missile->position, velocityDebug, velocityColor); // Velocity

		if (!missile->getSeeker().getSeekerOn()) { continue; }
		glm::vec3 forward{ 0.0f, 0.0f, 1.0f };
		glm::vec3 worldLookDirection = (missile->rotationQ * missile->getSeeker().seekerOrientation) * forward; // Convert local to world look direction vector
		Debugging::cone(missile->getSeeker().getSeekerFOV(), missile->getSeeker().maxRange, missile->position, worldLookDirection); // Seeker FOV visual
	}
}

void Application::processKeyBindings() {
	if (inputs.isKeyPressed(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(mainWindow, true);
	}
	if (inputs.isKeyPressed(GLFW_KEY_LEFT_ALT) && World::currentCamera) {
		if (!inputs.mouseCameraControl) {
			glfwSetCursorPos(mainWindow, windowWidth / 2.0f, windowHeight / 2.0f);
		}
		inputs.mouseCameraControl = !inputs.mouseCameraControl;
	}

	if (!world.objects.empty()) {
		glm::quat r = world.objects[0]->rotationQ;
		if (inputs.isKeyPressed(GLFW_KEY_W)) {
			world.objects[0]->physicsProperties->velocity += r * glm::vec3{ 0.0f, 0.0f, 10.0f };
		}
		if (inputs.isKeyPressed(GLFW_KEY_A)) {
			world.objects[0]->physicsProperties->velocity += r * glm::vec3{ 10.0f, 0.0f, 0.0f };
		}
		if (inputs.isKeyPressed(GLFW_KEY_S)) {
			world.objects[0]->physicsProperties->velocity += r * glm::vec3{ 0.0f, 0.0f, -10.0f };
		}
		if (inputs.isKeyPressed(GLFW_KEY_D)) {
			world.objects[0]->physicsProperties->velocity += r * glm::vec3{ -10.0f, 0.0f, 0.0f };
		}
		if (inputs.isKeyPressed(GLFW_KEY_RIGHT)) {
			world.objects[0]->physicsProperties->angularVelocity -= glm::vec3{ 0.0f, glm::pi<float>(), 0.0f};
		}
		if (inputs.isKeyPressed(GLFW_KEY_LEFT)) {
			world.objects[0]->physicsProperties->angularVelocity += glm::vec3{ 0.0f, glm::pi<float>(), 0.0f };
		}
		if (inputs.isKeyPressed(GLFW_KEY_KP_8)) {
			world.objects[0]->position += glm::vec3{ 0.0f, 1.0f, 0.0f };
		}
		if (inputs.isKeyPressed(GLFW_KEY_KP_2)) {
			world.objects[0]->position -= glm::vec3{ 0.0f, 1.0f, 0.0f };
		}
		if (inputs.isKeyPressed(GLFW_KEY_KP_4)) {
			world.objects[0]->position -= glm::vec3{ 1.0f, 0.0f, 0.0f };
		}
		if (inputs.isKeyPressed(GLFW_KEY_KP_6)) {
			world.objects[0]->position += glm::vec3{ 1.0f, 0.0f, 0.0f };
		}
		if (!world.missiles.empty()) {
			if (inputs.isKeyPressed(GLFW_KEY_R)) {
				world.missiles[0]->changeSeekerEnable();
			}
			if (inputs.isKeyPressed(GLFW_KEY_SPACE)) {
				world.missiles[0]->engineOn = true;
			}
			if (inputs.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
				world.missiles[0]->findRandomTarget(world);
			}
		}
		if (inputs.isKeyPressed(GLFW_KEY_1)) {
			world.currentCamera = world.cameras[0].get();
		}
		if (world.cameras.size() > 1) {
			if (inputs.isKeyPressed(GLFW_KEY_2)) {
				world.currentCamera = world.cameras[1].get();
			}
		}
		if (world.objects.size() > 1) {
			if (inputs.isKeyPressed(GLFW_KEY_F)) {
				world.flareBucket->deployFlares(gen, world.objects[1]->position, -world.objects[1]->up);
			}
		}
	}

	if (inputs.isKeyPressed(GLFW_KEY_U)) {
		world.unloadLevel();
		inputs.mouseCameraControl = false;
	}
	if (inputs.isKeyPressed(GLFW_KEY_L)) {
		world.unloadLevel();
		world.loadLevel("assets/levels/PhysicsSimWorldData.json");
	}
	if (inputs.isKeyPressed(GLFW_KEY_T)) {
		world.unloadLevel();
		world.loadLevel("assets/levels/PhysicsTesting.json");
	}
	if (inputs.isKeyPressed(GLFW_KEY_M)) {
		world.unloadLevel();
		world.loadLevel("assets/levels/MissileSim.json");
	}
	if (inputs.isKeyPressed(GLFW_KEY_N)) {
		world.unloadLevel();
		world.loadLevel("assets/levels/MissileSimSlow.json");
	}
	if (inputs.isKeyPressed(GLFW_KEY_B)) {
		world.unloadLevel();
		world.loadLevel("assets/levels/MissileSimSlowAlt.json");
	}
	if (inputs.isKeyPressed(GLFW_KEY_V)) {
		world.unloadLevel();
		world.loadLevel("assets/levels/MissileSimStill.json");
	}
	if (inputs.isKeyPressed(GLFW_KEY_C)) {
		world.unloadLevel();
		world.loadLevel("assets/levels/ChaseCamTest.json");
	}
}

void Application::window_resize(GLFWwindow* window, int width, int height) { // Triggers if glfw detects a window resize via glfwPollEvents()
	Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window)); // Very useful to pass window contexts to static functions
	app->inputs.windowResize(width, height);
}

void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) { // Triggers if glfw detects a key stroke
	Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) { app->inputs.currentKeys[key] = true; }
		if (action == GLFW_RELEASE) { app->inputs.currentKeys[key] = false; }
	}
}