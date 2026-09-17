#include "missile/Missile.h"
#include "environment/World.h"

#include <iostream>

void Missile::update(const World& world, float deltaTime) {
	seeker.update(world, *this, deltaTime);
	proNav(deltaTime);
	if (engineOn && burnTimeRemaining > 0.0f) { // While engine is on and has fuel remaining, emit smoke trail and add thrust
		world.missileSmoke->emitParticles(position, -front);
		physicsProperties->addForce(engineThrust);
		burnTimeRemaining -= deltaTime;
	}
	else {
		engineOn = false;
		world.missileSmoke->stopEmit();
	}

	if (seeker.curState == SeekerState::Off) { // If seeker is off, don't continue it's trajectory and go straight
		seeker.resetSeekerData();
		physicsProperties->angularVelocity = glm::vec3{ 0.0f };
	}
	std::cout << physicsProperties->velocity.z << std::endl; // Prints the forward velocity of the missile for debugging
}

void Missile::proNav(float deltaTime) { // Orients the missile using calculated proportional navigation to intercept the target
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