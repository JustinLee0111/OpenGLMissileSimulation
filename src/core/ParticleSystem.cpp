#include "core/ParticleSystem.h"
#include "environment/World.h"
#include "rendering/Model.h"
#include "rendering/Shader.h"
#include "environment/ParticleBucket.h"
#include "environment/FlareData.h"
#include "environment/Flares.h"


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
// Extremely laggy with high amount of particles spawned, will optimize
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
				particleShader->setBool("useColor", true);
				particleShader->setVec4("color", particleBucket->particles[i].color);
				particleShader->setVec3("position", particleBucket->particles[i].position);
				particleBucket->getModel().drawOpaque();
			}
		}
	}
	particleShader->setVec3("scale", world.flareBucket->scale);
	for (int i = 0; i < world.flareBucket->particles.size(); i++) {
		if (world.flareBucket->particles[i].active) {
			particleShader->setBool("useColor", true);
			particleShader->setVec4("color", world.flareBucket->particles[i].color);
			particleShader->setVec3("position", world.flareBucket->particles[i].position);
			world.flareBucket->model->drawOpaque();
		}
	}

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void ParticleSystem::updateParticles(std::mt19937& gen, World& world, float deltaTime) {
	for (auto& particleBucket : world.particleBuckets) {
		particleBucket->spawnBucket(deltaTime, gen);
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
	for (auto& particle : world.flareBucket->particles) { // Moves the active particles based on it's velocity
		if (!particle.active) { continue; }

		particle.remainingTime -= deltaTime;
		particle.particleTemp.temperature -= (particle.particleTemp.tempTransferCoef * (particle.particleTemp.temperature - world.ambientTemp)) * deltaTime;

		if (particle.remainingTime <= 0.0f) {
			particle.active = false;
			continue;
		}
		particle.position += particle.velocity * deltaTime;
	}
}

void ParticleSystem::clearEmittingObjs(std::vector<std::unique_ptr<ParticleBucket>>& buckets) {
	for (auto& bucket : buckets) {
		bucket->clearObjs();
	}
}

void ParticleSystem::stopEmitting(std::vector<std::unique_ptr<ParticleBucket>>& buckets){
	for (auto& bucket : buckets) {
		bucket->stopEmit();
	}
}