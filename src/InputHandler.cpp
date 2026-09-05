#include "InputHandler.h"
#include "World.h"

void InputHandler::init(GLFWwindow* window) {
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
}

void InputHandler::update(GLFWwindow* window) {
	if (mouseCameraControl && World::currentCamera) {
		cameraController(window);
	}
}

void InputHandler::cameraController(GLFWwindow* window) {
	double newXMousePos;
	double newYMousePos;
	glfwGetCursorPos(window, &newXMousePos, &newYMousePos);
	double xDelta = newXMousePos - windowWidth / 2.0f;
	double yDelta = newYMousePos - windowHeight / 2.0f;
	World::currentCamera->cameraRotate(-xDelta * mouseSensitivity, -yDelta * mouseSensitivity);
	glfwSetCursorPos(window, windowWidth / 2.0f, windowHeight / 2.0f);
}

bool InputHandler::isKeyPressed(int key) const{ // Checks if key is pressed, doesn't continuously repeat inputs
	return currentKeys[key] && !previousKeys[key];
}

void InputHandler::keysUpdate() { // Sets inputs during current frame to previous at the end of the frame
	for (int i = 0; i < 1024; i++) {
		previousKeys[i] = currentKeys[i];
	}
}