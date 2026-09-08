#version 460 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec4 vertexColor;
layout (location = 2) in vec3 vertexNormal;

out vec4 fragmentColor;
out vec3 normal;
out vec3 curVertexPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	fragmentColor = vertexColor;
	normal = vertexNormal;
	curVertexPos = vec3(model * vec4(vertexPos, 1.0f)); // Gets the vertex positions in world space
}