#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <random>
#include <ctime>
#include <array>

#include "rendering/Model.h"
#include "rendering/Shader.h"
#include "environment/ParticleData.h"

class World;

class ParticleSystem {
	public:
		ParticleSystem() {
			model = std::make_shared<Model>("assets/models/sphere.obj");
			particleShader->use();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		static void init();

		void draw(World& world, float aspectRatio);

		void emitParticles(glm::vec3 origin, glm::vec3 direction);

		void updateParticles(float deltaTime);

		void spawnParticle(float deltaTime);

		bool checkActive();

		void stopEmit();

		glm::vec3 origin{ 0.0f };
		glm::vec3 direction{ 0.0f };

		float velVariance{ 0.05f };
		glm::vec3 scale{ 0.05f, 0.05f, 0.05f };

		float particleSpeed{ 100.0f };
		float lifeTime{ 0.2f };
		float lastSpawnDeltaT{ 0.0f };

		std::array<ParticleData, 1000> particles; // An array of particles, max of 1000 particles at a given time
	private:
		inline static std::mt19937 gen{ (unsigned int)std::time(0) };
		std::shared_ptr<Model> model;
		
		bool IsEmitting = false;
		float spawnDelay = 0.001f; // Delay between particle spawns in seconds
		static std::unique_ptr<Shader> particleShader;
};