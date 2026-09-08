#include "World.h"
#include "AssetManager.h"
#include "InputHandler.h"
#include "LevelManager.h"

Camera* World::currentCamera = nullptr;

void World::worldInit() {
	setAmbientLightColor(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	createLight(glm::vec3{ 2.0f, 3.0f, 2.0f }, glm::vec4{0.5f, 0.5f, 0.5f, 0.5f});
}

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

// Doesn't unload lights currently
void World::unloadLevel() {
	objects.clear();
	cameras.clear();
	physicsEngine.physObjects.clear();
	AssetManager::clearCache();
	currentCamera = nullptr;
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

void World::update(const InputHandler& inputs){
	physicsEngine.update();
}