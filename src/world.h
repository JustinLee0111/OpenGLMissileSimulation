#pragma once

#include <vector>
#include <memory>

#include "PhysicsEngine.h"
#include "camera.h"
#include "Shader.h"

class Object;

class World {
private:
	PhysicsEngine physicsEngine;
	Shader shader;
public:
	// All objects and cameras stored here
	std::vector<std::unique_ptr<Object>> objects;
	std::vector<std::unique_ptr<Camera>> cameras;
	static Camera* currentCamera;

	World() : shader("shaders/vertex.vert", "shaders/fragment.frag"){}

	Object* spawnObject(const std::string& filepath, bool enablePhysics = false, bool enableGravity = true, bool enableCollisions = true, bool isKinematic = false);

	Camera* createCamera(float windowWidth, float windowHeight);

	void update();

	void draw() const;

	float getPhysicsRate() const{
		return physicsEngine.getPhysicsRate();
	}
};