#include "core/ParticleSystem.h"
#include "environment/World.h"
#include "rendering/Model.h"
#include "rendering/Shader.h"

#include <iostream>
#include <memory>

void ParticleSystem::init() {
	particleShader = std::make_unique<Shader>("shaders/particles.vert", "shaders/particles.frag");
	particleShader->use();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Independent rendering/drawing for particles only
// Particles don't have a model matrix but are "billboards" that face the camera
// Separate draw and shaders to do billboards
void ParticleSystem::draw(World& world, float aspectRatio) const{
	particleShader->use();

	glDepthMask(GL_FALSE); // For alpha in the color vec4 if smoke was to be transparent
	glEnable(GL_BLEND); // For alpha in the color vec4 if smoke was to be transparent

	const glm::mat4 view = world.currentCamera->getViewMatrix();
	const glm::mat4 projection = world.currentCamera->getProjectionMatrix(aspectRatio);

	particleShader->setMat4("view", view);
	particleShader->setMat4("projection", projection);

	for (auto& particleBucket : world.particleBuckets) {
		particleShader->setVec3("scale", particleBucket->scale);
		for (int i = 0; i < particleBucket->particles.size(); i++) {
			if (particleBucket->particles[i].active) {
				particleShader->setVec3("position", particleBucket->particles[i].position);
				particleBucket->getModel().drawOpaque();
			}
		}
	}

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void ParticleSystem::updateParticles(std::vector<std::unique_ptr<ParticleBucket>>& buckets, float deltaTime) {
	for (auto& particleBucket : buckets) {
		if (particleBucket->emissionType == EmitType::UNIDIRECTIONAL) {
			particleBucket->spawnDirectionalParticle(deltaTime, gen);
		}
		else {
			particleBucket->spawnOmniDirectionalParticle(gen); // Hardcoded 360 particle emission on the same frame
		}

		for (auto& particle : particleBucket->particles) { // Moves the active particles based on it's velocity
			if (!particle.active) { continue; }

			particle.remainingTime -= deltaTime;

			if (particle.remainingTime <= 0.0f) {
				particle.active = false;
				continue;
			}
			particle.position += particle.velocity * deltaTime;
		}
	}
}