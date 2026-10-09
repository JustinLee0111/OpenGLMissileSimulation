#pragma once

#include <GLFW/glfw3.h>
#include <random>
#include <ctime>

#include "environment/World.h"
#include "rendering/Renderer.h"
#include "core/InputHandler.h"
#include "rendering/Debugging.h"

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
		const float getFrametime() const { return frameTime; }
		float getAspectRatio() const {
			return static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
		}
		static void window_resize(GLFWwindow* window, int width, int height);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void processKeyBindings();
		void debugging();
		int appInit();
		void runApp();
	private:
		GLFWwindow* mainWindow{ nullptr };
		World world;
		Debugging debug;
		Renderer appRenderer;
		InputHandler inputs;
		static int windowWidth;
		static int windowHeight;
		float frameTime{ 0.0f };
		float accumulator{ 0.0f };
		bool debugEnabled{ true };
		std::mt19937 gen{ (unsigned int)std::time(0) };
};