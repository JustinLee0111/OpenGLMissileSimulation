#version 460 core

in vec4 fragmentColor;

// Outputs colors in RGBA format
out vec4 screenColor;

in vec3 normal;
in vec3 curVertexPos;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform float ambientLightStrength;
uniform vec3 camPos;
uniform bool lightingEnable;

void main(){
	if(!lightingEnable){
		screenColor = fragmentColor;
		return;
	}
	vec3 norm = normalize(normal); // Renormalizing the normal for redundancy/accuracy
	vec3 vertexToCamNorm = normalize(camPos - curVertexPos); // Normalized vector from vertex -> cam
	vec3 lightToVertexNorm = normalize(curVertexPos - lightPos); // Normalized vector from light source -> vertex

	// Specular lighting
	vec3 reflection = reflect(lightToVertexNorm, norm);
	float specStrength = pow(max(dot(vertexToCamNorm, reflection), 0.0f), 12.0f);
	float specScaling = 0.5f;
	float finalSpecular = specStrength * specScaling;

	// Diffuse lighting
	float diffuseStrength = max(dot(-lightToVertexNorm, norm), 0.0f);

	screenColor = vec4(fragmentColor) * lightColor * (diffuseStrength + ambientLightStrength + finalSpecular);
}