#pragma once

struct ParticleData {
	ParticleData() = default;
	glm::vec3 position{ 0.0f };
	glm::vec3 velocity{ 0.0f };
	
	float particleTemp{ 600.0f };
	//glm::vec4 color;

	float remainingTime = 0.0f;

	bool active = false;
};