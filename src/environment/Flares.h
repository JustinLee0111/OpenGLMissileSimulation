#pragma once

#include <array>

#include "environment/ParticleBucket.h"
#include "environment/FlareData.h"

enum class DeployType {
	SINGLE,
	BURST,
	CONTINUOUS
};

// Each single deployment of flares deploys two flares, one left and one right
class Flares{
	public:
		Flares(){
			model = std::make_shared<Model>("assets/models/sphere.obj");
		}

		void spawnDirectionalParticle(std::mt19937& gen, glm::vec3 origin, glm::vec3 direction, glm::vec3 initVel = glm::vec3{ 0.0f });

		void deployFlares(std::mt19937& gen, glm::vec3 origin, glm::vec3 direction, glm::vec3 initVel = glm::vec3{ 0.0f });

		DeployType deployType = DeployType::SINGLE;

		std::array<FlareData, 100> particles; // An array of particles

		glm::vec3 scale{ 0.3f, 0.3f, 0.3f };

		bool isDeploying = false;
		
		float deploySpeed{ 10.0f };
		float lifeTime{ 15.0f };
		float lastSpawnDeltaT{ 0.0f };
		float velVariance{ 0.1f };

		float contDeployGap{ 1.5f }; // Gap between deployment of continuous flares
		float burstDeployGap{ 0.25f }; // Gap between deployment of burst flares
		int burstCount{ 4 }; // How many sets of flares to drop per burst

		std::shared_ptr<Model> model = nullptr;
};