#pragma once

#include <vector>
#include <environment/Object.h>

struct HitData {
	Object* objectHit = nullptr;
	Object* objectHitter = nullptr;
	float distance = 0.0f;
	float timeToHit = -1.0f;
	glm::vec3 relativeVelocity{ 0.0f };
	glm::vec3 hitPoint{ 0.0f }; // Not used but could be useful for dynamic decals
	glm::vec3 normal{ 0.0f };
};

class PhysicsEngine {
public:
	// All physics objects pointers stored here for easier tracking and quicker updating
	// Actual objects are managed by World
	std::vector<Object*> physObjects;

	glm::vec3 gravity = { 0.0f, -9.81f, 0.0f };
	const float threshold = 0.00001f; // Threshold for floating point errors and small calculation deviations
	const float restingThreshold = 0.5f; // Threshold for speeds to start resting

	void update();
	void kinematicUpdater(float timeLeft);
	void positionUpdater(float hitTime);
	void forcesUpdater(float deltaTime);
	void forcesReset();
	std::vector<HitData> earliestCollision(float deltaTime);
	void addPhysObject(Object* obj) {
		if (obj && obj->physicsProperties) {
			physObjects.push_back(obj);
		}
	}
	void planeCollision(HitData& hitData);
	void sphereCollision(HitData& hitData);
	HitData planeCollisionData(Object& sphere, Object& plane, float timeLeft); // CCD implemented here
	HitData sphereCollisionData(Object& sphere1, Object& sphere2, float timeLeft); // CCD implemented here
	void resolveCollisions(HitData& hitData); // Resolves collision once simulation is advanced to the collision state
	float totalEnergy(); // For debugging conservation of energy when restitutions are 1.0f
	static float getPhysicsRate();
private:
	static const float deltaTime;
};