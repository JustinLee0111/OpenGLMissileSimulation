#include <glad/glad.h>

#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
	indexCount = (unsigned int)indices.size();
	// Generate buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normals));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

Mesh::~Mesh() {
	if (VAO) glDeleteVertexArrays(1, &VAO);
	if (VBO) glDeleteBuffers(1, &VBO);
	if (EBO) glDeleteBuffers(1, &EBO);
	VAO = VBO = EBO = 0;
	indexCount = 0;
}

// Move constructor
Mesh::Mesh(Mesh&& other) noexcept :
	VBO(other.VBO),
	VAO(other.VAO),
	EBO(other.EBO),
	indexCount(other.indexCount)
{
	other.VBO = 0;
	other.VAO = 0;
	other.EBO = 0;
	other.indexCount = 0;
}

// Move assignment
Mesh& Mesh::operator=(Mesh&& other) noexcept {
	if (this != &other) {
		VAO = other.VAO;
		VBO = other.VBO;
		EBO = other.EBO;
		indexCount = other.indexCount;

		other.VAO = 0;
		other.VBO = 0;
		other.EBO = 0;
		other.indexCount = 0;
	}
	return *this;
}

void Mesh::draw() const{
	if (VAO == 0 || indexCount == 0) return;
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}