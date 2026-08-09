#pragma once

#include <glm/glm.hpp>

enum class BoundingBox{Plane, Box, Sphere};

class Physics{
public:
	Physics(bool enableGravity, bool enableCollisions, bool isKinematic) {
		this->enableGravity = enableGravity;
		this->enableCollisions = enableCollisions;
		this->isKinematic = isKinematic;
	}
	float mass{ 1.0f };
	float restitution = 0.7f;
	float radius = 1.0f;

	bool enableGravity = true;
	bool enableCollisions = true;
	bool isGrounded = false;
	bool isKinematic = false;

	glm::vec3 velocity{ 0.0f };
	glm::vec3 totalForces{ 0.0f };

	float gravity = -9.81f;

	BoundingBox collider = BoundingBox::Sphere;

	// Straight movements only
	glm::vec3 moveDirection{ 0.0f, 1.0f, 0.0f };
	float moveDistance = 2.0f;
	float moveSpeed = 3.0f;
	float accumulatedTime = 0.0f;

	void addForce(const glm::vec3& force) {
		totalForces += force;
	}
};