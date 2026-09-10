#include "core/AssetManager.h"

std::unordered_map<std::string, std::weak_ptr<Model>> AssetManager::modelCache;

std::shared_ptr<Model> AssetManager::loadModel(const std::string& filepath) {
	auto it = modelCache.find(filepath);
	if (it != modelCache.end()) {
		return it->second.lock();
	}
	std::shared_ptr<Model> model = std::make_shared<Model>(filepath);
	modelCache[filepath] = model;
	return model;
}

std::shared_ptr<Model> AssetManager::loadModel(std::string name, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec4& color) {
	auto it = modelCache.find(name);
	if (it != modelCache.end()) {
		return it->second.lock();
	}
	std::shared_ptr<Model> model = std::make_shared<Model>(vertices, indices, color);
	modelCache[name] = model;
	return model;
}