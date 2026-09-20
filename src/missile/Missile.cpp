#include "missile/Missile.h"
#include "environment/World.h"

#include <iostream>
#include <glm/glm.hpp>

void Missile::update(const World& world, float deltaTime) {
	seeker.update(world, *this, deltaTime);
	proNav(deltaTime);
	updateAeroForce(world.airDensity);
	physicsProperties->addForce(aeroForce);
	if (engineOn && burnTimeRemaining > 0.0f) { // While engine is on and has fuel remaining, emit smoke trail and add thrust
		world.missileSmoke->emitParticles(position, -front);
		physicsProperties->addForce(rotationQ * engineThrust);
		burnTimeRemaining -= deltaTime;
	}
	else {
		engineOn = false;
		world.missileSmoke->stopEmit();
	}
	if (seeker.curState == SeekerState::Scanning) {
		physicsProperties->angularVelocity = glm::vec3{ 0.0f };
	}
	else if (seeker.curState == SeekerState::Off) { // If seeker is off, don't continue it's trajectory and go straight
		seeker.resetSeekerData();
		physicsProperties->angularVelocity = glm::vec3{ 0.0f };
	}
	std::cout << "Forward Vel: " << (glm::conjugate(rotationQ) * physicsProperties->velocity).z << std::endl; // Prints the forward velocity of the missile for debugging
	if (seeker.curState == SeekerState::Scanning) {
	}
	
}

void Missile::proNav(float deltaTime) { // Orients the missile using calculated proportional navigation to intercept the target
	if (seeker.curState == SeekerState::Tracking) {
		glm::vec3 flightPathRate{ getFlightPathRate() };
		float flightPathMag = glm::length(flightPathRate);
		
		glm::vec3 finalAngularVelocity{ 0.0f };

		if (flightPathMag >= 0.00001f) {
			float vel = glm::length(physicsProperties->velocity);

			// Makes the turn rate speed dependent, at the specified speed, the turn rate is at its peak
			// Minimum is 0.5 to mimic thrust vectoring at lower speeds
			float speedTurnCap = std::clamp( vel * vel / speedMaxTurn, 0.5f, 1.0f);

			if (flightPathMag > maxAngVel) {
				finalAngularVelocity = flightPathRate * ( (maxAngVel * speedTurnCap)  / flightPathMag);
			}
			else {
				finalAngularVelocity = flightPathRate * speedTurnCap;
			}
		}
		// Makes sure the seeker is always tracking even with sharp turns
		glm::quat missileRotate = glm::angleAxis(glm::length(finalAngularVelocity * deltaTime), glm::normalize(finalAngularVelocity * deltaTime));
		glm::vec3 projectedSeekerLook = (rotationQ * ( glm::conjugate(missileRotate) * seeker.seekerOrientation ) ) * glm::vec3{ 0.0f, 0.0f, 1.0f };
		float projectedSeekerAngle = glm::acos(glm::clamp(glm::dot(projectedSeekerLook, front), -1.0f, 1.0f));

		if (projectedSeekerAngle >= seeker.gimbalLimit / 2.0f) {
			physicsProperties->angularVelocity = glm::vec3{ 0.0f }; // Don't make missile turn if seeker will lose visual on target
		}
		else {
			physicsProperties->angularVelocity = finalAngularVelocity;
		}
	}
}

// Super simplified lift and drag calculations
// Lift and drag coefficient is simply linear to angle of attack but drag also has parasitic drag no matter what
// Using the lift force equation and drag force equation combined to get total aero force
// Lift force is supposed to drop off at extreme angle of attack but not implemented
void Missile::updateAeroForce(float airDensity) {
	glm::vec3 normVel;
	if (glm::length(physicsProperties->velocity) > 0.0f) { // If not moving, no lift is generated
		normVel = glm::normalize(physicsProperties->velocity);
	}
	else {
		aeroForce = glm::vec3{ 0.0f };
		return;
	}
	float angleOfAttack = glm::acos(glm::clamp(glm::dot(front, normVel), -1.0f, 1.0f));
	float division = (airDensity * glm::dot(physicsProperties->velocity, physicsProperties->velocity)) / 2.0f;

	float liftCoeff = angleOfAttack;
	float dragCoeff = angleOfAttack + 0.005f; // AOA plus parasitic drag

	glm::vec3 rotateAxis{ left };
	glm::vec3 liftDir{ 0.0f };

	// Sets the direction of the lift direction to be perpendicular to missile body
	if (glm::dot(normVel, front) > 0.0f) {
		rotateAxis = glm::normalize(glm::cross(normVel, front));
		liftDir = glm::normalize(glm::cross(rotateAxis, front));
	}

	glm::vec3 lift = liftDir * (liftCoeff * division * wingSurfaceArea);
	glm::vec3 drag = -normVel * (dragCoeff * division * wingFrontSurfaceArea);
	//std::cout << "Lift Force: " << glm::length(lift) << std::endl;
	//std::cout << "Drag Force: " << glm::length(drag) << std::endl;
	aeroForce = lift + drag;
}