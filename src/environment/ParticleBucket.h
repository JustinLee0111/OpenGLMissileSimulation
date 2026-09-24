#pragma once

#include <glm/glm.hpp>
#include <array>
#include <random>
#include <memory>
#include <string>
#include <vector>

#include "rendering/Model.h"
#include "environment/ParticleData.h"

class Object;

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
	void emitParticles(Object* obj, glm::vec3 direction = glm::vec3{ 0.0f });

	void spawnDirectionalParticle(float deltaTime, std::mt19937& gen, glm::vec3 initVel = glm::vec3{ 0.0f });
	void spawnOmniDirectionalParticle(std::mt19937& gen);

	void spawnBucket(float deltaTime, std::mt19937& gen);

	bool checkActive();

	void stopEmit();

	const Model& getModel() const{
		return *model;
	}

	void addObj(Object* obj) {
		emittingObjects.push_back(obj);
	}

	void clearObjs() {
		emittingObjects.clear();
	}

	std::vector<Object*>& getEmitObjs() {
		return emittingObjects;
	}


	std::string bucketName = {};

	EmitType emissionType{ EmitType::UNIDIRECTIONAL };

	glm::vec3 origin{ 0.0f };
	glm::vec3 direction{ 0.0f };
	glm::vec3 scale{ 0.1f, 0.1f, 0.1f };

	bool carryVelocity{ false }; // If particles should carry over origin object velocity
	glm::vec3 spawnVelocity{ 0.0f };

	float velVariance{ 0.05f };

	float particleSpeed{ 100.0f };
	float lifeTime{ 0.2f };
	float lastSpawnDeltaT{ 0.0f };

	std::array<ParticleData, 500> particles; // An array of particles, max of 500 particles at a given time

	bool IsEmitting = false;
	float spawnDelay = 0.0001f; // Delay between particle spawns in seconds

	Object* lastSpawnedObj = nullptr;
private:
	std::shared_ptr<Model> model = nullptr;
	std::vector<Object*> emittingObjects{};
};