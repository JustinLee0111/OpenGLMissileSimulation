#pragma once

#include <vector>
#include <object.h>

class PhysicsEngine {
public:
	// All physics objects stored here
	std::vector<Object*> physObjects;

	void update(float deltaTime);
	void addPhysObject(Object* obj) {
		if (obj && obj->physics) {
			physObjects.push_back(obj);
		}
	}
	// One of the objects is a plane
	// TODO: implement other object on object collision
	// TODO: implement angled collisions
	void planeCollision(Object* obj1, Object* obj2);
	void checkCollisions();
};