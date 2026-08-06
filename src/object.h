#pragma once

#include <string>
#include <glm/glm.hpp>
#include<memory>

class Physics;

class Object {
public:
	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };
	glm::vec3 scale{ 1.0f };

	std::unique_ptr<Physics> physics;

	Object(const std::string& filepath);
	~Object();

	// Removes copying and assignment
	Object(const Object&) = delete;            
	Object& operator=(const Object&) = delete;

	// Move & assignment constructor
	Object(Object&& other) noexcept;            
	Object& operator=(Object&& other) noexcept;

	void loadModel(const std::string& filepath);
	void deleteModel();
	void draw(unsigned int modelLocation) const;
	void addPhysics();
	void removePhysics();
private:
	unsigned int VBO, VAO, EBO, vertexCount, indexCount;
};