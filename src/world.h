#pragma once

#include <vector>
#include <memory>

#include "physicsEngine.h"

class Object;

class World {
private:
	PhysicsEngine physicsEngine;
public:
	// All objects stored here
	std::vector<std::unique_ptr<Object>> objects;

	Object* spawnObject(const std::string& filepath, bool enablePhysics = false, bool enableGravity = true, bool enableCollisions = true, bool isKinematic = false) {
		auto obj = std::make_unique<Object>(filepath);
		Object* ptr = obj.get();

		if (enablePhysics) {
			ptr->addPhysics(enableGravity, enableCollisions, isKinematic);
			physicsEngine.addPhysObject(ptr);
		}

		objects.push_back(std::move(obj));
		return ptr;
	}

	void update(float deltaTime) {
		physicsEngine.update(deltaTime);
	}

	void draw(unsigned int modelLocation) const {
		//std::cout << "--World Objects--" << std::endl;
		for (const auto& obj : objects) {
			//std::cout << "Grounded: " << obj->physics->isGrounded << std::endl;
			//std::cout << "Kinematic: " << obj->physics->isKinematic << std::endl;
			//std::cout << "Object X: " << obj->position.x << std::endl;
			//std::cout << "Object Y: " << obj->position.y << std::endl;
			//std::cout << "Object Z: " << obj->position.z << std::endl;
			//std::cout << "Object Y Vel: " << obj->physics->velocity.y << std::endl;
			//std::cout << std::endl;
			obj->draw(modelLocation);
		}
	}

	// Registers the object with physics engine
	void createPhysObject(Object* obj) {
		physicsEngine.addPhysObject(obj);
	}
};