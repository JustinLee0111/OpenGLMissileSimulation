#pragma once

#include <GLFW/glfw3.h>

#include "World.h"
#include "Renderer.h"
#include "InputHandler.h"

class Application {
	public:
		Application() = default; // Default constructor
		~Application() { // Deconstructor
			if (mainWindow) {
				glfwDestroyWindow(mainWindow);
			}
			glfwTerminate();
		}

		Application(const Application&) = delete; // Delete copy constructor
		Application& operator=(const Application&) = delete; // Delete copy assignment
		Application(Application&&) = delete; // Delete move constructor
		Application& operator=(Application&&) = delete; // Delete move assignment

		int getWidth() const { return windowWidth; }
		int getHeight() const { return windowHeight; }
		float getAspectRatio() const {
			return static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
		}
		static void window_resize(GLFWwindow* window, int width, int height);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void processKeyBindings();
		int appInit();
		void runApp();
	private:
		GLFWwindow* mainWindow = nullptr;
		World world;
		Renderer appRenderer;
		InputHandler inputs;
		int windowWidth = 1920;
		int windowHeight = 1440;
		float accumulator = 0.0f;
};