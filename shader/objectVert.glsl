#version 440

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;

out vec3 worldPos;
out vec3 normal;
out vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() {
    vec4 world = model * vec4(aPos, 1);
    worldPos = world.xyz;
    normal = (model * vec4(aNormal, 0.0)).xyz;
    uv = aUv;
    gl_Position = projection * view * world;
}
