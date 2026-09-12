#pragma once

#include <glm/glm.hpp>

// Infrared Counter Counter Measure
enum class IRCCMType {
	MEMORY,
	TWO_COLOR,
	SPATIAL_GATING
};

enum class SeekerState {
	Scanning, // Nothing detected and waiting to detect object at default position
	Locking, // Missile is rotating gimbal to the object
	Resetting, // Seeker is returning to default position aligning with missile's boresight
	Tracking // Seeker has an active lock and is rotated on to the object
};

struct SeekerData { // Local coordinate space
	bool hasLock = false;
	bool tracking = false;

	float horizontalDiff = 0.0f;
	float verticalDiff = 0.0f;

	float targetTemperature{ 0.0f };

	glm::vec3 losRate{0.0f};
};

class World;
class Missile;

class MissileSeeker {
public:
	SeekerState curState = SeekerState::Scanning;

	//glm::vec3 lookDirection{ 0.0f, 0.0f, 1.0f }; // Local look direction

	float horizontalAngle = 0.0f; // Radians
	float verticalAngle = 0.0f; // Radians

	const float slewRate{ 0.0f };
	const float gimbalLimit{ 80.0f }; // In degrees, relative to missile boresight
	const float maxRange{ 100.0f };
	const float recenterDelay{ 0.0f };

	float angleFOV{ 45.0f }; // Degrees

	void update(const World& world, Missile& missile, float deltaTime);

	void updateSeekerState();

	void scan(const World& world, Missile& missile);

private:
	SeekerData curSeekerData;
};