#pragma once

#include <glad/glad.h>
#include <memory>
#include <vector>
#include <memory>
#include <random>

#include "rendering/Shader.h"

class Flares;
class ParticleBucket;
class World;

class ParticleSystem {
public:
	ParticleSystem() = default;

	void draw(World& world, float aspectRatio) const; // Very unoptimized rendering/drawing

	void init();

	void updateParticles(std::mt19937& gen, World& world, float deltaTime);

	void stopEmitting(std::vector<std::unique_ptr<ParticleBucket>>& buckets);

	void clearEmittingObjs(std::vector<std::unique_ptr<ParticleBucket>>& buckets);
private:

	std::unique_ptr<Shader> particleShader = nullptr;
};