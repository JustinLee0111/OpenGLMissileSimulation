#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Object.h"
#include "Model.h"

void Object::addPhysics(bool enablePhysics, bool enableCollisions, bool isKinematic) {
	if (!physicsProperties) physicsProperties = std::make_unique<PhysicsData>(enablePhysics, enableCollisions, isKinematic);
}

glm::mat4 Object::getObjectMatrix() const {
	return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotationQ);
}

void Object::draw() const {
	model->draw();
}