#pragma once

#include "imgui.h"

class Missile;

class MissileHud {
public:
	static void MissileHUD(std::vector<Missile*>& missiles);
};