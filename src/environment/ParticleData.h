#pragma once

struct ParticleData {
	ParticleData() = default;
	glm::vec3 position{ 0.0f };
	glm::vec3 velocity{ 0.0f };

	glm::vec4 color{ 1.0f, 0.0f, 0.0f, 1.0f }; // Default red color

	float remainingTime = 0.0f;

	bool active = false;
};