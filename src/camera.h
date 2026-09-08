#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
	public:
		float sensitivity = 0.15f;

		// Camera default setup
		glm::vec3 cameraPos{ 0.0f, 0.0f, 0.0f };
		glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		float cameraFOV = 45.0f;
		float closePlane = 0.1f;
		float farPlane = 1000.0f;

		Camera() = default;
		Camera(glm::vec3 cameraPosition = glm::vec3{ 0.0f, 0.0f, 0.0f }) : cameraPos(cameraPosition) {};

		glm::mat4 getViewMatrix();
		glm::mat4 getProjectionMatrix(float aspectRatio);

		void cameraRotate(float xDelta, float yDelta); // xDelta and yDelta is treated as angles
};