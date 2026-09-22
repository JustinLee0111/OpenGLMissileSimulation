#pragma once

#include <memory>

#include "missile/MissileSeeker.h"
#include "environment/Object.h"
#include "environment/ParticleBucket.h"

class World;

class Missile : public Object {
	public:
		Missile() = default;
		~Missile() = default;
		void update(World& world, float deltaTime);

		const MissileSeeker getSeeker() const{
			return *seeker;
		}

		void changeSeekerEnable() {
			seeker->seekerEnabled = !seeker->seekerEnabled;
		}

		glm::vec3 getFlightPathRate() { // Gets how much to rotate by to get on proper flight path angle for collision
			return proNavGain * seeker->getLOSrate();
		}

		void setParticles(ParticleBucket& particles) { this->particles = &particles; }

		void proNav(float deltaTime); // Uses proportional navigation to correct flight path for collision

		void updateAeroForce(float airDensity, float deltaTime); // Calculates lift and drag forces

		bool proximityFuseTrig(World& world);

		void findRandomTarget(World& world);

		bool engineOn = false;

	private:
		std::unique_ptr<MissileSeeker> seeker = std::make_unique<MissileSeeker>();
		ParticleBucket* particles = nullptr;

		float proNavGain{ 3.5f }; // Gain for how aggressive the missile gets on optimal flight path, higher = more aggressive turning earlier
		float maxAngVel{ glm::pi<float>() }; // Radians per second

		float engineBurnTime{ 5.0f }; // Burn time in seconds
		float burnTimeRemaining{ engineBurnTime };

		float wingSurfaceArea{ 0.25f }; // In m^2
		float wingFrontSurfaceArea{ 0.025f }; // Frontal surface area for drag calculations, also in m^2
		float speedMaxTurn{ 100.0f }; // The speed at which the missile has the best turn rate

		float proxyTrigDist{ 5.0f }; // Distance from edge of missile to target edge for triggering proxy fuse
		float explosionDamage{ 100.0f };
		float explosionRadius{ 5.0f };

		glm::vec3 aeroForce{ 0.0f }; // Aero force in newtons
		glm::vec3 engineThrust{0.0f, 0.0f, 150.0f}; // Thrust in newtons
};