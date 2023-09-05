#version 440

#define shadowBias 0.015

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform vec2 gScreenSize;

uniform samplerCube pointLightMap;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 attenuation;

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
    vec2 uv = gl_FragCoord.xy / gScreenSize;

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
    float attenuation = 
        attenuation.x + 
        distance * attenuation.y + 
        distance * distance * attenuation.z;

    vec3 lightInfluence = shadowMod * 
        (diffuseIntensity + specularIntensity) /
        attenuation * 
        lightColor;
    lightInfluence = clamp(lightInfluence, 0, 1);

    out_Color = vec4(texture(gAlbedo, uv).xyz * lightInfluence, 1);
    // out_Color = (out_Color + vec4(lightColor, 1)) / 2;
}
