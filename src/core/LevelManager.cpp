#include <nlohmann/json.hpp>
#include <fstream>

#include "environment/World.h"
#include "core/LevelManager.h"
#include "environment/camera.h"
#include "environment/Object.h"
#include "environment/PhysicsData.h"
#include "missile/Missile.h"

// Dynamic level loading using JSON level data
void LevelManager::loadLevel(const std::string& filepath, World& world) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		throw std::runtime_error("Invalid file. Check file path: " + filepath);
	}

	nlohmann::json levelData;
	file >> levelData;
	for (auto camera : levelData["cameras"]) {
		Camera* tempCamera = world.createCamera();
		auto position = camera["position"];
		float yaw = camera["yaw"];
		float pitch = camera["pitch"];
		tempCamera->cameraPos = glm::vec3{ position[0].get<float>(), position[1].get<float>(), position[2].get<float>() };;
		tempCamera->cameraRotate(yaw, pitch);
	}
	for (auto object : levelData["objects"]) {
		Object* spawnedObject;
		if (object["objectName"] == "missile") {
			spawnedObject = world.spawnMissile(object["modelPath"]);
		}
		else {
			spawnedObject = world.spawnObject(object["modelPath"]);
		}		
		if (object.contains("gravity")) {
			spawnedObject->physicsProperties->enableGravity = object["gravity"];
		}
		if (object.contains("kinematic")) {
			spawnedObject->physicsProperties->isKinematic = object["kinematic"];
		}
		if (object.contains("isStatic")) {
			spawnedObject->physicsProperties->isStatic = object["isStatic"];
		}
		if (object.contains("enableCollisions")) {
			spawnedObject->physicsProperties->enableCollisions = object["enableCollisions"];
		}
		if (object.contains("restitution")){
			spawnedObject->physicsProperties->restitution = object["restitution"];
		}
		auto position = object["position"];
		spawnedObject->position = glm::vec3{ position[0].get<float>(), position[1].get<float>(), position[2].get<float>() };
		
		if (object.contains("rotation")) {
			auto rotation = object["rotation"];
			auto axis = rotation["axis"];
			glm::vec3 rotationAxis{ axis[0].get<float>(), axis[1].get<float>(), axis[2].get<float>() };

			spawnedObject->rotate(rotationAxis, rotation["angle"]);
		}
		std::string tempCollider = object["collider"];
		if (tempCollider == "Sphere") {
			spawnedObject->physicsProperties->collider = ColliderType::Sphere;
		}
		else if (tempCollider == "Plane") {
			spawnedObject->physicsProperties->collider = ColliderType::Plane;
		}
	}
}