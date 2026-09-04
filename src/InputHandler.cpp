#include "InputHandler.h"
#include "World.h"

GLFWwindow* InputHandler::currentWindow = nullptr;
bool InputHandler::currentKeys[1024]{false};
bool InputHandler::previousKeys[1024]{ false };
float InputHandler::windowWidth;
float InputHandler::windowHeight;
double InputHandler::mouseXpos;
double InputHandler::mouseYpos;
float InputHandler::mouseSensitivity;

void InputHandler::init(GLFWwindow* window) {
	glfwSetKeyCallback(window, key_callback);
	currentWindow = window;
	windowWidth = 1920.0f; // REMOVE AND MAKE WINDOW/APPLICATION CLASS
	windowHeight = 1440.0f; // REMOVE AND MAKE WINDOW/APPLICATION CLASS
	mouseXpos = windowWidth / 2;
	mouseYpos = windowHeight / 2;
	mouseSensitivity = 0.1f;
}

void InputHandler::cameraController() {
	double newXMousePos;
	double newYMousePos;
	glfwGetCursorPos(currentWindow, &newXMousePos, &newYMousePos);
	double xDelta = newXMousePos - mouseXpos;
	double yDelta = newYMousePos - mouseYpos;
	World::currentCamera->cameraRotate(-xDelta * mouseSensitivity, -yDelta * mouseSensitivity);
	glfwSetCursorPos(currentWindow, windowWidth / 2.0f, windowHeight / 2.0f);
}

void InputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) { currentKeys[key] = true; }
		if (action == GLFW_RELEASE) { currentKeys[key] = false; }
	}
}

bool InputHandler::isKeyPressed(int key) {
	return currentKeys[key] && !previousKeys[key];
}

void InputHandler::keysUpdate() {
	for (int i = 0; i < 1024; i++) {
		previousKeys[i] = currentKeys[i];
	}
}