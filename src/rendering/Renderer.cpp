#include "rendering/Renderer.h"
#include "environment/World.h"
#include "environment/Light.h"
#include "environment/Object.h"
#include "rendering/Model.h"

#include <glad/glad.h>
#include <iostream>

// Currently only for one camera
void Renderer::draw(const World& world, float aspectRatio){
	shader->use();
	// Grabs the camera matrices
	const glm::mat4 view = world.currentCamera->getViewMatrix();
	const glm::mat4 projection = world.currentCamera->getProjectionMatrix(aspectRatio);

	shader->setVec4("lightColor", world.lights[0]->getLightColor());
	shader->setVec3("lightPos", world.lights[0]->getLightPos());
	shader->setFloat("ambientLightStrength", world.getAmbientLightStrength());

	// Send view and projection to shader
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);
	shader->setVec3("camPos", world.currentCamera->cameraPos);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	shader->setBool("lightingEnable", true);
	for (const auto& obj : world.objects) {
		if (obj->model) {
			shader->setMat4("model", obj->getObjectMatrix()); // Sends object position and orientation to shader
			obj->model->drawOpaque(); // Draws the model through all meshes being drawn
		}
	}
	for (const auto& obj : world.missiles) {
		if (obj->model) {
			shader->setMat4("model", obj->getObjectMatrix()); // Sends object position and orientation to shader
			obj->model->drawOpaque(); // Draws the model through all meshes being drawn
		}
	}

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	shader->setBool("lightingEnable", false);
	for (const auto& obj : world.objects) {
		if (obj->model) {
			shader->setMat4("model", obj->getObjectMatrix()); // Sends object position and orientation to shader
			obj->model->drawTransparent(); // Draws the model through all meshes being drawn
		}
	}
	for (const auto& obj : world.missiles) {
		if (obj->model) {
			shader->setMat4("model", obj->getObjectMatrix()); // Sends object position and orientation to shader
			obj->model->drawTransparent(); // Draws the model through all meshes being drawn
		}
	}

	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	shader->setBool("lightingEnable", true);
}