#pragma once

#include "Shader.h"
#include <vector>
#include <memory>

class Object;
class World;

class Renderer {
	public:
		Renderer() = default;
		void rendererInit() {
			shader = std::make_unique<Shader>("shaders/vertex.vert", "shaders/fragment.frag");
		}
		void draw(const World& world, float aspectRatio);
	private:
		std::unique_ptr<Shader> shader;
};