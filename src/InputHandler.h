#pragma once
#include <GLFW/glfw3.h>

class InputHandler {
	public:
		static void init(GLFWwindow* window);
		static void update();
		static void cameraController();
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	private:
		static GLFWwindow* currentWindow;
		static float windowWidth;
		static float windowHeight;
		static double mouseXpos;
		static double mouseYpos;
		static bool mouseCamera;
		static float mouseSensitivity;
};