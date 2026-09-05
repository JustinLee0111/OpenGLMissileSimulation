#include "World.h"
#include "AssetManager.h"
#include "InputHandler.h"
#include "LevelManager.h"

Camera* World::currentCamera = nullptr;

// Creates an object and constructs it based on inputted parameters
Object* World::spawnObject(const std::string& filepath, bool enablePhysics, bool enableGravity, bool enableCollisions, bool isKinematic) {
	auto obj = std::make_unique<Object>();
	obj->model = AssetManager::loadModel(filepath);
	Object* ptr = obj.get();

	if (enablePhysics) {
		ptr->addPhysics(enableGravity, enableCollisions, isKinematic);
		physicsEngine.addPhysObject(ptr);
	}

	objects.push_back(std::move(obj));
	return ptr;
}

void World::loadLevel(const std::string& filepath) {
	LevelManager::loadLevel(filepath, *this);
}

void World::unloadLevel() {
	objects.clear();
	cameras.clear();
	physicsEngine.physObjects.clear();
	AssetManager::clearCache();
	currentCamera = nullptr;
}

Camera* World::createCamera() {
	auto camera = std::make_unique<Camera>();
	Camera* cameraPtr = camera.get();
	if (!currentCamera) currentCamera = cameraPtr;
	cameras.push_back(std::move(camera));
	return cameraPtr;
}

void World::update(const InputHandler& inputs){
	physicsEngine.update();
}