#version 440

layout(location = 0) in vec3 aPos;

out vec2 pos;

// uniform mat4 view;
// uniform mat4 projection;


void main() {
  // gl_Position = projection * view * vec4(aPos, 1.0);
  pos = vec2(aPos.x, aPos.y);
  gl_Position = vec4(aPos, 1.0);
}
