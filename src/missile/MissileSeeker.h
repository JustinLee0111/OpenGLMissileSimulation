#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>

class World;
class Missile;
class Object;
class FlareData;

enum class SeekerState {
	Scanning, // Nothing detected and waiting to detect object at default position
	Tracking, // Seeker has an active lock and is rotated on to the object
	Memory,
	MemoryRelock,
	Off // Seeker scanning off but can still transmit data, useful for IRCCM
};

struct SeekerData {
	bool tracking = false;
	Object* trackingObj = nullptr;
	FlareData* trackingFlare = nullptr;

	glm::quat rotateToTarget{ 1.0f, 0.0f, 0.0f, 0.0f }; // How much to rotate current seeker quaternion by to point towards target

	float targetTemperature{ 0.0f };

	float seekerAngleToTarget{ 0.0f };

	glm::vec3 oldLOStoTarget{ 0.0f };
	glm::vec3 LOStoTarget{ 0.0f };

	glm::vec3 losRate{ 0.0f }; // Line of Sight Rate, if this zero or near zero, it means current flight path is optimal for collision
};

class MissileSeeker {
public:
	SeekerState curState = SeekerState::Scanning;

	glm::quat seekerOrientation{ 1.0f, 0.0f, 0.0f, 0.0f }; // Local seeker quaternion

	const float gimbalLimit{ glm::pi<float>() / 1.1f}; // In radians, total gimbal range. NOT FROM MISSILE BORESIGHT
	const float maxRange{ 10000.0f };
	const float tempThreshold{ 300.0f };

	void update(const World& world, Missile& missile, float deltaTime);
	void lateUpdate(const World& world, Missile& missile, float deltaTime);
	void updateSeekerState();
	void clampGimbal(Missile& missile); // Clamps how much the fov can rotate based on gimbal limit
	void scanFOV(const World& world, Missile& missile, float deltaTime); // Scans seeker FOV for any targets, need to create targeting priority for multiple targets
	SeekerData scanFlares(const World& world, Missile& missile, float deltaTime);
	SeekerData scanObjects(const World& world, Missile& missile, float deltaTime);
	void updateSeekerAngle(Missile& missile);
	void updateScanData(Missile& missile, float deltaTime);

	void findRandomTarget(World& world, Missile& missile); // Locks a random target within the gimbal limit, kind of like helmet cue system

	void printState();

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

	const float getSeekerAngle() const {
		return seekerAngle;
	}

	const float getSeekerFOV() const {
		return angleFOV;
	}

	const bool getMemoryFlight() const {
		return memoryFlight;
	}

	const bool getSeekerOn() const {
		return seekerEnabled;
	}

	const bool getIsTracking() const{
		return curSeekerData.tracking;
	}

	const SeekerData getSeekerData() const{
		return curSeekerData;
	}

	bool seekerEnabled = true;
private:
	// Infrared Counter Counter Measure (IRCCM) bools
	float timeSinceLock{ 0.0f };
	float relockTime{ 0.0f };
	float angleFOV{ glm::pi<float>() / 45.0f }; // Radians for seeker's instant FOV
	float seekerAngle{ 0.0f };
	float spatialGateDelayTimer{ 0.0f };

	bool memoryFlight{ false };

	const bool spatialGating{ true };
	const bool memoryIRCCM{ true };
	const float irccmTempDiffThreshold{ 200.0f }; // Threshold for if the locked target temperature difference from memory target temp is greater than threshold
	const float memoryTimeout{ 1.0f };
	const float relockTimeout{ 2.0f };
	const float spatialGateDelay{ 0.25f };

	const float scanningFOV{ glm::pi<float>() / 45.0f };
	const float spatialGatedFOV{ glm::pi<float>() / 720.0f }; // Radians for seeker's spatial gated FOV

	SeekerData curSeekerData;
	SeekerData seekerDataMemory;
};