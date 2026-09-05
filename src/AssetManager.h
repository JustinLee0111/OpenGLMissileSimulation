#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "Model.h"

class AssetManager {
	public:
		static std::shared_ptr<Model> loadModel(const std::string& filepath);

		~AssetManager() { clearCache(); }

		static void clearCache() {
			modelCache.clear();
		}
	private:
		static std::unordered_map<std::string, std::weak_ptr<Model>> modelCache; // Weak ptr to model so no memory leaks happen
};