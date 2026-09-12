#pragma once

#include "missile/MissileSeeker.h"
#include "missile/MissileTracking.h"

class Object;
class World;

class Missile : public Object {
	public:
		Missile() = default;
		~Missile() = default;
		void update(const World& world, float deltaTime);

		const MissileSeeker getSeeker() const{
			return seeker;
		}
	private:
		MissileSeeker seeker;
		MissileTracking tracking;
};