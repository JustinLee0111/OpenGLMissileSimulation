#include "missile/Missile.h"

void Missile::update(const World& world, float deltaTime) {
	seeker.update(world, *this, deltaTime);
	proNav(deltaTime);
	if (seeker.curState == SeekerState::Off) {
		seeker.resetSeekerData();
		physicsProperties->angularVelocity = glm::vec3{ 0.0f };
	}
}

void Missile::proNav(float deltaTime) {
	if (seeker.curState == SeekerState::Tracking) {
		glm::vec3 flightPathRate{ getFlightPathRate() };
		float flightPathMag = glm::length(flightPathRate);
		if (flightPathMag >= 0.00001f) {
			if (flightPathMag > maxAngVel) {
				physicsProperties->angularVelocity = flightPathRate * (maxAngVel / flightPathMag); // Caps the rotation rate to the maxAngVel of the missile, will replace with speed dependent turn rate
			}
			else {
				physicsProperties->angularVelocity = flightPathRate;
			}
		}
		else {
			physicsProperties->angularVelocity = glm::vec3{ 0.0f };
		}
	}
}