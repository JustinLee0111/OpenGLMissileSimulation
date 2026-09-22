#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "environment/Object.h"

class Camera {
	public:
		float sensitivity = 0.15f;

		// Camera default setup
		glm::vec3 cameraPos{ 0.0f, 0.0f, 0.0f };
		glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		float cameraFOV = 35.0f;
		float closePlane = 0.005f;
		float farPlane = 50000.0f;

		Camera() = default;
		Camera(glm::vec3 cameraPosition = glm::vec3{ 0.0f, 0.0f, 0.0f }) : cameraPos(cameraPosition) {};

		glm::mat4 getViewMatrix();
		glm::mat4 getProjectionMatrix(float aspectRatio);

		void cameraRotate(float xDelta, float yDelta); // xDelta and yDelta is treated as angles

		bool chase = false;

		Object* chaseObject = nullptr;
};