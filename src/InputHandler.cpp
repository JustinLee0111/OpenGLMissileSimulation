#include "InputHandler.h"
#include "World.h"

GLFWwindow* InputHandler::currentWindow = nullptr;
float InputHandler::windowWidth;
float InputHandler::windowHeight;
double InputHandler::mouseXpos;
double InputHandler::mouseYpos;
bool InputHandler::mouseCamera;
float InputHandler::mouseSensitivity;

void InputHandler::init(GLFWwindow* window) {
	glfwSetKeyCallback(window, key_callback);
	currentWindow = window;
	mouseCamera = false;
	windowWidth = 1920.0f; // REMOVE AND MAKE WINDOW/APPLICATION CLASS
	windowHeight = 1440.0f; // REMOVE AND MAKE WINDOW/APPLICATION CLASS
	mouseXpos = windowWidth / 2;
	mouseYpos = windowHeight / 2;
	mouseSensitivity = 0.1f;
}

void InputHandler::update() {
	if (mouseCamera) {
		cameraController();
	}
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
    if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
		glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
		mouseCamera = !mouseCamera;
    }
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(currentWindow, true);
	}
}