#include <iostream>
#include <memory>

#include "missile/MissileSeeker.h"
#include "missile/Missile.h"
#include "environment/World.h"
#include "environment/Object.h"
#include "environment/Temperature.h"
#include "environment/Flares.h"

void MissileSeeker::update(const World& world, Missile& missile, float deltaTime){	
	//printState();
	if (seekerEnabled) {
		scanFOV(world, missile, deltaTime);
		updateSeekerState();

		if (curState == SeekerState::MemoryRelock) { // If the target was lost
			relockTime += deltaTime;
		}
	}
	else if (curState == SeekerState::Memory) { // If the target has deployed flares
		timeSinceLock += deltaTime;
		updateSeekerState();
	}
	else if(curState == SeekerState::Off){
		updateSeekerState();
		resetSeekerRot();
	}
	else {
		updateSeekerState();
	}
}

void MissileSeeker::lateUpdate(const World& world, Missile& missile, float deltaTime) {
	if (curState == SeekerState::Tracking) {
		seekerOrientation = glm::normalize(curSeekerData.rotateToTarget * seekerOrientation); // Updates the rotation for seeker to point towards target
		if (spatialGateDelayTimer >= spatialGateDelay) {
			angleFOV = spatialGatedFOV;
			spatialGateDelayTimer = 0.0f;
		}
		else {
			spatialGateDelayTimer += deltaTime;
		}
	}
	else {
		angleFOV = scanningFOV;
		spatialGateDelayTimer = 0.0f;
	}
	updateSeekerAngle(missile);
	clampGimbal(missile);
}

void MissileSeeker::updateSeekerState() {
	if (seekerEnabled) {
		if (curSeekerData.tracking) { // If seeker detects object/flare
			if (curSeekerData.targetTemperature - seekerDataMemory.targetTemperature > irccmTempDiffThreshold && seekerDataMemory.targetTemperature > 0.0f) { // Branch for flare detected by seeker
				curState = SeekerState::Memory;
				curSeekerData = seekerDataMemory;
				seekerEnabled = false;
				memoryFlight = true;
			}
			else { // If no flares detected, keep tracking object and reset memory flight flags
				curState = SeekerState::Tracking;
				relockTime = 0.0f;
				timeSinceLock = 0.0f;
				memoryFlight = false;
			}
		}
		else if (curState == SeekerState::MemoryRelock) { // If lock was lost, go off of memory and keep flying
			if (relockTime >= relockTimeout) {
				curState = SeekerState::Scanning;
				relockTime = 0.0f;
				memoryFlight = false;
				resetSeekerData();
				resetSeekerRot();
			}
			else {
				curSeekerData = seekerDataMemory;
				memoryFlight = true;
			}
		}
		else if (curSeekerData.tracking == false && curState == SeekerState::Tracking) { // If lock was lost, go off of memory and attempt relocking target
			curState = SeekerState::MemoryRelock;
			curSeekerData = seekerDataMemory;
			memoryFlight = true;
		}
		else{
			curState = SeekerState::Scanning;
			relockTime = 0.0f;
			timeSinceLock = 0.0f;
			memoryFlight = false;
		}
	}
	else {
		if (curState == SeekerState::Memory) { // If flare was detected, keep going off of memory with seeker off until timeout
			if (timeSinceLock >= memoryTimeout) {
				curState = SeekerState::MemoryRelock;
				curSeekerData.tracking = false;
				seekerEnabled = true;
				timeSinceLock = 0.0f;
			}
			else {
				curSeekerData = seekerDataMemory;
				memoryFlight = true;
			}
		}
		else { // If seeker is turned off manually
			curState = SeekerState::Off;
			curSeekerData.tracking = false;
		}
	}
}

// Clamps the gimbal limit
// Gimbal angle limit is total angle but seeker angles are relative to missile boresight
void MissileSeeker::clampGimbal(Missile& missile) { 
	glm::vec3 forward{ 0.0f, 0.0f, 1.0f };
	glm::quat identity{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 worldSeekerLook = glm::normalize(missile.rotationQ * seekerOrientation * forward); // Converts local to world space
	float halfGimbal = gimbalLimit / 2.0f; // Uses angle relative to missile boresight so must be half angle

	float angle = glm::acos(glm::clamp(glm::dot(missile.front, worldSeekerLook), 0.0f, 1.0f)); // Gets angle between missile boresight vector and seeker vector

	if (angle > halfGimbal) {
		float interpRatio = halfGimbal / angle; // Gets the percentage of the current orientation that would set it back within gimbal limit
		seekerOrientation = glm::slerp(identity, seekerOrientation, interpRatio); // Sets the seeker's quaternion from interp ratio from the missile's boresight
	}
}

void MissileSeeker::scanFOV(const World& world, Missile& missile, float deltaTime){
	SeekerData objScan = scanObjects(world, missile, deltaTime);
	SeekerData flareScan = scanFlares(world, missile, deltaTime);

	if (!memoryFlight) {
		seekerDataMemory = curSeekerData; // Store current seeker data in memory for memory IRCCM
		seekerDataMemory.tracking = false;
	}

	// If both flare and object in FOV, get the hottest thing instead
	if (objScan.targetTemperature < flareScan.targetTemperature) {
		curSeekerData = flareScan;
	}
	else {
		curSeekerData = objScan;
	}
}

SeekerData MissileSeeker::scanObjects(const World& world, Missile& missile, float deltaTime) {
	SeekerData scannedData;
	glm::vec3 forward{ 0.0f, 0.0f, 1.0f };
	glm::vec3 worldLookDirection = (missile.rotationQ * seekerOrientation) * forward; // Convert local to world seeker look direction vector

	for (auto& obj : world.objects) {
		if (obj->physicsProperties->collider != ColliderType::Sphere || obj.get() == &missile) continue; // Only tracking spheres
		glm::vec3 missileToObj = obj->position - missile.position;
		float normalDistance = glm::dot(missileToObj, worldLookDirection);

		if (normalDistance - obj->physicsProperties->radius >= maxRange || normalDistance <= -obj->physicsProperties->radius) continue; // If object out of seeker max range, object is skipped

		float objectAngle = glm::acos(glm::clamp(glm::dot(glm::normalize(missileToObj), worldLookDirection), 0.0f, 1.0f));

		float angularRadiusObj = glm::clamp( glm::asin( ( obj->physicsProperties->radius * 2.0f ) / (2.0f * normalDistance) ), 0.0f, angleFOV / 2.0f);

		// If object is within seeker FOV, start tracking
		if (objectAngle <= angleFOV / 2.0f + angularRadiusObj && obj->objTemp.temperature >= tempThreshold) {
			scannedData.tracking = true;
			scannedData.targetTemperature = obj->objTemp.temperature;
			scannedData.trackingObj = obj.get();
			scannedData.oldLOStoTarget = curSeekerData.oldLOStoTarget;
			scannedData.LOStoTarget = curSeekerData.LOStoTarget;
			return scannedData;
		}	
	}
	return scannedData;
}

SeekerData MissileSeeker::scanFlares(const World& world, Missile& missile, float deltaTime) {
	SeekerData scannedData;
	glm::vec3 forward{ 0.0f, 0.0f, 1.0f };
	glm::vec3 worldLookDirection = (missile.rotationQ * seekerOrientation) * forward; // Convert local to world seeker look direction vector

	for (auto& flare : world.flareBucket->particles) {
		if (!flare.active) { continue; }
		glm::vec3 missileToFlare = flare.position - missile.position;
		float normalDistance = glm::dot(missileToFlare, worldLookDirection);

		if (normalDistance >= maxRange) continue; // If object out of seeker max range, object is skipped

		float objectAngle = glm::acos(glm::clamp(glm::dot(glm::normalize(missileToFlare), worldLookDirection), 0.0f, 1.0f));

		// If object is within seeker FOV, start tracking
		if (objectAngle <= angleFOV / 2.0f && flare.particleTemp.temperature >= tempThreshold) {
			scannedData.tracking = true;
			scannedData.targetTemperature = flare.particleTemp.temperature;
			scannedData.trackingFlare = &flare;
			scannedData.oldLOStoTarget = curSeekerData.oldLOStoTarget;
			scannedData.LOStoTarget = curSeekerData.LOStoTarget;
			return scannedData;
		}
	}
	return scannedData;
}

void MissileSeeker::updateSeekerAngle(Missile& missile) {
	glm::vec3 seekerFront = (missile.rotationQ * seekerOrientation) * glm::vec3{ 0.0f, 0.0f, 1.0f };
	float angle = glm::acos(glm::clamp(glm::dot(missile.front, seekerFront), -1.0f, 1.0f));
	seekerAngle = angle;
}

void MissileSeeker::updateScanData(Missile& missile, float deltaTime) {
	glm::vec3 forward{ 0.0f, 0.0f, 1.0f };
	glm::vec3 localLookDirection = seekerOrientation * forward; // Makes the seeker look direction
	glm::vec3 missileToObject{ 0.0f };
	if (curSeekerData.trackingObj) {
		missileToObject = curSeekerData.trackingObj->position - missile.position;
	}
	else if(curSeekerData.trackingFlare){
		missileToObject = curSeekerData.trackingFlare->position - missile.position;
	}
	else {
		return;
	}
	curSeekerData.oldLOStoTarget = curSeekerData.LOStoTarget; // Sets the old LOS
	curSeekerData.LOStoTarget = (glm::length(missileToObject) >= 0.00001f) ? missileToObject : glm::vec3{ 0.0f }; // Gets the new LOS
	glm::vec3 oldLos = glm::normalize(curSeekerData.oldLOStoTarget);
	glm::vec3 newLos = glm::normalize(curSeekerData.LOStoTarget);

	// SEEKER ROTATION CALCULATIONS
	missileToObject = glm::normalize(glm::conjugate(missile.rotationQ) * missileToObject); // Converts the target's coordinates to the missile's local space then the seeker's local space
	glm::quat rotate = glm::normalize(glm::rotation(localLookDirection, missileToObject));
	curSeekerData.rotateToTarget = rotate; // Gets the quaternion to rotate the current seeker quaternion to the target

	// LOS CALCULATIONS
	glm::vec3 rotationAxis = glm::conjugate(missile.rotationQ) * glm::cross(oldLos, newLos); // Gets the world space losRate and converts it to local space
	float rotationMag = glm::clamp(glm::length(rotationAxis), 0.0f, 1.0f);
	if (rotationMag < 0.000001f) { // Checks if losRate is negligible to stop more calculations from happening
		curSeekerData.losRate = glm::vec3{ 0.0f };
		curSeekerData.tracking = true;
	}
	float rotateAngle = glm::asin(rotationMag);
	glm::vec3 losRate = (glm::normalize(rotationAxis) * rotateAngle) / deltaTime;
	curSeekerData.losRate = (glm::length(losRate) > 0.00001f) ? losRate : glm::vec3{ 0.0f }; // Filter out negligible losRates
}

void MissileSeeker::findRandomTarget(World& world, Missile& missile){
	glm::vec3 forward{ 0.0f, 0.0f, 1.0f };
	glm::vec3 missileFront = missile.rotationQ * forward;

	for (auto& obj : world.objects) {
		if (obj->physicsProperties->collider != ColliderType::Sphere || obj.get() == &missile) continue; // Only tracking spheres

		glm::vec3 missileToObj = obj->position - missile.position;
		float normalDistance = glm::dot(missileToObj, missileFront);

		if (normalDistance - obj->physicsProperties->radius >= maxRange || normalDistance <= -obj->physicsProperties->radius) continue; // If object out of seeker max range, object is skipped

		glm::vec3 fovCenterToObj = missileToObj - normalDistance * missileFront; // Center of cone to the object
		float distanceFovCenterToObj = glm::length(fovCenterToObj);

		float theta = gimbalLimit / 2.0f; // Gets angle of half of cone to find radius at specific distance using trig
		float radiusAtDistance = glm::tan(theta) * normalDistance; // Gets the radius of cone at the distance the object is at along cone

		// If object is within seeker FOV, start tracking
		if (distanceFovCenterToObj < radiusAtDistance + obj->physicsProperties->radius && obj->objTemp.temperature >= tempThreshold) {
			// SEEKER ROTATION CALCULATIONS
			missileToObj = glm::conjugate(missile.rotationQ) * glm::normalize(missileToObj); // Converts the target's coordinates to the missile's local space then the seeker's local space
			seekerOrientation = glm::rotation(forward, missileToObj);
			curSeekerData.tracking = true;
			return;
		}
	}
}

void MissileSeeker::printState() {
	if (curState == SeekerState::Scanning) {
		std::cout << "SCANNING" << std::endl;
	}
	if (curState == SeekerState::Tracking) {
		std::cout << "TRACKING" << std::endl;
	}
	if (curState == SeekerState::MemoryRelock) {
		std::cout << "RELOCKING" << std::endl;
	}
	if (curState == SeekerState::Memory) {
		std::cout << "MEMORY" << std::endl;
	}
	if (curState == SeekerState::Off) {
		std::cout << "OFF" << std::endl;
	}
}