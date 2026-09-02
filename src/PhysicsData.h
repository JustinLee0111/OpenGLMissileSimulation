#pragma once

#include <glm/glm.hpp>

enum class ColliderType{Plane, Box, Sphere};

struct PhysicsData{
	// Object Properties
	float mass{ 1.0f };
	float restitution = 0.7f;
	float radius = 1.0f;
	float planeWidth = 2.0f;
	float planeHeight = 2.0f;

	// Simulation Bools
	bool enableGravity = true;
	bool enableCollisions = true;
	bool isStatic = true;
	bool isGrounded = false;
	bool isKinematic = false;

	// Physics Vectors
	glm::vec3 velocity{ 0.0f };
	glm::vec3 totalForces{ 0.0f };

	ColliderType collider = ColliderType::Sphere;

	// Straight movements only
	glm::vec3 moveDirection{ 0.0f, 1.0f, 0.0f };
	float moveDistance = 2.0f;
	float moveSpeed = 3.0f;
	float accumulatedTime = 0.0f;

	PhysicsData(bool enableGravity, bool enableCollisions, bool isKinematic) :
		enableGravity(enableGravity),
		enableCollisions(enableCollisions),
		isKinematic(isKinematic){}

	PhysicsData() = default;

	void addForce(const glm::vec3& force) {
		totalForces += force;
	}
	void zeroForces() {
		totalForces = glm::vec3{ 0.0f };
	}
};