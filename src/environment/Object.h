#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

#include "environment/PhysicsData.h"

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

	glm::mat4 getObjectMatrix() const;

	void rotate(glm::vec3 axis, float angle) {
		glm::quat q = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
		rotationQ *= q;
	}
	void setRotation(glm::vec3 axis, float angle) {
		rotationQ = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
	}
	void addPhysics();
	void removePhysics() {
		if (physicsProperties) physicsProperties = nullptr;
	}

	void addChild(Object* child) {
		children.push_back(child);
		child->parent = this;
	}

	private:
		std::vector<Object*> children;
		Object* parent = nullptr;
};