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

		Camera() = default;

		glm::mat4 getViewMatrix();
		glm::mat4 getProjectionMatrix(float aspectRatio);

		void cameraRotate(float xDelta, float yDelta); // xDelta and yDelta is treated as angles
};