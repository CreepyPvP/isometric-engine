#version 440

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec2 modelPos;
out vec3 worldPos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() {
  modelPos = vec2(aPos.x, aPos.y);
  vec4 worldPosition = model * vec4(aPos, 1.0);
  worldPos = worldPosition.xyz;
  normal = (model * vec4(aNormal, 0.0)).xyz;
  gl_Position = projection * view * worldPosition;
}
