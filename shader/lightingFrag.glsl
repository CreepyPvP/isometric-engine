#version 440

#define lightDropoff 100
#define shadowBias 0.015

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D shadowMap;
uniform samplerCube cubemapShadow;

// struct Light {
//     vec3 color;
//     vec4 position;
//
//     mat4 transform;
//     sampler2D shadowMap;
//
//     // attenuation
//     float constant;
//     float linear;
//     float quadratic;
// };

uniform vec3 lightPositions[1];
uniform vec3 lightColors[1];
uniform mat4 lightSpace;

uniform vec3 cameraPos;

in vec2 screenPos;
out vec4 out_Color;

// Directional lighting
// float shadowCalculation(vec4 fragPosLightSpace) {
//     vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//     projCoords = projCoords * 0.5 + 0.5;
//     float closestDepth = texture(shadowMap, projCoords.xy).r;
//     float currentDepth = projCoords.z;
//     float shadow = currentDepth - shadowBias > closestDepth ? 1.0 : 0.0;
//
//     return shadow;
// }

float calcShadowPointLight(vec3 lightToPixel) {
    float distance = length(lightToPixel);
    lightToPixel.y = -lightToPixel.y;
    float sampledDistance = texture(cubemapShadow, lightToPixel).r;
    float shadow = distance - shadowBias > sampledDistance ? 1.0 : 0.0;
    return shadow;
}

void main() {
    float uvX = (screenPos.x + 1) / 2;
    float uvY = (screenPos.y + 1) / 2;
    vec2 uv = vec2(uvX, uvY);

    vec4 worldPos = texture(gPosition, uv);
    // vec3 worldPos3 = worldPos.xyz;
    vec3 normal = texture(gNormal, uv).xyz;

    vec3 lightInfluence = vec3(0.1, 0.1, 0.1);
    for (int i = 0; i < 1; ++i) {
        vec3 lightDirection = lightPositions[i].xyz - worldPos.xyz;
        vec3 unitLightDirection = normalize(lightDirection);
        vec3 eyeDirection = normalize(-(worldPos.xyz - cameraPos));
        vec3 reflectedDirection = normalize(-reflect(unitLightDirection, normal));

        float diffuseIntensity = max(dot(normal, unitLightDirection), 0);
        float specularIntensity = max(dot(reflectedDirection, eyeDirection), 0);

        // float shadowMod = 1 - shadowCalculation(lightSpace * worldPos);
        float shadowMod = 1 - calcShadowPointLight(-lightDirection);

        float distance = length(lightDirection);
        const float constant = 2;
        const float linear = 0.2;
        const float quadratic = 0.05;
        float attenuation = constant + distance * linear + distance * distance * quadratic;

        lightInfluence += min((diffuseIntensity + specularIntensity) / attenuation, 1) * shadowMod * lightColors[i];

        // lightInfluence += shadowMod * lightColors[i];
    }
    lightInfluence = clamp(lightInfluence, 0, 1);

    // vec3 lightInfluence = vec3(0.1, 0.1, 0.1);
    // float shadowMod = 1 - shadowCalculation(lightSpace * worldPos);
    // for (int i = 0; i < 1; ++i) {
    //     vec3 lightToSelf = worldPos3 - lightPositions[i];
    //     float alignmentMod = max(0, -dot(normal, normalize(lightToSelf)));
    //     float distanceMod = 
    //         1 / (1 + (pow(lightToSelf.x, 2) + pow(lightToSelf.y, 2) + pow(lightToSelf.z, 2)) / lightDropoff);
    //
    //     lightInfluence += shadowMod * alignmentMod * distanceMod * lightColors[i];
    // }
    


    out_Color = vec4(texture(gAlbedo, uv).xyz * lightInfluence, 1);
    // out_Color = texture(gPosition, uv);
    // out_Color = vec4(cameraPos, 1);
}
