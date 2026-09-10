#pragma once

#include <glm/glm.hpp>

#include <vector>

class Shader;

struct Vertex {
	glm::vec3 position;
	glm::vec4 color;
	glm::vec3 normals;
};

class Mesh {
	public:
		bool isTransparent = false;
		Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		// Move & assignment constructor
		Mesh(Mesh&& other) noexcept;
		Mesh& operator=(Mesh&& other) noexcept;

		void draw() const;
	private:
		unsigned int VBO = 0;
		unsigned int VAO = 0;
		unsigned int EBO = 0;
		unsigned int indexCount = 0;
};