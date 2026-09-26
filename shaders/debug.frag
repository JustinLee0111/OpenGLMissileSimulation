#version 460 core

in vec4 fragmentColor;

// Outputs colors in RGBA format
out vec4 screenColor;

uniform vec4 color;
uniform bool useColor;

void main(){
	vec4 finalColor = (useColor) ? color : fragmentColor;
	screenColor = finalColor;
}