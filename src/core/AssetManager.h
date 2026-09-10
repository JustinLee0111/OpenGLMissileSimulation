#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "rendering/Model.h"

class AssetManager {
	public:
		static std::shared_ptr<Model> loadModel(const std::string& filepath);
		static std::shared_ptr<Model> loadModel(std::string name, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec4& color);

		~AssetManager() { clearCache(); }

		std::weak_ptr<Model> getModel(const std::string& name) {
			return modelCache[name];
		}

		static void clearCache() {
			modelCache.clear();
		}
	private:
		static std::unordered_map<std::string, std::weak_ptr<Model>> modelCache; // Weak ptr to model so no memory leaks happen
};