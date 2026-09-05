#pragma once

#include <string>

class LevelManager {
public:
	static void loadLevel(const std::string& filepath, World& world);
};