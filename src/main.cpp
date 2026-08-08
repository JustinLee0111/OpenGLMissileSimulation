#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.h"
#include "triangle_mesh.h"
#include "object.h"
#include "physics.h"
#include "world.h"

using namespace std;

int main() {
	ifstream file;
	stringstream bufferedLines;
	string line;
	World world;

    GLFWwindow* mainWindow;

	if (!glfwInit()) {
		cerr << "Failed to initialize GLFW" << endl;
		return -1;
	}

	// Holds all objects in the world
	vector<unique_ptr<Object>> worldObjects;

	// Camera setup
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 5.0f, 20.0f), // Camera position
		glm::vec3(0.0f, 0.0f, 0.0f), // Look at point
		glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
	);

	float aspectRatio = 2880.0f / 2160.0f;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);

	mainWindow = glfwCreateWindow(2880, 2160, "Physics Sim", nullptr, nullptr);
	glfwMakeContextCurrent(mainWindow);
	
	glfwSwapInterval(1);// VSYNC on

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwTerminate();
		return -1;
	}

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Adds sphere object with physics enabled to worldObjects list
	Object* sphere = world.spawnObject("models/sphere.obj", true);
	sphere->position.y = 2.0f;
	sphere->position.z = 5.0f;
	sphere->physics->velocity.y = 0.0f;
	sphere->physics->gravity = -9.81f;
	sphere->physics->collider = BoundingBox::Sphere;
	sphere->physics->restitution = 0.7f;

	// Adds ground as a kinematic moving object
	Object* ground = world.spawnObject("models/plane.obj", true, false, true, true);
	ground->position.y = -1.0f;
	ground->position.z = 5.0f;
	ground->physics->collider = BoundingBox::Plane;
	ground->physics->restitution = 1.0f;

	//TriangleMesh* triangle = new TriangleMesh();

	shaders shaderProgram;
	unsigned int shader = shaderProgram.make_shader("shaders/vertex.vert", "shaders/fragment.frag");
	glUseProgram(shader);
	
	unsigned int modelLocation = glGetUniformLocation(shader, "model");
	unsigned int viewLoc = glGetUniformLocation(shader, "view");
	unsigned int projectionLoc = glGetUniformLocation(shader, "projection");

	// Upload view and projection
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Fixed physics rate to 60hz
	const float fixedDeltaTime = 1.0f / 60.0f;
	float accumulator = 0.0f;
	float lastFrame = (float)glfwGetTime();

	while (!glfwWindowShouldClose(mainWindow)) {
		glfwPollEvents();
		glUseProgram(shader);

		float currentFrame = (float)glfwGetTime();
		float frameTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Extreme low fps frametime clamp
		if (frameTime > 0.25f) frameTime = 0.25f;

		accumulator += frameTime;

		// Ensures physics simulation does fixed time steps
		while (accumulator >= fixedDeltaTime) {
			world.update(fixedDeltaTime);
			accumulator -= fixedDeltaTime;
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		world.draw(modelLocation);
		glfwSwapBuffers(mainWindow);
	}

	glDeleteProgram(shader);
	glfwTerminate();
    return 0;
}