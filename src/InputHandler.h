#pragma once
#include <GLFW/glfw3.h>
#include <vector>

class InputHandler {
	public:
		static bool currentKeys[1024];
		static bool previousKeys[1024];
		static void init(GLFWwindow* window);
		static void cameraController();
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static bool isKeyPressed(int key);  // Checks if key pressed and only returns initial press
		static void keysUpdate(); // Sets the previousKeys as currentKeys at the end of the frame

	private:
		static GLFWwindow* currentWindow;
		static float windowWidth;
		static float windowHeight;
		static double mouseXpos;
		static double mouseYpos;
		static float mouseSensitivity;
};