#pragma once

#include <vector>
#include <memory>
#include <string>

#include "environment/ParticleBucket.h"
#include "core/ParticleSystem.h"
#include "core/PhysicsEngine.h"
#include "environment/camera.h"
#include "environment/Light.h"
#include "missile/Missile.h"
#include "environment/Flares.h"

class InputHandler;
class Object;

struct environmentLightingSettings {
	glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	float ambientLightStrength{ 0.5f };
};

class World {
private:
	PhysicsEngine physicsEngine;
	ParticleSystem particleSystem;
	environmentLightingSettings worldLightSettings; // Stores ambient lighting data currently
public:
	// All objects and cameras stored here
	std::vector<std::unique_ptr<Object>> objects;
	std::vector<std::unique_ptr<Camera>> cameras;
	std::vector<std::unique_ptr<Light>> lights;
	std::vector<Missile*> missiles;
	std::vector<std::unique_ptr<ParticleBucket>> particleBuckets;
	std::unique_ptr<Flares> flareBucket = nullptr;

	float airDensity = 1.0f; // kg/m^3

	float ambientTemp = 23.0f; // Temp in celcius

	static Camera* currentCamera;

	World() = default;

	void worldInit();

	Object* spawnObject(const std::string& filepath);
	Missile* spawnMissile(const std::string& filepath);

	//Object* spawnObject(std::string name, VertexData& vertexData);

	Camera* createCamera(glm::vec3 cameraPosition = glm::vec3{0.0f});

	Light* createLight(glm::vec3 lightPosition = glm::vec3{ 0.0f }, glm::vec4 lightColor = glm::vec4{ 1.0f });

	void setAmbientLightColor(glm::vec4 ambientLightColor){
		worldLightSettings.ambientLightColor = ambientLightColor;
	}
	void setAmbientLightStrength(float ambientLightStrength) {
		worldLightSettings.ambientLightStrength = ambientLightStrength;
	}
	const glm::vec4 getAmbientLightColor() const {
		return worldLightSettings.ambientLightColor;
	}
	const float getAmbientLightStrength() const {
		return worldLightSettings.ambientLightStrength;
	}

	ParticleBucket* getParticleBucket(std::string bucketName);

	void loadLevel(const std::string& filepath);
	void unloadLevel();

	void fixedUpdate(std::mt19937& gen);
	void update();

	void updateParticles(float deltaTime);

	const ParticleSystem& getParticleSystem() const{
		return particleSystem;
	}

	float getPhysicsRate() const{
		return physicsEngine.getPhysicsRate();
	}

	void deleteObj(std::string deleteName);
};