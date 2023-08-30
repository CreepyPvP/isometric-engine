#version 440

layout(location = 0) in vec3 aPos;

out vec2 screenPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() {
    gl_Position = projection * view * model * vec4(aPos, 1);
    screenPos = vec2(
        gl_Position.x / gl_Position.w, 
        gl_Position.y / gl_Position.w
    );
}
