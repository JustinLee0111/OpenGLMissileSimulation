#pragma once

#include <vector>
#include <memory>
#include <string>

#include "PhysicsEngine.h"
#include "camera.h"
#include "Light.h"

class InputHandler;
class Object;

struct environmentLightingSettings {
	glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	float ambientLightStrength{ 0.5f };
};

class World {
private:
	PhysicsEngine physicsEngine;
	environmentLightingSettings worldLightSettings; // Stores ambient lighting data currently
public:
	// All objects and cameras stored here
	std::vector<std::unique_ptr<Object>> objects;
	std::vector<std::unique_ptr<Camera>> cameras;
	std::vector<std::unique_ptr<Light>> lights;
	static Camera* currentCamera;

	World() = default;

	void worldInit();

	Object* spawnObject(const std::string& filepath, bool enablePhysics = false, bool enableGravity = true, bool enableCollisions = true, bool isKinematic = false);

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

	void loadLevel(const std::string& filepath);
	void unloadLevel();

	void update(const InputHandler& inputs);

	float getPhysicsRate() const{
		return physicsEngine.getPhysicsRate();
	}
};