#include <memory>
#include <string>
#include <utility>
#include <iostream>

#include "environment/World.h"
#include "core/AssetManager.h"
#include "core/LevelManager.h"
#include "missile/Missile.h"
#include "environment/Flares.h"

Camera* World::currentCamera = nullptr;

void World::worldInit() {
	setAmbientLightColor(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	createLight(glm::vec3{ 2.0f, 3.0f, 2.0f }, glm::vec4{0.5f, 0.5f, 0.5f, 0.5f});

	particleSystem.init();

	std::unique_ptr<ParticleBucket> missileSmoke = std::make_unique<ParticleBucket>("missileSmoke");
	missileSmoke->carryVelocity = true;
	missileSmoke->particleSpeed = 100.0f;
	missileSmoke->lifeTime = 0.15f;
	missileSmoke->velVariance = 0.1f;
	missileSmoke->spawnDelay = 0.01f;
	particleBuckets.push_back(std::move(missileSmoke));

	std::unique_ptr<ParticleBucket> missileExplosion = std::make_unique<ParticleBucket>("missileExplosion");
	missileExplosion->emissionType = EmitType::OMNIDIRECTIONAL;
	missileExplosion->particleSpeed = 75.0f;
	missileExplosion->lifeTime = 1.0f;
	particleBuckets.push_back(std::move(missileExplosion));

	flareBucket = std::make_unique<Flares>();

	loadLevel("assets/levels/MissileSim.json"); // Load default level

	missiles[0]->findRandomTarget(*this);
}

// Creates an object and constructs it based on inputted parameters
Object* World::spawnObject(const std::string& filepath) {
	auto obj = std::make_unique<Object>();
	obj->model = AssetManager::loadModel(filepath);
	Object* ptr = obj.get();
	ptr->addPhysics();
	physicsEngine.addPhysObject(ptr);

	objects.push_back(std::move(obj));
	return ptr;
}

Missile* World::spawnMissile(const std::string& filepath) {
	auto obj = std::make_unique<Missile>();
	obj->model = AssetManager::loadModel(filepath);

	for (auto& bucket : particleBuckets) {
		ParticleBucket* missileSmoke = getParticleBucket("missileSmoke");
		if (missileSmoke) {
			missileSmoke->addObj(obj.get());
			obj->setParticles(*bucket); 
			break;
		}	
	}

	Missile* ptr = obj.get();
	ptr->addPhysics();
	physicsEngine.addPhysObject(ptr);

	objects.push_back(std::move(obj));
	missiles.push_back(ptr);
	return ptr;
}

void World::loadLevel(const std::string& filepath) {
	LevelManager::loadLevel(filepath, *this);
	if (!missiles.empty()) {
		for (auto& camera : cameras) {
			if (camera->chase) {
				camera->chaseObject = missiles[0];
			}
		}
		missiles[0]->findRandomTarget(*this);
	}
}

// Doesn't unload world lights
void World::unloadLevel() {
	particleSystem.stopEmitting(particleBuckets);
	particleSystem.clearEmittingObjs(particleBuckets);
	objects.clear();
	cameras.clear();
	missiles.clear();
	physicsEngine.physObjects.clear();
	AssetManager::clearCache();
	currentCamera = nullptr;
	for (auto& camera : cameras) {
		if (camera->chase) {
			camera->chaseObject = nullptr;
		}
	}
}

Camera* World::createCamera(glm::vec3 cameraPosition) {
	auto camera = std::make_unique<Camera>(cameraPosition);
	Camera* cameraPtr = camera.get();
	if (!currentCamera) currentCamera = cameraPtr;
	cameras.push_back(std::move(camera));
	return cameraPtr;
}

Light* World::createLight(glm::vec3 lightPos, glm::vec4 lightColor) {
	auto light = std::make_unique<Light>(lightPos, lightColor);
	Light* lightPtr = light.get();
	lights.push_back(std::move(light));
	return lightPtr;
}

void World::fixedUpdate(std::mt19937& gen){
	if (!objects.empty()) {
		physicsEngine.update();
	}
	for (auto& missile : missiles) {
		if (missile) {
			missile->update(*this, physicsEngine.getPhysicsRate());
		}
	}
	for (auto& missile : missiles) {
		if(missile->proximityFuseTrig(*this)) {
			ParticleBucket* explosionBucket = getParticleBucket("missileExplosion");
			if (explosionBucket) {
				explosionBucket->emitParticles(missile->position);
				for (auto& camera : cameras) {
					if (camera->chase) {
						camera->chaseObject = nullptr;
					}
				}
				deleteObj(missile->objName);
				for (auto& obj : objects) {
					if (obj->health <= 0.0f) {
						deleteObj(obj->objName);
					}
				}
			}
		}
	}
	for (auto& particleBucket : particleBuckets) {
		particleSystem.updateParticles(gen, *this, physicsEngine.getPhysicsRate());
	}
}

void World::update() {
	if (currentCamera->chaseObject && currentCamera && currentCamera->chase) {
		currentCamera->cameraChaseUpdate();
	}
}

ParticleBucket* World::getParticleBucket(std::string bucketName) {
	for (auto& bucket : particleBuckets) {
		if (bucket->bucketName == bucketName) {
			return bucket.get();
		}
	}
	return nullptr;
}

void World::deleteObj(std::string deleteName) {
	std::erase_if(missiles, [deleteName](Missile* missile) {return missile->objName == deleteName; });
	std::erase_if(physicsEngine.physObjects, [deleteName](Object* obj) {return obj->objName == deleteName; });
	std::erase_if(getParticleBucket("missileSmoke")->getEmitObjs(), [deleteName](Object* obj) {return obj->objName == deleteName; });
	std::erase_if(objects, [deleteName](std::unique_ptr<Object>& obj) {return obj->objName == deleteName; });
}