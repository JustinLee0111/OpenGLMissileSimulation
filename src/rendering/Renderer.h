#pragma once

#include "rendering/Shader.h"
#include <memory>

class Object;
class World;
class Lighting;

class Renderer {
	public:
		Renderer() = default;
		void rendererInit(float windowWidth, float windowHeight);
		void draw(const World& world, float windowWidth, float windowHeight);
		void gridInit();
		void drawParticles(const World& world, float windowWidth, float windowHeight);
		Shader& getShader(){
			return *shader;
		}
		unsigned int getFrameBuffer() {
			return frameBuffer;
		}
	private:
		std::unique_ptr<Shader> shader;
		std::unique_ptr<Shader> gridShader;
		unsigned int gridVAO = 0;
		unsigned int frameBuffer = 0;
		unsigned int colorRenderBuffer = 0;
		unsigned int depthRenderBuffer = 0;
};