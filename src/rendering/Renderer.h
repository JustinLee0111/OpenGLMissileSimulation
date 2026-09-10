#pragma once

#include "rendering/Shader.h"
#include <memory>

class Object;
class World;
class Lighting;

class Renderer {
	public:
		Renderer() = default;
		void rendererInit() {
			shader = std::make_unique<Shader>("shaders/vertex.vert", "shaders/fragment.frag");
			shader->use();
		}
		void draw(const World& world, float aspectRatio);
		Shader& getShader(){
			return *shader;
		}
	private:
		std::unique_ptr<Shader> shader;
};