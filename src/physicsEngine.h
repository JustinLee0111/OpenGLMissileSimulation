#pragma once

#include <vector>
#include <object.h>

class PhysicsEngine {
public:
	// All physics objects stored here
	std::vector<Object*> physObjects;

	float gravity = -9.81f;

	void update();
	void kinematicUpdater(float deltaTime);
	void positionUpdater(float hitTime, float deltaTime);
	void forcesUpdater(float deltaTime);
	float earliestCollision(float deltaTime);
	void addPhysObject(Object* obj) {
		if (obj && obj->physicsProperties) {
			physObjects.push_back(obj);
		}
	}
	// One of the objects is a plane
	// TODO: implement other object on object collision
	// TODO: implement mass collision calculations
	void planeCollision(Object* obj1, Object* obj2, const float& deltaTime);
	void resolveCollisions(const float& deltaTime);
	float getPhysicsRate() const {
		return deltaTime;
	}
private:
	const float deltaTime = (1.0f / 60.0f);
};