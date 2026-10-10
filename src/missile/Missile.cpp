#include "missile/Missile.h"
#include "environment/World.h"

#include <iostream>
#include <glm/glm.hpp>

void Missile::update(World& world, float deltaTime) {
	if (proximityFuseTrig(world)) {
		return;
	}
	updateAeroForce(world.airDensity, deltaTime);
	physicsProperties->addForce(aeroForce);
	if (engineOn && burnTimeRemaining > 0.0f) { // While engine is on and has fuel remaining, emit smoke trail and add thrust
		smokeParticles->emitParticles(this, -front);
		physicsProperties->addForce(rotationQ * engineThrust);
		burnTimeRemaining -= deltaTime;
	}
	else if(burnTimeRemaining <= 0.0f){
		engineOn = false;
		smokeParticles->stopEmit();
	}
	if (seeker->curState == SeekerState::Scanning) {
		physicsProperties->angularVelocity = glm::vec3{ 0.0f };
	}
	else if (seeker->curState == SeekerState::Off) { // If seeker is off, don't continue it's trajectory and go straight
		seeker->resetSeekerData();
		physicsProperties->angularVelocity = glm::vec3{ 0.0f };
	}

	// Slows turn rate during memory flight to help reaquire target after memory flight
	// Prevents the seeker being too far forward or behind target after seeker is enabled again
	if (seeker->getMemoryFlight() && !memoryTurnDamping && seeker->curState != SeekerState::MemoryRelock) {
		physicsProperties->angularVelocity /= 2.5f;
		memoryTurnDamping = true;
	}
	else if (!seeker->getMemoryFlight()) {
		memoryTurnDamping = false;
	}
}

// Early update checks if objects are within seeker FOV before objects move
// Doesn't get data from the objects yet, just checks if in FOV
// This is a pseudo continuous seeker solution and doesn't account for seeker slew rate for now
void Missile::earlyUpdate(World& world, float deltaTime) {
	seeker->update(world, *this, deltaTime);
}

// Once objects update, using the scanned object from earlyUpdate, it updates the data
void Missile::lateUpdate(World& world, float deltaTime) {
	seeker->updateScanData(*this, deltaTime);
	seeker->lateUpdate(world, *this, deltaTime);
	if (launched) { // If missile isn't launched yet, don't rotate missile
		proNav(deltaTime);
	}
	else { // Bandaid solution to reset angular velocity to prevent perpetual spinning in some cases
		physicsProperties->angularVelocity = glm::vec3{ 0.0f };
	}
}

// Damping is necessary to prevent/help with over correction from aerodynamic lag during high aoa/gforce maneuvers
// A fix would be to orient boresight to velocity during low turn commands from proportional navigation
void Missile::proNav(float deltaTime) { // Orients the missile using calculated proportional navigation to intercept the target with constraints such as AOA and GForces
	if (seeker->curState == SeekerState::Tracking) {
		glm::vec3 flightPathRate{ getFlightPathRate() };
		float flightPathMag = glm::length(flightPathRate);
		
		glm::vec3 finalAngularVelocity{ 0.0f };

		if (flightPathMag >= 0.001f) {
			float gForce = liftForce / (physicsProperties->mass * 9.81f);
			float vel = glm::length(physicsProperties->velocity);

			float finalDamp = 0.0f;

			if (gForce > gLimitDampStart) { // Damp turn rate when nearing gLimit
				float GdampRatio = glm::clamp((gForce - gLimitDampStart) / (gLimit - gLimitDampStart), 0.0f, 1.0f);
				finalDamp = GdampRatio;
				std::cout << GdampRatio << std::endl;
			}
			if (!engineOn && missileAOA >= glm::radians(aoaNoEngineDampStart)) { // Damps turn rate when AOA is nearing AOA limit while engine is off, this is to mimic thrust vectoring being off
				float engineOffAOAstart = glm::radians(aoaNoEngineDampStart);
				float engineOffAOAratio = glm::clamp((missileAOA - engineOffAOAstart) / (glm::radians(aoaLimitNoEngine) - engineOffAOAstart), 0.0f, 1.0f);
				finalDamp = glm::max(finalDamp, engineOffAOAratio);
			}
			if (engineOn && missileAOA >= glm::radians(aoaEngineDampStart)) { // Damps turn rate when AOA is nearing AOA limit while engine is on, this is to mimic thrust vectoring being on
				float engineOnAOAstart = glm::radians(aoaEngineDampStart);
				float engineOnAOAratio = glm::clamp((missileAOA - engineOnAOAstart) / (glm::radians(aoaLimitEngine) - engineOnAOAstart), 0.0f, 1.0f);
				finalDamp = glm::max(finalDamp, engineOnAOAratio);
			}

			// Makes the turn rate speed dependent, at the specified speed, the turn rate is at its peak
			// Minimum is 0.75 to mimic thrust vectoring at lower speeds only while engine is on
			// Minimum becomes 0.0 when engine is off to mimic thrust vectoring not available with engine off
			float lowSpeedCap = 0.75f;
			if (!engineOn) { lowSpeedCap = 0.0f; }
			float speedTurnCap = std::clamp( vel * vel / speedMaxTurn, lowSpeedCap, 1.0f);

			if (flightPathMag > maxAngVel) {
				finalAngularVelocity = flightPathRate * ( (maxAngVel * speedTurnCap)  / flightPathMag);
			}
			else {
				finalAngularVelocity = flightPathRate * speedTurnCap;
			}
			if (finalDamp > 0.0f) {
				finalAngularVelocity *= (1 - finalDamp);
			}
		}
		physicsProperties->angularVelocity = finalAngularVelocity;

		// Makes sure the seeker is always tracking even with sharp turns
		glm::quat missileRotate = glm::angleAxis(glm::length(finalAngularVelocity * deltaTime), glm::normalize(finalAngularVelocity * deltaTime));
		glm::vec3 projectedSeekerLook = (rotationQ * (glm::conjugate(missileRotate) * seeker->seekerOrientation)) * glm::vec3{ 0.0f, 0.0f, 1.0f };
		float projectedSeekerAngle = glm::acos(glm::clamp(glm::dot(projectedSeekerLook, front), 0.0f, 1.0f));

		if (projectedSeekerAngle >= seeker->gimbalLimit / 2.0f) {
			physicsProperties->angularVelocity = glm::vec3{ 0.0f }; // Don't make missile turn if seeker will lose visual on target
		}
	}
}

// Super simplified lift and drag calculations
// Lift and drag coefficient is simply linear to angle of attack but drag also has parasitic drag no matter what
// Using the lift force equation and drag force equation combined to get total aero force
void Missile::updateAeroForce(float airDensity, float deltaTime) {
	glm::vec3 normVel;
	if (glm::length(physicsProperties->velocity) > 0.0f) { // If not moving, no lift is generated
		normVel = glm::normalize(physicsProperties->velocity);
	}
	else {
		aeroForce = glm::vec3{ 0.0f };
		return;
	}
	float angleOfAttack = glm::acos(glm::clamp(glm::dot(front, normVel), -1.0f, 1.0f));
	missileAOA = angleOfAttack;

	// Once velocity exceeds 1000, it starts limiting AOA to reduce excessive forces
	// Fixes exponential lift and drag force gain at extremely high speeds
	float vel = glm::length(physicsProperties->velocity);
	if (vel > 1000.0f) {
		angleOfAttack = glm::clamp(angleOfAttack, 0.0f, (glm::pi<float>() / 16.0f) / std::sqrt(glm::length(physicsProperties->velocity)));
	}

	float division = (airDensity * glm::dot(physicsProperties->velocity, physicsProperties->velocity)) / 2.0f;

	// Due to the simplified lift and drag coefficients, an AOA multiplier is needed to more match realistic lift and drag forces
	float liftCoeff = angleOfAttack * aoaMultiplier;
	float dragCoeff = angleOfAttack * aoaMultiplier + 0.25f; // AOA plus parasitic drag

	glm::vec3 rotateAxis{ left };
	glm::vec3 liftDir{ 0.0f };

	// Sets the direction of the lift direction to be perpendicular to missile body
	if (angleOfAttack > 0.00001f) {
		rotateAxis = glm::normalize(glm::cross(normVel, front));
		liftDir = glm::normalize(glm::cross(rotateAxis, front));
	}

	glm::vec3 lift = liftDir * (liftCoeff * division * wingSurfaceArea);
	glm::vec3 drag = -normVel * (dragCoeff * division * wingFrontSurfaceArea);

	liftForce = glm::length(lift);

	aeroForce = lift + drag;
}

bool Missile::proximityFuseTrig(World& world){
	for (auto& obj : world.objects) {
		if (!obj || obj->physicsProperties->collider != ColliderType::Sphere || obj.get() == this) { continue; }
		glm::vec3 missileToObject = obj->position - position;
		float speedSlop = glm::length(physicsProperties->velocity) / 50.0f; // To prevent tunneling of proxy fuse check
		float dist = (glm::length(missileToObject) - obj->physicsProperties->radius - physicsProperties->radius);
		if (dist > proxyTrigDist + speedSlop) { continue; }
		else {
			smokeParticles->stopEmit();
			obj->takeDamage(explosionDamage);
			return true;
		}
	}
	return false;
}

void Missile::findRandomTarget(World& world){
	seeker->findRandomTarget(world, *this);
}