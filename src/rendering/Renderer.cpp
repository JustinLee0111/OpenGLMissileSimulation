#include "rendering/Renderer.h"
#include "environment/World.h"
#include "environment/Light.h"
#include "environment/Object.h"
#include "rendering/Model.h"

#include <glad/glad.h>

// Currently only for one camera
void Renderer::draw(const World& world, float windowWidth, float windowHeight){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float aspectRatio = windowWidth / windowHeight;

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

	// Draws opaque objects first
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	shader->setBool("lightingEnable", true);
	for (const auto& obj : world.objects) {
		if (obj->model) {
			shader->setMat4("model", obj->getObjectMatrix()); // Sends object position and orientation to shader
			shader->setBool("useColor", true);
			shader->setVec4("color", obj->color);
			shader->setVec3("scale", obj->scale);
			obj->model->drawOpaque(); // Draws the model through all meshes being drawn
		}
	}

	// Draws transparent objects
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);

	shader->setBool("lightingEnable", false);
	for (const auto& obj : world.objects) {
		if (obj->model) {
			shader->setMat4("model", obj->getObjectMatrix()); // Sends object position and orientation to shader
			shader->setVec3("scale", obj->scale);
			obj->model->drawTransparent(); // Draws the model through all meshes being drawn
		}
	}
	shader->setBool("lightingEnable", true);

	// Draw infinite grid
	gridShader->use();
	gridShader->setMat4("view", view);
	gridShader->setMat4("projection", projection);
	gridShader->setVec3("camPos", world.currentCamera->cameraPos);

	glBindVertexArray(gridVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Renderer::rendererInit(float windowWidth, float windowHeight) {
	shader = std::make_unique<Shader>("shaders/vertex.vert", "shaders/fragment.frag");
	gridShader = std::make_unique<Shader>("shaders/grid.vert", "shaders/grid.frag");
	gridInit();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::gridInit() {
	// Generate buffers
	glGenVertexArrays(1, &gridVAO);

}