#include <glm/glm.hpp>

#include "physics.h"

Physics::Physics(bool enableGravity, bool enableCollisions, bool isKinematic) {
	this->enableGravity = enableGravity;
	this->enableCollisions = enableCollisions;
	this->isKinematic = isKinematic;
}

void Physics::addForce(const glm::vec3& force) {
	totalForces += force;
}