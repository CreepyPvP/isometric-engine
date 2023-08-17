#version 440 core

uniform vec4 lightPos;

in vec3 worldPos;

out float lightToPixelDistance;

void main() {
    vec3 lightToVertex = worldPos - lightPos.xyz;
    lightToPixelDistance = length(lightToVertex);
}
