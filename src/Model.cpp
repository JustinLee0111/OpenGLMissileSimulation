#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "Model.h"

// Loads model with all meshes
Model::Model(const std::string& filepath) {
	deleteModel();

	// Loads model using Assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	// Grabs meshes
	aiMesh* currentMesh = nullptr;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) { // Goes through all meshes and adds it to the meshes vector
		currentMesh = scene->mMeshes[i];

		bool hasNormals = currentMesh->HasNormals();
		bool hasColors = currentMesh->HasVertexColors(0);

		// Create VBO vector
		unsigned int vertexCount = currentMesh->mNumVertices;
		std::vector<Vertex> vertices;
		vertices.reserve(vertexCount);
		for (unsigned int i = 0; i < vertexCount; i++) {
			Vertex vertex;
			vertex.position = { currentMesh->mVertices[i].x, currentMesh->mVertices[i].y, currentMesh->mVertices[i].z };
			vertex.color = hasColors ? glm::vec4(currentMesh->mColors[0][i].r, currentMesh->mColors[0][i].g, currentMesh->mColors[0][i].b, currentMesh->mColors[0][i].a) : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			vertex.normals = hasNormals ? glm::vec3(currentMesh->mNormals[i].x, currentMesh->mNormals[i].y, currentMesh->mNormals[i].z) : glm::vec3(0.0f, 1.0f, 0.0f);
			vertices.push_back(vertex);
		}

		// Create EBO vector
		unsigned int indexCount = currentMesh->mNumFaces * 3;
		std::vector<unsigned int> indices;
		indices.reserve(indexCount);
		for (unsigned int i = 0; i < currentMesh->mNumFaces; i++) {
			aiFace face = currentMesh->mFaces[i];
			for (unsigned int j = 0; j < 3; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		meshes.push_back(Mesh(vertices, indices));
	}
}

void Model::draw() const {
	for (const Mesh& mesh : meshes) {
		mesh.draw();
	}
}

void Model::deleteModel() {
	meshes.clear();
}