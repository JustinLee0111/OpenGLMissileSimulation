#pragma once

#include <string>

class LevelManager {
public:
	static int loadLevel(const int levelIndex, World& world);
	static void unloadLevel(World& world);

	static inline std::unordered_map<int, std::string> levelMap{
		{0, "assets/levels/MissileSim.json"},
		{1, "assets/levels/MissileTailing.json"},
		{2, "assets/levels/MissileSharpTurn.json"},
		{3, "assets/levels/MissileSimStill.json"},
		{4, "assets/levels/SideAspect.json"},
		{5, "assets/levels/ChaseCamTest.json"},
		{6, "assets/levels/PhysicsSimWorldData.json"},
		{7, "assets/levels/PhysicsTesting.json"}
	};

	static inline int currentLevel{ -1 };
	static inline bool reloadLevel{ false };
};