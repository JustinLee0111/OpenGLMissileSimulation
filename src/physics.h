#pragma once

#include <glm/glm.hpp>

class Object;

class Physics{
public:
	float mass{ 1.0f };
	glm::vec3 velocity{ 0.0f };
	bool enableGravity = true;
	bool enableCollisions = true;
	glm::vec3 totalForces{ 0.0f };
	float gravity = -9.81f;

	void addForce(const glm::vec3& force);
};