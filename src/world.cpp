#include "World.h"
#include "AssetManager.h"

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

Camera* World::createCamera(float windowWidth, float windowHeight) {
	auto camera = std::make_unique<Camera>(windowWidth, windowHeight);
	Camera* cameraPtr = camera.get();
	if (cameras.empty()) currentCamera = cameraPtr;
	cameras.push_back(std::move(camera));
	return cameraPtr;
}

void World::update() {
	physicsEngine.update();
}

// Currently only for one camera
void World::draw() const {
	shader.use();
	// Upload view and projection
	shader.setMat4("view", currentCamera->view);
	shader.setMat4("projection", currentCamera->projection);

	for (const auto& obj : objects) {
		if (obj->model) {
			shader.setMat4("model", obj->getObjectMatrix());
			obj->draw();
		}

	}
}