#pragma once

#include "environment/Temperature.h"

struct FlareData : public ParticleData {
	FlareData() = default;
	glm::vec3 position{ 0.0f };
	glm::vec3 velocity{ 0.0f };
	glm::vec4 color{ 1.0f, 0.4f, 0.0f, 1.0f };

	float remainingTime = 0.0f;

	bool active = false;
	Temperature particleTemp;
};