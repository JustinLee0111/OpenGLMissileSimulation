#pragma once

#include <glm/glm.hpp>
#include <array>
#include <random>
#include <memory>
#include <string>

#include "rendering/Model.h"
#include "environment/ParticleData.h"

enum class EmitType{
	UNIDIRECTIONAL,
	OMNIDIRECTIONAL
};

class ParticleBucket {
public:
	ParticleBucket(std::string bucketName) : bucketName(bucketName) {
		model = std::make_shared<Model>("assets/models/sphere.obj");
	}

	void emitParticles(glm::vec3 origin, glm::vec3 direction = glm::vec3{ 0.0f });

	void spawnDirectionalParticle(float deltaTime, std::mt19937& gen);
	void spawnOmniDirectionalParticle(std::mt19937& gen);

	bool checkActive();

	void stopEmit();

	const Model& getModel() const{
		return *model;
	}

	std::string bucketName = {};

	EmitType emissionType{ EmitType::UNIDIRECTIONAL };

	glm::vec3 origin{ 0.0f };
	glm::vec3 direction{ 0.0f };

	float velVariance{ 0.05f };
	glm::vec3 scale{ 0.1f, 0.1f, 0.1f };

	float particleSpeed{ 100.0f };
	float lifeTime{ 0.2f };
	float lastSpawnDeltaT{ 0.0f };

	std::array<ParticleData, 500> particles; // An array of particles, max of 1000 particles at a given time

	bool IsEmitting = false;
	float spawnDelay = 0.00001f; // Delay between particle spawns in seconds
private:
	std::shared_ptr<Model> model = nullptr;
};