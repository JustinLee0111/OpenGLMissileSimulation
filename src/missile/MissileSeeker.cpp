#define GLM_ENABLE_EXPERIMENTAL

#include "missile/MissileSeeker.h"
#include "missile/Missile.h"
#include "rendering/Debugging.h"
#include "environment/World.h"
#include "environment/Object.h"

#include <iostream>
#include <memory>
#include <glm/gtx/quaternion.hpp>

void MissileSeeker::update(const World& world, Missile& missile, float deltaTime){
	if (curState != SeekerState::Off) {
		scan(world, missile);
		updateSeekerState();
		if (curState == SeekerState::Tracking) {
			seekerOrientation = glm::normalize(curSeekerData.rotateToTarget * seekerOrientation); // Updates the rotation for seeker to point towards target
			clampGimbal(missile);
		}
	}
	else if(curState == SeekerState::Off){
		updateSeekerState();
	}
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

void MissileSeeker::scan(const World& world, Missile& missile){
	glm::vec3 forward{ 0.0f, 0.0f, 1.0f };

	glm::vec3 localLookDirection = seekerOrientation * forward; // Makes the seeker look direction
	glm::vec3 worldLookDirection = (missile.rotationQ * seekerOrientation) * forward; // Convert local to world seeker look direction vector

	for (auto& obj : world.objects) {
		if (obj->physicsProperties->collider != ColliderType::Sphere) continue; // Only tracking spheres

		glm::vec3 missileToObj = obj->position - missile.position;
		float normalDistance = glm::dot(missileToObj, worldLookDirection);

		if (normalDistance - obj->physicsProperties->radius >= maxRange || normalDistance <= -obj->physicsProperties->radius) continue; // If object out of seeker max range, object is skipped

		glm::vec3 fovCenterToObj = missileToObj - normalDistance * worldLookDirection; // Center of cone to the object
		float distanceFovCenterToObj = glm::length(fovCenterToObj);

		float theta = angleFOV / 2.0f; // Gets angle of half of cone to do trig
		float radiusAtDistance = glm::tan(theta) * normalDistance; // Gets the radius of cone at the distance the object is at along cone

		if (distanceFovCenterToObj < radiusAtDistance + obj->physicsProperties->radius) {
			curSeekerData.tracking = true;
		
			missileToObj = glm::normalize( glm::conjugate(missile.rotationQ) * missileToObj ); // Converts the object coordinates to the missile's local space then the seeker's local space

			curSeekerData.rotateToTarget = glm::rotation(localLookDirection, missileToObj); // Gets the quaternion to rotate the current seeker quaternion to the target
			return;
		}
	}
	curSeekerData.tracking = false;
}