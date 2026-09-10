#pragma once

#include <memory>

#include "environment/Object.h"

enum class InfraredCounterMeasureType {
	MEMORY,
	TWO_COLOR,
	SPATIAL_GATING
};

struct SeekerData {
	Object* target{ nullptr };
	float targetTemperature{ 0.0f };
};

class World;
class Missile;

class MissileSeeker {
public:
	MissileSeeker() = default;

	glm::vec3 lookDirection{};

	const float slewRate{ 0.0f };
	const float gimbalLimit{ 0.0f };
	const float maxRange{ 0.0f };
	const float flareResistance{ 0.0f };
	const float recenterDelay{ 0.0f };
	float angleFOV{ 0.0f }; // Degrees

	void update(const World& world, const Missile& missile, float deltaTime);
	const SeekerData& getSeekerData() const;
private:
	SeekerData seekerData;
};