#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "environment/Object.h"

void Object::addPhysics() {
	if (!physicsProperties) physicsProperties = std::make_unique<PhysicsData>();
}

glm::mat4 Object::getObjectMatrix() const {
	if (parent) {
		glm::mat4 child = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotationQ);
		return parent->getObjectMatrix() * child;
	}
	else {
		return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotationQ);
	}
}