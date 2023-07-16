#version 330 core

in vec2 uvCoords;

out vec4 out_Color;

void main() {
	out_Color = vec4(uvCoords, 0.0f, 0.0f);
}
