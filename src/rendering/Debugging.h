#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

class Model;
class Shader;
class World;

struct VertexData {
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	glm::vec4 color;
	VertexData(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices, glm::vec4 color) : vertices(vertices), indices(indices), color(color) {}
};

class Debugging {
	public:
		void Init(World* world, float aspectRatio);
		static void cone(glm::vec3 position, glm::vec3 direction);
	private:
		static void createCone(float angle, float length, glm::vec4 color);
		static void drawWrapper();
		static std::unique_ptr<Shader> debugShader;
		static std::shared_ptr<Model> debugCone;
		static World* world;
		static float aspectRatio;
};