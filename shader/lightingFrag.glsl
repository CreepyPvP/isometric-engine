#version 440

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

in vec2 screenPos;

out vec4 out_Color;

void main() {
    float uvX = (screenPos.x + 1) / 2;
    float uvY = (screenPos.y + 1) / 2;

    out_Color = texture(gNormal, vec2(uvX, uvY));
    // out_Color = vec4(uvX, uvY, 0, 1);
}
