#include <iostream>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "object.h"

// Move constructor
Object::Object(Object&& other) noexcept :
	position(other.position),
	rotation(other.rotation),
	scale(other.scale),
	physics(std::move(other.physics)),
	VBO(other.VBO),
	VAO(other.VAO),
	EBO(other.EBO),
	vertexCount(other.vertexCount),
	indexCount(other.indexCount)
{
	other.VBO = 0;
	other.VAO = 0;
	other.EBO = 0;
	other.vertexCount = 0;
	other.indexCount = 0;
}

// Move assignment
Object& Object::operator=(Object&& other) noexcept{
	if (this != &other) {
		deleteModel();
		VAO = other.VAO;
		VBO = other.VBO;
		EBO = other.EBO;
		position = other.position;
		rotation = other.rotation;
		scale = other.scale;
		physics = std::move(other.physics);
		vertexCount = other.vertexCount;
		indexCount = other.indexCount;

		other.VAO = 0;
		other.VBO = 0;
		other.EBO = 0;
		other.vertexCount = 0;
		other.indexCount = 0;
	}
	return *this;
}

// Loads model with only one mesh currently
// Will be updated to support multiple meshes in the future
// Utilizes EBO
void Object::loadModel(const std::string& filepath) {
	deleteModel();

	// Loads model using Assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	// Set initial position
	position = { 0.0f, 0.0f, 0.0f };
	
	// Grabs first mesh
	// For now only supports one mesh per object
	// TODO: Support multiple meshes per object
	aiMesh* mesh = scene->mMeshes[0];

	bool hasNormals = mesh->HasNormals();
	bool hasColors = mesh->HasVertexColors(0);

	// Create VBO vector
	vertexCount = mesh->mNumVertices;
	std::vector<Vertex> vertices;
	vertices.reserve(vertexCount);
	for (unsigned int i = 0; i < vertexCount; i++) {
		Vertex vertex;
		vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		vertex.color = hasColors ? glm::vec4( mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a ) : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		vertex.normals = hasNormals ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : glm::vec3(0.0f, 1.0f, 0.0f);
		vertices.push_back(vertex);
	}

	// Create EBO vector
	indexCount = mesh->mNumFaces * 3;
	std::vector<unsigned int> indices;
	indices.reserve(indexCount);
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < 3; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

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

// Draws object using EBO
void Object::draw(unsigned int modelLocation) const{
	if (VAO == 0 || indexCount == 0) return;
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(glm::identity<glm::mat4>(), position);

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Object::deleteModel() {
	if (VAO) glDeleteVertexArrays(1, &VAO);
	if (VBO) glDeleteBuffers(1, &VBO);
	if (EBO) glDeleteBuffers(1, &EBO);
	VAO = VBO = EBO = 0;
	indexCount = 0;
}

