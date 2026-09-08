#include <glm/gtc/quaternion.hpp>

#include "camera.h"

// Updates the camera rotation based on X and Y angles
void Camera::cameraRotate(float xAngle, float yAngle) {
	glm::quat yaw = glm::angleAxis(glm::radians(xAngle), glm::vec3{ 0.0f, 1.0f ,0.0f });
	glm::quat pitch = glm::angleAxis(glm::radians(yAngle), glm::vec3{ 1.0f, 0.0f, 0.0f });
	rotation = glm::normalize(yaw * rotation * pitch);
}
glm::mat4 Camera::getViewMatrix() {
	return glm::mat4_cast(glm::conjugate(rotation)) * glm::translate(glm::mat4(1.0f), -cameraPos);
}
glm::mat4 Camera::getProjectionMatrix(float aspectRatio) {
	return glm::perspective(glm::radians(cameraFOV), aspectRatio, closePlane, farPlane);
}