#include "world.h"

Object* World::spawnObject(const std::string& filepath, bool enablePhysics, bool enableGravity, bool enableCollisions, bool isKinematic) {
	auto obj = std::make_unique<Object>(filepath);
	Object* ptr = obj.get();

	if (enablePhysics) {
		ptr->addPhysics(enableGravity, enableCollisions, isKinematic);
		physicsEngine.addPhysObject(ptr);
	}

	objects.push_back(std::move(obj));
	return ptr;
}