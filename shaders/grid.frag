#version 460 core

out vec4 fragmentColor;

in vec3 vertWorldPos;

uniform float gridCellSize = 8.0f;
uniform vec4 gridThick = vec4(0.0f, 0.0f, 0.0f, 1.0f);
uniform float lineThickness = 0.1f;

void main(){
	float tempDist = mod(vertWorldPos.z, gridCellSize);
	float closestLineDistZ = (gridCellSize - tempDist <= lineThickness) ? gridCellSize - tempDist : tempDist;
	float lineAlphaZ = 1.0f - ( closestLineDistZ / lineThickness );
	lineAlphaZ = clamp( lineAlphaZ, 0.0f, 1.0f );

	tempDist = mod(vertWorldPos.x, gridCellSize);
	float closestLineDistX = (gridCellSize - tempDist <= lineThickness) ? gridCellSize - tempDist : tempDist;
	float lineAlphaX = 1.0f - ( closestLineDistX / lineThickness );
	lineAlphaX = clamp( lineAlphaX, 0.0f, 1.0f );

	vec4 color = gridThick;
	color.a *= max(lineAlphaZ, lineAlphaX);
	fragmentColor = color;
}