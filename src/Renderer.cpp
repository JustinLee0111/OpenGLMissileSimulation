#include "Renderer.h"
#include "World.h"
#include <iostream>

// Currently only for one camera
void Renderer::draw(const World& world, float aspectRatio){
	shader->use();

	// Grabs the camera matrices
	const glm::mat4 view = world.currentCamera->getViewMatrix();
	const glm::mat4 projection = world.currentCamera->getProjectionMatrix(aspectRatio);

	// Send view and projection to shader
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);

	for (const auto& obj : world.objects) {
		if (obj->model) {
			shader->setMat4("model", obj->getObjectMatrix()); // Sends object position and orientation to shader
			obj->draw(); // Draws the model through all meshes being drawn
		}
	}
}