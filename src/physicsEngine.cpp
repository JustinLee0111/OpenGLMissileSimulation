#include "physicsEngine.h"
#include "object.h"
#include "physics.h"

void physicsEngine::update(std::vector<Object>& objects, float deltaTime) {
	for (Object& object : objects) {
		if (object.physics) {
			// Avoid division by zero or negative mass
			if (object.physics->mass <= 0.0f) {
				return;
			}
			if (object.physics->enableGravity) {
				object.physics->addForce({ 0.0f, object.physics->mass * object.physics->gravity, 0.0f });
			}
			glm::vec3 acceleration = object.physics->totalForces / object.physics->mass;
			object.physics->velocity += acceleration * deltaTime;
			object.position += object.physics->velocity * deltaTime;
			object.physics->totalForces = glm::vec3{ 0.0f };
		}		
	}
}