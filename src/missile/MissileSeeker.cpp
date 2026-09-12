#include "missile/MissileSeeker.h"
#include "missile/Missile.h"
#include "rendering/Debugging.h"
#include "environment/World.h"
#include "environment/Object.h"

#include <iostream>
#include <memory>

void MissileSeeker::update(const World& world, Missile& missile, float deltaTime){
	updateSeekerState();
	/*if (curState == SeekerState::Tracking) {
		scan(world, missile);
	}*/
	if (curState == SeekerState::Locking) {
		horizontalAngle += curSeekerData.horizontalDiff;
		verticalAngle -= curSeekerData.verticalDiff;
	}
	scan(world, missile);
	float halfAngle = glm::radians(gimbalLimit / 2.0f);
	horizontalAngle = glm::clamp(horizontalAngle, -halfAngle, halfAngle);
	verticalAngle = glm::clamp(verticalAngle, -halfAngle, halfAngle);
}

void MissileSeeker::updateSeekerState() {
	if (curSeekerData.hasLock && curSeekerData.tracking) {
		curState = SeekerState::Tracking;
	}
	/*else if (!curSeekerData.hasLock) {
		curState = SeekerState::Scanning;
		curSeekerData.tracking = false;
	}*/
	else if (curSeekerData.hasLock) {
		curState = SeekerState::Locking;
	}
	else {
		curState = SeekerState::Scanning;
	}
}

void MissileSeeker::scan(const World& world, Missile& missile){
	glm::vec3 forward{ 0.0f, 0.0f, 1.0f };

	glm::quat seekerQ = glm::quat(glm::vec3{ verticalAngle, horizontalAngle, 0.0f }); // Use glm to create quaternion with angles

	glm::vec3 localLookDirection = seekerQ * forward; // Makes the seeker look direction
	glm::vec3 worldLookDirection = (missile.rotationQ * seekerQ) * forward; // Convert local to world seeker look direction vector

	for (auto& obj : world.objects) {
		if (obj->physicsProperties->collider != ColliderType::Sphere) continue; // Only tracking spheres

		glm::vec3 missileToObj = obj->position - missile.position;
		float normalDistance = glm::dot(missileToObj, worldLookDirection);

		if (normalDistance - obj->physicsProperties->radius >= maxRange || normalDistance <= -obj->physicsProperties->radius) continue; // If object out of seeker max range, object is skipped

		glm::vec3 fovCenterToObj = missileToObj - normalDistance * worldLookDirection; // Center of cone to the object
		float distanceFovCenterToObj = glm::length(fovCenterToObj);

		float theta = glm::radians(angleFOV / 2.0f); // Gets angle of half of cone to do trig
		float radiusAtDistance = glm::tan(theta) * normalDistance; // Gets the radius of cone at the distance the object is at along cone

		if (distanceFovCenterToObj < radiusAtDistance + obj->physicsProperties->radius) {
			curSeekerData.hasLock = true;

			missileToObj = glm::normalize(missileToObj);

			missileToObj = glm::conjugate(missile.rotationQ * seekerQ) * missileToObj; // Converts the object coordinates to the missile's local space then the seeker's local space

			// Gets the difference in angle from object and current look at direction
			curSeekerData.verticalDiff = std::atan2(missileToObj.y, missileToObj.z);
			curSeekerData.horizontalDiff = std::atan2(missileToObj.x, missileToObj.z);
			return;
		}
	}
	curSeekerData.hasLock = false;
}