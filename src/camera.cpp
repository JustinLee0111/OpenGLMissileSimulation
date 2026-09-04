#include <glm/gtc/quaternion.hpp>

#include "camera.h"

bool Camera::mouseControl = false;

// Updates the camera rotation based on X and Y angles
void Camera::cameraRotate(float xDelta, float yDelta) {
	glm::quat yaw = glm::angleAxis(glm::radians(xDelta), glm::vec3{ 0.0f, 1.0f ,0.0f });
	glm::quat pitch = glm::angleAxis(glm::radians(yDelta), glm::vec3{ 1.0f, 0.0f, 0.0f });
	rotation = glm::normalize(yaw * rotation * pitch);

	glm::mat4 viewMatrix = glm::mat4_cast(glm::conjugate(rotation));
	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), -cameraPos);

	view = viewMatrix * translateMatrix;
}