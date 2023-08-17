#version 440 core

layout (location = 0) in vec3 aPos;

uniform mat4 lightSpace;
uniform mat4 model;

out vec3 worldPos;

void main() {
    vec4 worldSpace = model * vec4(aPos, 1);
    worldPos = worldSpace.xyz;
    gl_Position = lightSpace * worldSpace;
}
