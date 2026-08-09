#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "physics.h"

class Object {
public:
	struct Vertex {
		glm::vec3 position;
		glm::vec4 color;
		glm::vec3 normals;
	};

	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };
	glm::vec3 scale{ 1.0f };

	std::unique_ptr<Physics> physics;

	Object(const std::string& filepath) {
		loadModel(filepath);
	}
	~Object() {
		deleteModel();
	}
	// Removes copying and assignment
	Object(const Object&) = delete;            
	Object& operator=(const Object&) = delete;
	// Move & assignment constructor
	Object(Object&& other) noexcept;            
	Object& operator=(Object&& other) noexcept;

	void loadModel(const std::string& filepath);
	void deleteModel();
	void draw(unsigned int modelLocation) const;

	void addPhysics(bool enablePhysics, bool enableCollisions, bool isKinematic) {
		if (!physics) physics = std::make_unique<Physics>(enablePhysics, enableCollisions, isKinematic);
	}
	void removePhysics() {
		if (physics) physics = nullptr;
	}
private:
	unsigned int VBO, VAO, EBO, vertexCount, indexCount;
};