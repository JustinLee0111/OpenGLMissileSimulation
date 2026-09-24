#include <environment/Flares.h>
#include <environment/Object.h>

#include <iostream>

void Flares::spawnDirectionalParticle(std::mt19937& gen, glm::vec3 origin, glm::vec3 direction, glm::vec3 initVel) {
	for (auto& particle : particles) { // Goes through the particle array to find a ready particle to be spawned
		if (!particle.active) {
			// Adds variance in direction of particle emission
			float xRand = std::uniform_real_distribution<float>{ -velVariance, velVariance }(gen);
			float yRand = std::uniform_real_distribution<float>{ -velVariance, velVariance }(gen);
			float zRand = std::uniform_real_distribution<float>{ -velVariance, velVariance }(gen);

			particle.position = origin;
			particle.velocity = (direction + glm::vec3{ xRand, yRand, zRand }) * deploySpeed - initVel;
			particle.remainingTime = lifeTime;
			particle.particleTemp.temperature = 1000.0f;
			particle.active = true;
			break;
		}
	}
}

void Flares::deployFlares(std::mt19937& gen, glm::vec3 origin, glm::vec3 direction, glm::vec3 initVel) { // This sets the origin and direction for particles to be emitted from
	spawnDirectionalParticle(gen, origin, direction, initVel);
}