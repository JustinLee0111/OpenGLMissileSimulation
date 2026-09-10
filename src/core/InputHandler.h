#pragma once
#include <GLFW/glfw3.h>

class InputHandler {
	public:
		bool mouseCameraControl = false;
		bool currentKeys[1024]{ false };
		bool previousKeys[1024]{ false };
		void init(GLFWwindow* window);
		void windowResize(int width, int height) { // Application calls this upon window resize
			windowWidth = width;
			windowHeight = height;
		}
		void update(GLFWwindow* window);
		void cameraController(GLFWwindow* window);
		bool isKeyPressed(int key) const;  // Checks if key pressed and only returns initial press
		void keysUpdate(); // Sets the previousKeys as currentKeys at the end of the frame

	private:
		int windowWidth;
		int windowHeight;
		float mouseSensitivity = 0.1f;
};