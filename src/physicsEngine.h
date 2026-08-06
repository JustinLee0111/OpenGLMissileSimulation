#pragma once

#include <vector>

class Object;

class physicsEngine {
public:
	void update(std::vector<Object>& objects, float deltaTime);
};