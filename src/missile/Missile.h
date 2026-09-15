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

	private:
		MissileSeeker seeker;
		//MissileTracking tracking;
		float proNavGain = 3.5f; // Gain for how aggressive the missile gets on optimal flight path, higher = more aggressive
		float maxAngVel = glm::pi<float>() / 5.0f; // Radians per second
};