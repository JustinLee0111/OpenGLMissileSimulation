#version 460 core

in vec4 fragmentColor;

// Outputs colors in RGBA format
out vec4 screenColor;

void main(){
	screenColor = vec4(fragmentColor);
}