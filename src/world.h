#pragma once

#include <vector>
#include <memory>
#include <string>

#include "PhysicsEngine.h"
#include "camera.h"

class InputHandler;
class Object;

class World {
private:
	PhysicsEngine physicsEngine;
public:
	// All objects and cameras stored here
	std::vector<std::unique_ptr<Object>> objects;
	std::vector<std::unique_ptr<Camera>> cameras;
	static Camera* currentCamera;
	World() = default;

	Object* spawnObject(const std::string& filepath, bool enablePhysics = false, bool enableGravity = true, bool enableCollisions = true, bool isKinematic = false);

	Camera* createCamera();

	void loadLevel(const std::string& filepath);

	void unloadLevel();

	void update(const InputHandler& inputs);

	float getPhysicsRate() const{
		return physicsEngine.getPhysicsRate();
	}
};