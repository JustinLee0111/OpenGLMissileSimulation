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
		void earlyUpdate(World& world, float deltaTime);
		void lateUpdate(World& world, float deltaTime);

		const MissileSeeker getSeeker() const{
			return *seeker;
		}

		void changeSeekerEnable() {
			seeker->seekerEnabled = !seeker->seekerEnabled;
		}

		glm::vec3 getFlightPathRate() { // Gets how much to rotate by to get on proper flight path angle for collision
			return proNavGain * seeker->getLOSrate();
		}

		glm::vec3 getAeroForce() {
			return aeroForce;
		}

		void setParticles(ParticleBucket& particles) { this->smokeParticles = &particles; }

		void proNav(float deltaTime); // Uses proportional navigation to correct flight path for collision

		void updateAeroForce(float airDensity, float deltaTime); // Calculates lift and drag forces

		bool proximityFuseTrig(World& world);

		void findRandomTarget(World& world);

		const float getAOA() const{
			return missileAOA;
		}

		const float getFuelTimeLeft() const {
			return burnTimeRemaining;
		}

		const float getLift() const {
			return liftForce;
		}

		void resetFuel() {
			burnTimeRemaining = engineBurnTime;
		}

		void launchMissile() {
			launched = true;
		}

		bool engineOn = false;

	private:
		std::unique_ptr<MissileSeeker> seeker = std::make_unique<MissileSeeker>();
		ParticleBucket* smokeParticles = nullptr; // Breaks responsibility ownership with world but didn't want to search bucket everytime for smoke

		float proNavGain{ 3.5f }; // Gain for how aggressive the missile gets on optimal flight path, higher = more aggressive turning earlier
		float maxAngVel{ glm::pi<float>() }; // Radians per second

		float engineBurnTime{ 5.0f }; // Burn time in seconds
		float burnTimeRemaining{ engineBurnTime };

		float wingSurfaceArea{ 0.95f }; // In m^2
		float wingFrontSurfaceArea{ 0.2f }; // Frontal surface area for drag calculations, also in m^2
		float aoaMultiplier{ 2.5f }; // Multiplier for aoa force generated to more match realistic lift
		float speedMaxTurn{ 400.0f }; // The speed at which the missile has the best turn rate

		float gLimit{ 60.0f };
		float aoaLimitNoEngine{ 35.0f };
		float aoaLimitEngine{ 50.0f };

		float gLimitDampStart{ 45.0f };
		float aoaNoEngineDampStart{ 30.0f };
		float aoaEngineDampStart{ 45.0f };

		float proxyTrigDist{ 5.0f }; // Distance from edge of missile to target edge for triggering proxy fuse
		float explosionDamage{ 100.0f };
		float explosionRadius{ 5.0f };

		bool memoryTurnDamping{ false }; // Slows turn rate during memory flight
		bool launched{ false };

		glm::vec3 aeroForce{ 0.0f }; // Aero force in newtons
		glm::vec3 engineThrust{0.0f, 0.0f, 17000.0f}; // Thrust in newtons

		float missileAOA{ 0.0f };
		float liftForce{ 0.0f }; // For g limiter
};