#version 440

#define lightDropoff 100000

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

in vec2 screenPos;

out vec4 out_Color;

void main() {
    float uvX = (screenPos.x + 1) / 2;
    float uvY = (screenPos.y + 1) / 2;
    vec2 uv = vec2(uvX, uvY);

    vec3 worldPos = texture(gPosition, uv).xyz;
    vec3 normal = texture(gNormal, uv).xyz;

    vec3 lightInfluence = vec3(0.1, 0.1, 0.1);
    for (int i = 0; i < 1; ++i) {
        vec3 lightToSelf = worldPos - lightPositions[i];
        float alignmentMod = max(0, -dot(normal, normalize(lightToSelf)));
        float distanceMod = 
            1 / (1 + (pow(lightToSelf.x, 2) + pow(lightToSelf.y, 2) + pow(lightToSelf.z, 2)) / lightDropoff);

        lightInfluence += alignmentMod * distanceMod * lightColors[i];
    }

    out_Color = texture(gAlbedo, uv) * vec4(lightInfluence, 1);
    // out_Color = vec4(lightColors[0], 1);
    // out_Color = vec4(uvX, uvY, 0, 1);
}
