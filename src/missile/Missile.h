#pragma once

#include "missile/MissileSeeker.h"
#include "environment/Object.h"

class World;

class Missile : public Object {
	public:
		Missile() = default;
		~Missile() = default;
		void update(const World& world, float deltaTime);

		const MissileSeeker getSeeker() const{
			return seeker;
		}

		void changeSeekerEnable() {
			seeker.seekerEnabled = !seeker.seekerEnabled;
		}

		glm::vec3 getFlightPathRate() { // Gets how much to rotate by to get on proper flight path angle for collision
			return proNavGain * seeker.getLOSrate();
		}

		void proNav(float deltaTime); // Uses proportional navigation to correct flight path for collision

		void updateAeroForce(float airDensity); // Calculates lift and drag forces

		bool engineOn = false;

	private:
		MissileSeeker seeker;
		//MissileTracking tracking;
		float proNavGain = 3.5f; // Gain for how aggressive the missile gets on optimal flight path, higher = more aggressive turning earlier
		float maxAngVel = glm::pi<float>() / 4.0f; // Radians per second
		float engineBurnTime = 5.0f; // Burn time in seconds
		float burnTimeRemaining = engineBurnTime;
		float wingSurfaceArea = 0.25f; // In m^2
		float wingFrontSurfaceArea = 0.025f; // Frontal surface area for drag calculations, also in m^2
		float speedMaxTurn = 50.0f; // The speed at which the missile has the best turn rate
		glm::vec3 aeroForce{ 0.0f }; // Aero force in newtons
		glm::vec3 engineThrust{0.0f, 0.0f, 140.0f}; // Thrust in newtons
};