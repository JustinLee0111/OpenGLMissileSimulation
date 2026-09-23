#pragma once

#include <glad/glad.h>
#include <memory>
#include <random>
#include <ctime>
#include <vector>
#include <memory>

#include "rendering/Shader.h"

class ParticleBucket;
class World;

class ParticleSystem {
public:
	ParticleSystem() = default;

	void draw(World& world, float aspectRatio) const; // Very unoptimized rendering/drawing

	void init();

	void updateParticles(std::vector<std::unique_ptr<ParticleBucket>>& buckets, float deltaTime);

	void stopEmitting(std::vector<std::unique_ptr<ParticleBucket>>& buckets);

	void clearEmittingObjs(std::vector<std::unique_ptr<ParticleBucket>>& buckets);
private:
	std::mt19937 gen{ (unsigned int)std::time(0) };

	std::unique_ptr<Shader> particleShader = nullptr;
};