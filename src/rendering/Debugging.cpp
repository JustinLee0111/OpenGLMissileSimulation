#include <vector>
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glad/glad.h>

#include "rendering/Debugging.h"
#include "Shader.h"
#include "core/AssetManager.h"
#include "environment/World.h"

std::unique_ptr<Shader> Debugging::debugShader = nullptr;
std::shared_ptr<Model> Debugging::debugCone = nullptr;
World* Debugging::world = nullptr;
float Debugging::aspectRatio{ 0.0f };

void Debugging::cone(glm::vec3 position, glm::vec3 direction) {
	debugShader->use();
	const glm::mat4 view = world->currentCamera->getViewMatrix();
	const glm::mat4 projection = world->currentCamera->getProjectionMatrix(aspectRatio);
	debugShader->setMat4("view", view);
	debugShader->setMat4("projection", projection);

	glm::quat orientation = glm::quatLookAt(glm::normalize(direction), glm::vec3{ 0.0f, 1.0f, 0.0f });
	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(orientation);
	debugShader->setMat4("model", matrix); // Sends object position and orientation to shader
	debugCone->drawTransparent(); // Draws the model through all meshes being drawn
}

void Debugging::createCone(float angle, float length, glm::vec4 color) {
	float pi = glm::pi<float>();
	int segments = 16;
	float angleStep = pi / (segments / 2.0f);
	float radius = glm::tan(glm::radians(angle / 2.0f)) * length;
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	vertices.push_back(glm::vec3{ 0.0f });

	for (int i = 0; i <= segments; i++) {
		float theta = i * angleStep;
		vertices.push_back(glm::vec3{ glm::sin(theta) * radius, glm::cos(theta) * radius, -length });
	}

	for (int i = 1; i <= segments; i++) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i + 1);

		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}
	debugCone = AssetManager::loadModel("debugCone", vertices, indices, color);
}

void Debugging::Init(World* world, float aspectRatio) {
	this->world = world;
	this->aspectRatio = aspectRatio;
	debugShader = std::make_unique<Shader>("shaders/debug.vert", "shaders/debug.frag");
	debugShader->use();
	createCone(45.0f, 100.0f, glm::vec4{ 1.0f, 1.0f, 1.0f, 0.25f });
}