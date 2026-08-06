#include <glm/glm.hpp>

#include "physics.h"
#include "object.h"


void Physics::addForce(const glm::vec3& force) {
	totalForces += force;
}