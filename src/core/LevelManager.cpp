#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

#include "environment/World.h"
#include "core/LevelManager.h"
#include "environment/camera.h"
#include "environment/Object.h"
#include "environment/PhysicsData.h"
#include "core/AssetManager.h"

// Dynamic level loading using JSON level data
// return meaning, -1 = failed to open file, 0 = success loading level, 1 = already loaded level
int LevelManager::loadLevel(const int levelIndex, World& world) {
	std::string filePath;
	for (auto& it : levelMap) {
		if (it.first == levelIndex) {
			filePath = it.second;
			currentLevel = it.first;
			unloadLevel(world);
			break;
		}
	}
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cout << ("Invalid file. Check file path: " + filePath) << std::endl;
		return -1;
	}
	nlohmann::json levelData;
	file >> levelData;
	for (auto object : levelData["objects"]) {
		Object* spawnedObject;
		if (object["objectName"] == "missile") {
			spawnedObject = world.spawnMissile(object["modelPath"]);
			spawnedObject->objName = object["objectName"];
		}
		else {
			spawnedObject = world.spawnObject(object["modelPath"]);
			spawnedObject->objName = object["objectName"];
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
		if (object.contains("scale")) {
			spawnedObject->scale = glm::vec3{ object["scale"][0], object["scale"][1], object["scale"][2] };
		}
		if (object.contains("mass")) {
			spawnedObject->physicsProperties->mass = object["mass"];
		}
		if (object.contains("color")) {
			spawnedObject->color = glm::vec4{ object["color"][0], object["color"][1], object["color"][2], object["color"][3] };
		}
		if (object.contains("rotation")) {
			auto rotation = object["rotation"];
			auto axis = rotation["axis"];
			glm::vec3 rotationAxis{ axis[0].get<float>(), axis[1].get<float>(), axis[2].get<float>() };

			spawnedObject->rotate(rotationAxis, rotation["angle"]);
		}
		if (object.contains("velocity")) {
			spawnedObject->physicsProperties->velocity = spawnedObject->rotationQ * glm::vec3{ object["velocity"][0], object["velocity"][1], object["velocity"][2] };
		}
		auto position = object["position"];
		spawnedObject->position = glm::vec3{ position[0].get<float>(), position[1].get<float>(), position[2].get<float>() };
		
		std::string tempCollider = object["collider"];
		if (tempCollider == "Sphere") {
			spawnedObject->physicsProperties->collider = ColliderType::Sphere;
		}
		else if (tempCollider == "Plane") {
			spawnedObject->physicsProperties->collider = ColliderType::Plane;
		}
	}
	for (auto camera : levelData["cameras"]) {
		Camera* tempCamera = world.createCamera();
		auto position = camera["position"];
		float yaw = camera["yaw"];
		float pitch = camera["pitch"];
		tempCamera->cameraPos = glm::vec3{ position[0].get<float>(), position[1].get<float>(), position[2].get<float>() };
		tempCamera->cameraRotate(yaw, pitch);
		if (camera.contains("chase")) {
			tempCamera->chase = camera["chase"];
		}
	}
	reloadLevel = false;
	return 0;
}

void LevelManager::unloadLevel(World& world) {
	world.clearParticleSystem();
	world.objects.clear();
	world.cameras.clear();
	world.missiles.clear();
	world.clearPhysObj();
	AssetManager::clearCache();
	world.currentCamera = nullptr;
	for (auto& camera : world.cameras) {
		if (camera->chase) {
			camera->chaseObject = nullptr;
		}
	}
}