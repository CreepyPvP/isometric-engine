#version 440

uniform sampler2D tileset;

in vec3 worldPos;
in vec3 normal;
in vec2 uv;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

void main() {
    gPosition = worldPos;
    gNormal = normalize(normal);
    gAlbedo = texture(tileset, uv).xyz;
}
