#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Casts light in all directions, not uni directional
class Light {
	public:
		Light() = default;
		Light(glm::vec3 lightPos = glm::vec3{ 0.0f }, glm::vec4 lightColor = glm::vec4{ 1.0f }) :
			lightPos(lightPos), lightColor(lightColor){};

		void setLightColor(glm::vec4 lightColor){
			this->lightColor = lightColor;
		}
		glm::vec4 getLightColor() const {
			return lightColor;
		}
		glm::vec3 getLightPos() const {
			return lightPos;
		}
	private:
		glm::vec4 lightColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec3 lightPos{ 0.0f, 0.0f, 0.0f};
		glm::quat lightQ{ 1.0f, 0.0f, 0.0f, 0.0f }; // Not used currently, for uni directional lighting in the future
};