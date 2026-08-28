#pragma once

#include <string>
#include <vector>

#include "Mesh.h"

class Shader;

class Model {
	public:
		Model(const std::string& filepath);
		void draw() const;
		void deleteModel();
	private:
		std::vector<Mesh> meshes;
};