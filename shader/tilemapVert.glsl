#version 440

layout(location = 0) in vec3 aPos;

out vec2 modelPos;
out vec3 worldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() {
  modelPos = vec2(aPos.x, aPos.y);
  vec4 worldPosition = model * vec4(aPos, 1.0);
  worldPos = worldPosition.xyz;
  gl_Position = projection * view * worldPosition;
}
