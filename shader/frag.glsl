#version 330 core

in vec2 uvCoords;

out vec4 out_Color;

uniform sampler2D tex_diffuse;

void main() {
	out_Color = texture(tex_diffuse, uvCoords);
}
