#pragma once

#include <string>
#include <vector>

#include "rendering/Mesh.h"

class Shader;

class Model {
	public:
		Model(const std::string& filepath);
		Model(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec4& color);
		void drawTransparent() const;
		void drawOpaque() const;
		void deleteModel();
	private:
		std::vector<Mesh> meshes;
};