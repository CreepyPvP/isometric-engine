#version 440

#define shadowBias 0.015

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

uniform samplerCube pointLightMap;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

in vec2 screenPos;
out vec4 out_Color;

float calcShadowPointLight(vec3 lightToPixel) {
    float distance = length(lightToPixel);
    lightToPixel.y = -lightToPixel.y;
    lightToPixel.z = -lightToPixel.z;
    float sampledDistance = texture(pointLightMap, lightToPixel).r;
    float shadow = distance - shadowBias > sampledDistance ? 1.0 : 0.0;
    return shadow;
}

void main() {
    float uvX = (screenPos.x + 1) / 2;
    float uvY = (screenPos.y + 1) / 2;
    vec2 uv = vec2(uvX, uvY);

    vec4 worldPos = texture(gPosition, uv);
    vec3 normal = texture(gNormal, uv).xyz;

    vec3 lightDirection = lightPos - worldPos.xyz;
    vec3 unitLightDirection = normalize(lightDirection);
    vec3 eyeDirection = normalize(-(worldPos.xyz - cameraPos));
    vec3 reflectedDirection = normalize(-reflect(unitLightDirection, normal));

    float diffuseIntensity = max(dot(normal, unitLightDirection), 0);
    float specularIntensity = max(dot(reflectedDirection, eyeDirection), 0);

    float shadowMod = 1 - calcShadowPointLight(-lightDirection);

    float distance = length(lightDirection);
    const float constant = 2;
    const float linear = 0.2;
    const float quadratic = 0.05;
    float attenuation = constant + distance * linear + distance * distance * quadratic;

    vec3 lightInfluence = shadowMod * 
        (diffuseIntensity / attenuation + specularIntensity) * 
        lightColor;
    lightInfluence = clamp(lightInfluence, 0, 1);

    out_Color = vec4(texture(gAlbedo, uv).xyz * lightInfluence, 1);
}
