#include "environment/World.h"
#include "core/AssetManager.h"
#include "core/InputHandler.h"
#include "core/LevelManager.h"

Camera* World::currentCamera = nullptr;

void World::worldInit() {
	setAmbientLightColor(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	createLight(glm::vec3{ 2.0f, 3.0f, 2.0f }, glm::vec4{0.5f, 0.5f, 0.5f, 0.5f});
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
	Missile* ptr = obj.get();
	ptr->addPhysics();
	physicsEngine.addPhysObject(ptr);

	missiles.push_back(std::move(obj));
	return ptr;
}

/*Object* World::spawnObject(std::string name, VertexData& vertexData) {
	auto obj = std::make_unique<Object>();
	obj->model = AssetManager::loadModel(name, vertexData.vertices, vertexData.indices, vertexData.color);
	Object* ptr = obj.get();
	ptr->addPhysics();
	physicsEngine.addPhysObject(ptr);

	objects.push_back(std::move(obj));
	return ptr;
}*/

void World::loadLevel(const std::string& filepath) {
	LevelManager::loadLevel(filepath, *this);
}

// Doesn't unload lights currently
void World::unloadLevel() {
	objects.clear();
	cameras.clear();
	missiles.clear();
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

void World::update(){
	physicsEngine.update();
	for (auto& missile : missiles) {
		missile->update(*this, physicsEngine.getPhysicsRate());
	}
}