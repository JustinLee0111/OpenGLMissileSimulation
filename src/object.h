#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

#include "PhysicsData.h"

class Model;

class Object {
public:
	glm::vec3 position{ 0.0f };
	glm::quat rotationQ{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 scale{ 1.0f }; // Not implemented yet

	glm::vec3 left = rotationQ * glm::vec3{ -1.0f, 0.0f, 0.0f };
	glm::vec3 front = rotationQ * glm::vec3{ 0.0f, 0.0f, 1.0f };
	glm::vec3 up = rotationQ * glm::vec3{ 0.0f, 1.0f, 0.0f };
	glm::vec3 normal = rotationQ * glm::vec3{ 0.0f, 1.0f, 0.0f };

	std::unique_ptr<PhysicsData> physicsProperties;
	std::shared_ptr<Model> model;

	Object() = default;
	~Object() = default;

	void draw() const;
	glm::mat4 getObjectMatrix() const;

	void rotate(glm::vec3 axis, float angle) {
		glm::quat q = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
		rotationQ *= q;
	}
	void setRotation(glm::vec3 axis, float angle) {
		rotationQ = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
	}
	void addPhysics(bool enablePhysics = false, bool enableCollisions = true, bool isKinematic = false);
	void removePhysics() {
		if (physicsProperties) physicsProperties = nullptr;
	}
};