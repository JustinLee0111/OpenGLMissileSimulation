#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>

// Infrared Counter Counter Measure
enum class IRCCMType {
	MEMORY,
	TWO_COLOR,
	SPATIAL_GATING
};

enum class SeekerState {
	Scanning, // Nothing detected and waiting to detect object at default position
	Resetting, // Seeker is returning to default position aligning with missile's boresight
	Tracking, // Seeker has an active lock and is rotated on to the object
	Off // Seeker scanning off but can still transmit data, useful for IRCCM
};

struct SeekerData {
	bool tracking = false;

	glm::quat rotateToTarget{ 1.0f, 0.0f, 0.0f, 0.0f }; // How much to rotate current seeker quaternion by to point towards target

	float targetTemperature{ 0.0f };

	glm::vec3 losRate{0.0f};
};

class World;
class Missile;

class MissileSeeker {
public:
	SeekerState curState = SeekerState::Scanning;

	glm::quat seekerOrientation{ 1.0f, 0.0f, 0.0f, 0.0f }; // Local seeker quaternion

	const float slewRate{ 0.0f };
	const float gimbalLimit{ glm::pi<float>() / 2.0f}; // In radians, total gimbal range. NOT FROM MISSILE BORESIGHT
	const float maxRange{ 100.0f };
	const float recenterDelay{ 0.0f };

	float angleFOV{ glm::pi<float>() / 45.0f }; // Degrees

	void update(const World& world, Missile& missile, float deltaTime);

	void updateSeekerState();

	void clampGimbal(Missile& missile);

	void scan(const World& world, Missile& missile);

	const bool getSeekerOn() const {
		return seekerEnabled;
	}

	bool seekerEnabled = true;
private:
	SeekerData curSeekerData;
};