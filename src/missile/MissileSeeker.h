#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>

// Infrared Counter Counter Measure
enum class IRCCMType {
	MEMORY,
	TWO_COLOR,
	SPATIAL_GATING
};

enum class SeekerState {
	Scanning, // Nothing detected and waiting to detect object at default position
	Tracking, // Seeker has an active lock and is rotated on to the object
	Off // Seeker scanning off but can still transmit data, useful for IRCCM
};

struct SeekerData {
	bool tracking = false;

	glm::quat rotateToTarget{ 1.0f, 0.0f, 0.0f, 0.0f }; // How much to rotate current seeker quaternion by to point towards target

	float targetTemperature{ 0.0f };

	glm::vec3 oldLOStoTarget{ 0.0f };
	glm::vec3 LOStoTarget{ 0.0f };

	glm::vec3 losRate{ 0.0f }; // Line of Sight Rate, if this zero or near zero, it means current flight path is optimal for collision
};

class World;
class Missile;

class MissileSeeker {
public:
	SeekerState curState = SeekerState::Scanning;

	glm::quat seekerOrientation{ 1.0f, 0.0f, 0.0f, 0.0f }; // Local seeker quaternion

	glm::vec3 seekerAngVel{ 0.0f };

	const float maxSlewRate{ glm::radians(100.0f) }; // Radians per second
	const float gimbalLimit{ glm::pi<float>() / 2.0f}; // In radians, total gimbal range. NOT FROM MISSILE BORESIGHT
	const float maxRange{ 500.0f };
	const float recenterDelay{ 0.0f };

	float angleFOV{ glm::pi<float>() / 45.0f }; // Degrees

	void update(const World& world, Missile& missile, float deltaTime);
	void updateSeekerState();
	void clampGimbal(Missile& missile); // Clamps how much the fov can rotate based on gimbal limit
	void scan(const World& world, Missile& missile, float deltaTime); // Scans seeker FOV for any targets, need to create targeting priority for multiple targets

	const glm::vec3 getLOSrate() const{
		return curSeekerData.losRate;
	}

	void resetSeekerRot() {
		seekerOrientation = glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f };
	}

	void resetSeekerData() {
		curSeekerData.losRate = glm::vec3{ 0.0f };
		curSeekerData.oldLOStoTarget = glm::vec3{ 0.0f };
		curSeekerData.LOStoTarget = glm::vec3{ 0.0f };
		curSeekerData.losRate = glm::vec3{ 0.0f };
		curSeekerData.rotateToTarget = glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f };
	}

	const bool getSeekerOn() const {
		return seekerEnabled;
	}

	bool seekerEnabled = true;
private:
	SeekerData curSeekerData;
};