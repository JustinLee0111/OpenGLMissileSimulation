#include <nlohmann/json.hpp>
#include <fstream>

#include <World.h>
#include "LevelManager.h"
#include "camera.h"
#include "Object.h"
#include "PhysicsData.h"

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
		tempCamera = nullptr;
	}
	for (auto object : levelData["objects"]) {
		std::string tempString = object["modelPath"];
		Object* tempObject = world.spawnObject(tempString, true);
		if (object.contains("gravity")) {
			tempObject->physicsProperties->enableGravity = object["gravity"];
		}
		auto position = object["position"];
		tempObject->position = glm::vec3{ position[0].get<float>(), position[1].get<float>(), position[2].get<float>() };
		
		std::string tempCollider = object["collider"];
		if (tempCollider == "Sphere") {
			tempObject->physicsProperties->collider = ColliderType::Sphere;
		}
		else if (tempCollider == "Plane") {
			tempObject->physicsProperties->collider = ColliderType::Plane;
			auto rotation = object["rotation"];
			auto axis = rotation["axis"];
			glm::vec3 rotationAxis{ axis[0].get<float>(), axis[1].get<float>(), axis[2].get<float>() };

			tempObject->rotate(rotationAxis,rotation["angle"]);
		}
		tempObject->physicsProperties->restitution = object["restitution"];
		tempObject = nullptr;
	}
}