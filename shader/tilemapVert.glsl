#version 440

layout(location = 0) in vec3 aPos;

out vec2 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() {
  pos = vec2(aPos.x, aPos.y);
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
