#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 uv;

uniform mat4 view;
uniform mat4 projection;

out vec2 uvCoords;

void main() {
  uvCoords = uv;
  gl_Position = projection * view * vec4(aPos, 1.0);
}
