#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
	public:
		float windowWidth;
		float windowHeight;
		float aspectRatio;
		float sensitivity = 0.15f;

		// Camera default setup
		glm::mat4 view{ 1.0f };
		glm::mat4 projection{ 1.0f };
		glm::vec3 cameraPos{ 0.0f, 0.0f, 0.0f };
		glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };

		Camera(float windowWidth, float windowHeight) : windowWidth(windowWidth), windowHeight(windowHeight){
			aspectRatio = windowWidth / windowHeight;
			projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
			view = glm::mat4_cast(glm::conjugate(rotation));
		}
		void cameraRotate(float xDelta, float yDelta); // xDelta and yDelta is treated as angles
};