#pragma once

#include <vector>
#include <object.h>

struct HitData {
	Object* objectHit = nullptr;
	Object* objectHitter = nullptr;
	float distance = 0.0f;
	float timeToHit = -1.0f;
	glm::vec3 relativeVelocity{ 0.0f };
	glm::vec3 hitPoint{ 0.0f };
	glm::vec3 normal{ 0.0f };
};

class PhysicsEngine {
public:
	// All physics objects pointers stored here for easier tracking and quicker updating
	// Actual objects are managed by World
	std::vector<Object*> physObjects;

	glm::vec3 gravity = { 0.0f, -9.81f, 0.0f };

	void update();
	void kinematicUpdater(float deltaTime);
	void positionUpdater(float hitTime);
	void forcesUpdater(float deltaTime);
	void forcesReset();
	HitData earliestCollision(float deltaTime);
	void addPhysObject(Object* obj) {
		if (obj && obj->physicsProperties) {
			physObjects.push_back(obj);
		}
	}
	void planeCollision(HitData& hitData);
	void sphereCollision(HitData& hitData);
	HitData planeCollisionData(Object& sphere, Object& plane, float timeLeft);
	HitData sphereCollisionData(Object& sphere1, Object& sphere2, float timeLeft);
	void resolveCollisions(HitData& hitData);
	float totalEnergy();
	float getPhysicsRate() const {
		return deltaTime;
	}
private:
	const float deltaTime = (1.0f / 60.0f);
};