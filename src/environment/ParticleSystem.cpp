#include "environment/ParticleSystem.h"
#include "environment/World.h"

#include <iostream>
#include <random>

std::unique_ptr<Shader> ParticleSystem::particleShader = nullptr; 

void ParticleSystem::init() {
	particleShader = std::make_unique<Shader>("shaders/particles.vert", "shaders/particles.frag");
}

// Independent rendering/drawing for particles only
// Particles don't have a model matrix but are "billboards" that face the camera
void ParticleSystem::draw(World& world, float aspectRatio) {
	particleShader->use();
	const glm::mat4 view = world.currentCamera->getViewMatrix();
	const glm::mat4 projection = world.currentCamera->getProjectionMatrix(aspectRatio);
	particleShader->setMat4("view", view);
	particleShader->setMat4("projection", projection);
	particleShader->setVec3("scale", scale);
	glDepthMask(GL_FALSE); // For alpha in the color vec4 if smoke was to be transparent
	glEnable(GL_BLEND); // For alpha in the color vec4 if smoke was to be transparent
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i].active) {
			particleShader->setVec3("position", particles[i].position);
			model->drawOpaque();
		}
	}
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void ParticleSystem::spawnParticle(float deltaTime) {
	lastSpawnDeltaT += deltaTime;
	while (IsEmitting && lastSpawnDeltaT >= spawnDelay) { // While the particles are still being emitted and spawn cooldown is met, find the next particle to spawn
		lastSpawnDeltaT -= spawnDelay;
		for (auto& particle : particles) { // Goes through the particle array to find a ready particle to be spawned
			if (!particle.active) {
				lastSpawnDeltaT = 0.0f; // Resets the time since last spawn since this exact frame is when particle was spawned

				// Adds variance in direction of particle emission
				float xRand = std::uniform_real_distribution<float>{ -velVariance, velVariance }(gen);
				float yRand = std::uniform_real_distribution<float>{ -velVariance, velVariance }(gen);
				float zRand = std::uniform_real_distribution<float>{ -velVariance, velVariance }(gen);

				particle.position = origin;
				particle.velocity = ( direction + glm::vec3{ xRand, yRand, zRand } ) * particleSpeed;
				particle.remainingTime = lifeTime;
				particle.active = true;

				break;
			}
		}
	}
}

void ParticleSystem::updateParticles(float deltaTime) {
	spawnParticle(deltaTime);

	for (auto& particle : particles) { // Moves the active particles based on it's velocity
		if (!particle.active) { continue; }

		particle.remainingTime -= deltaTime;

		if (particle.remainingTime <= 0.0f) {
			particle.active = false;
			continue;
		}
		particle.position += particle.velocity * deltaTime;
	}
}

void ParticleSystem::emitParticles(glm::vec3 origin, glm::vec3 direction){ // This sets the origin and direction for particles to be emitted from
	this->origin = origin;
	this->direction = direction;
	IsEmitting = true; // This tells the particle system to start emitting particles
}

bool ParticleSystem::checkActive() { // Currently unused, not sure why I made this to be honest
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i].active) {
			return true;
		}
	}
	return false;
}

void ParticleSystem::stopEmit() { // Stops emitting new particles
	IsEmitting = false;
}