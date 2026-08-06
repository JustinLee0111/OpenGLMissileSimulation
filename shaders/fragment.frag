#version 460 core

in vec4 fragmentColor;

out vec4 screenColor;

void main(){
	screenColor = vec4(fragmentColor);
}