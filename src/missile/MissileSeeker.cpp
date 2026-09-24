#include <iostream>
#include <memory>

#include "missile/MissileSeeker.h"
#include "missile/Missile.h"
#include "environment/World.h"
#include "environment/Object.h"
#include "environment/Temperature.h"

void MissileSeeker::update(const World& world, Missile& missile, float deltaTime){
	std::cout << "WOW: " << glm::length(curSeekerData.LOStoTarget) << std::endl;
	if (curState != SeekerState::Off) {
		scanFOV(world, missile, deltaTime);
		updateSeekerState();
		
		if (curState == SeekerState::Tracking) {
			seekerOrientation = glm::normalize(curSeekerData.rotateToTarget * seekerOrientation); // Updates the rotation for seeker to point towards target
			//std::cout << "LOS Rate: " << glm::length( curSeekerData.losRate ) << std::endl;
		}
	}
	else if(curState == SeekerState::Off){
		updateSeekerState();
		resetSeekerRot();
	}
	updateSeekerAngle(missile);
	clampGimbal(missile);
	curSeekerData.tracking = false;
}

void MissileSeeker::updateSeekerState() {
	if (curSeekerData.tracking && seekerEnabled) {
		curState = SeekerState::Tracking;
	}
	else if(seekerEnabled){
		curState = SeekerState::Scanning;
	}
	else if(!seekerEnabled){
		curState = SeekerState::Off;
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
	std::cout << "WOW1: " << glm::length(objScan.losRate) << std::endl;
	std::cout << "WOW2: " << glm::length(flareScan.losRate) << std::endl;

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
	glm::vec3 localLookDirection = seekerOrientation * forward; // Makes the seeker look direction
	glm::vec3 worldLookDirection = (missile.rotationQ * seekerOrientation) * forward; // Convert local to world seeker look direction vector

	for (auto& obj : world.objects) {
		if (obj->physicsProperties->collider != ColliderType::Sphere || obj.get() == &missile) continue; // Only tracking spheres

		glm::vec3 missileToObj = obj->position - missile.position;
		float normalDistance = glm::dot(missileToObj, worldLookDirection);

		if (normalDistance - obj->physicsProperties->radius >= maxRange || normalDistance <= -obj->physicsProperties->radius) continue; // If object out of seeker max range, object is skipped

		glm::vec3 fovCenterToObj = missileToObj - normalDistance * worldLookDirection; // Center of cone to the object
		float distanceFovCenterToObj = glm::length(fovCenterToObj);

		float theta = angleFOV / 2.0f; // Gets angle of half of cone to find radius at specific distance using trig
		float radiusAtDistance = glm::tan(theta) * normalDistance; // Gets the radius of cone at the distance the object is at along cone

		// If object is within seeker FOV, start tracking
		if (distanceFovCenterToObj < radiusAtDistance + obj->physicsProperties->radius && obj->objTemp.temperature >= tempThreshold) {
			scannedData.tracking = true;
			scannedData.targetTemperature = obj->objTemp.temperature;

			scannedData.oldLOStoTarget = (glm::length(curSeekerData.LOStoTarget) != 0.0f) ? curSeekerData.LOStoTarget : glm::vec3{ 0.0f }; // Sets the old LOS
			scannedData.LOStoTarget = (glm::length(missileToObj) >= 0.00001f) ? missileToObj : glm::vec3{ 0.0f }; // Gets the new LOS
			std::cout << "WOW3: " << glm::length(scannedData.LOStoTarget) << std::endl;
			std::cout << "WOW4: " << glm::length(scannedData.oldLOStoTarget) << std::endl;
			glm::vec3 oldLos = glm::normalize(scannedData.oldLOStoTarget);
			glm::vec3 newLos = glm::normalize(scannedData.LOStoTarget);

			// LOS CALCULATIONS
			glm::vec3 rotationAxis = glm::conjugate(missile.rotationQ) * glm::cross(oldLos, newLos); // Gets the world space losRate and converts it to local space
			float rotationMag = glm::clamp(glm::length(rotationAxis), -1.0f, 1.0f);
			if (rotationMag < 0.00001f) { // Checks if losRate is negligible to stop more calculations from happening
				scannedData.losRate = glm::vec3{ 0.0f };
				scannedData.rotateToTarget = glm::quat{ 1.0f, 0.0f, 0.0f ,0.0f };
				scannedData.tracking = true;
				return scannedData;
			}
			float rotateAngle = glm::asin(rotationMag);
			glm::vec3 losRate = (glm::normalize(rotationAxis) * rotateAngle) / deltaTime;
			scannedData.losRate = (glm::length(losRate) > 0.0001f) ? losRate : glm::vec3{ 0.0f }; // Filter out negligible losRates

			// SEEKER ROTATION CALCULATIONS
			missileToObj = glm::normalize(glm::conjugate(missile.rotationQ) * missileToObj); // Converts the target's coordinates to the missile's local space then the seeker's local space
			glm::quat rotate = glm::normalize(glm::rotation(localLookDirection, missileToObj));
			scannedData.rotateToTarget = rotate; // Gets the quaternion to rotate the current seeker quaternion to the target
			scannedData.tracking = true;

			return scannedData;
		}
	}
	return scannedData;
}

SeekerData MissileSeeker::scanFlares(const World& world, Missile& missile, float deltaTime) {
	SeekerData scannedData;
	glm::vec3 forward{ 0.0f, 0.0f, 1.0f };
	glm::vec3 localLookDirection = seekerOrientation * forward; // Makes the seeker look direction
	glm::vec3 worldLookDirection = (missile.rotationQ * seekerOrientation) * forward; // Convert local to world seeker look direction vector

	for (auto& flare : world.flareBucket->particles) {
		if (!flare.active) { continue; }
		glm::vec3 missileToFlare = flare.position - missile.position;
		float normalDistance = glm::dot(missileToFlare, worldLookDirection);

		if (normalDistance >= maxRange || normalDistance <= 0.0f) continue; // If object out of seeker max range, object is skipped

		glm::vec3 fovCenterToObj = missileToFlare - normalDistance * worldLookDirection; // Center of cone to the object
		float distanceFovCenterToObj = glm::length(fovCenterToObj);

		float theta = angleFOV / 2.0f; // Gets angle of half of cone to find radius at specific distance using trig
		float radiusAtDistance = glm::tan(theta) * normalDistance; // Gets the radius of cone at the distance the object is at along cone

		// If object is within seeker FOV, start tracking
		if (distanceFovCenterToObj < radiusAtDistance && flare.particleTemp.temperature >= tempThreshold) {
			scannedData.tracking = true;
			scannedData.targetTemperature = flare.particleTemp.temperature;

			scannedData.oldLOStoTarget = (glm::length(curSeekerData.LOStoTarget) != 0.0f) ? curSeekerData.LOStoTarget : glm::vec3{ 0.0f }; // Sets the old LOS
			scannedData.LOStoTarget = (glm::length(missileToFlare) >= 0.00001f) ? missileToFlare : glm::vec3{ 0.0f }; // Gets the new LOS
			glm::vec3 oldLos = glm::normalize(scannedData.oldLOStoTarget);
			glm::vec3 newLos = glm::normalize(scannedData.LOStoTarget);

			// LOS CALCULATIONS
			glm::vec3 rotationAxis = glm::conjugate(missile.rotationQ) * glm::cross(oldLos, newLos); // Gets the world space losRate and converts it to local space
			float rotationMag = glm::clamp(glm::length(rotationAxis), -1.0f, 1.0f);
			if (rotationMag < 0.00001f) { // Checks if losRate is negligible to stop more calculations from happening
				scannedData.losRate = glm::vec3{ 0.0f };
				scannedData.rotateToTarget = glm::quat{ 1.0f, 0.0f, 0.0f ,0.0f };
				scannedData.tracking = true;
				return scannedData;
			}
			float rotateAngle = glm::asin(rotationMag);
			glm::vec3 losRate = (glm::normalize(rotationAxis) * rotateAngle) / deltaTime;
			scannedData.losRate = (glm::length(losRate) > 0.0001f) ? losRate : glm::vec3{ 0.0f }; // Filter out negligible losRates

			// SEEKER ROTATION CALCULATIONS
			missileToFlare = glm::normalize(glm::conjugate(missile.rotationQ) * missileToFlare); // Converts the target's coordinates to the missile's local space then the seeker's local space
			glm::quat rotate = glm::normalize(glm::rotation(localLookDirection, missileToFlare));
			scannedData.rotateToTarget = rotate; // Gets the quaternion to rotate the current seeker quaternion to the target
			scannedData.tracking = true;
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