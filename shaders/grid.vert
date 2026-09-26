#version 460 core

out vec3 vertWorldPos;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 camPos;

uniform float gridSize = 400.0f;

const vec3 vertices[4] = vec3[4](
	vec3(-1.0f, 0.0f, -1.0f),
	vec3(1.0f, 0.0f, -1.0f),
	vec3(1.0f, 0.0f, 1.0f),
	vec3(-1.0f, 0.0f, 1.0f)
);

const int indices[6] = int[6]( 0, 2, 1 ,2 ,0, 3 );

void main(){
	int index = indices[gl_VertexID];
	vec3 vertPos = vertices[index] * gridSize;

	vertPos.x += camPos.x;
	vertPos.z += camPos.z;

	gl_Position = projection * view * vec4(vertPos, 1.0f);
	vertWorldPos = vertPos;
}