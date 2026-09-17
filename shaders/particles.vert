#version 460 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec4 vertexColor;
layout (location = 2) in vec3 vertexNormal;

out vec4 fragmentColor;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 position;
uniform vec3 scale;


void main(){
	vec3 worldPosition = position + (vertexPos * scale);

	gl_Position = projection * view * vec4(worldPosition, 1.0);
	fragmentColor = vertexColor;
}