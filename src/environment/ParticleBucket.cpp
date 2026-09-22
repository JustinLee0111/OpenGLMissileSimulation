#include "environment/ParticleBucket.h"

void ParticleBucket::spawnDirectionalParticle(float deltaTime, std::mt19937& gen) {
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
				particle.velocity = (direction + glm::vec3{ xRand, yRand, zRand }) * particleSpeed;
				particle.remainingTime = lifeTime;
				particle.active = true;

				break;
			}
		}
	}
}

// For emitting particles from a single point in all directions
// Spawns all particles in the bucket in the same frame
void ParticleBucket::spawnOmniDirectionalParticle(std::mt19937& gen) {
	if (IsEmitting) { // If the particles are being emitted, find the next particle to spawn
		for (auto& particle : particles) { // Goes through the particle array to find a ready particle to be spawned
			if (!particle.active) {

				// Adds variance in direction of particle emission
				// Hardcoded for 360 coverage
				float xRand = std::uniform_real_distribution<float>{ -1.0f, 1.0f }(gen);
				float yRand = std::uniform_real_distribution<float>{ -1.0f, 1.0f }(gen);
				float zRand = std::uniform_real_distribution<float>{ -1.0f, 1.0f }(gen);

				glm::vec3 emitDir = glm::normalize( glm::vec3{ xRand, yRand, zRand } );

				particle.position = origin;
				particle.velocity = emitDir * particleSpeed;
				particle.remainingTime = lifeTime;
				particle.active = true;
			}
		}
		IsEmitting = false;
	}
}

void ParticleBucket::emitParticles(glm::vec3 origin, glm::vec3 direction) { // This sets the origin and direction for particles to be emitted from
	this->origin = origin;
	this->direction = direction;
	IsEmitting = true; // This tells the particle system to start emitting particles
}

bool ParticleBucket::checkActive() { // Currently unused, not sure why I made this to be honest
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i].active) {
			return true;
		}
	}
	return false;
}

void ParticleBucket::stopEmit() { // Stops emitting new particles
	IsEmitting = false;
}