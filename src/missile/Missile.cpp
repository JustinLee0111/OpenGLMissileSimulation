#include "missile/Missile.h"
#include <iostream>

void Missile::update(const World& world, float deltaTime) {
	seeker.update(world, *this, deltaTime);
}