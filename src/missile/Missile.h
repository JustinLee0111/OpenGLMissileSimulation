#pragma once

#include "missile/MissileSeeker.h"
#include "missile/MissileTracking.h"
#include "environment/Object.h"

class World;

class Missile : public Object {
	public:
		Missile() = default;
		~Missile() = default;
		void update(const World& world, float deltaTime);
	private:
		MissileSeeker seeker;
		//MissileTracking tracking;
};