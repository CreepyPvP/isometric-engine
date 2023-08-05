#version 440

#define byte unsigned char
#define tilesetWidth 10
#define tilesetHeight 5
#define tileSize 32

uniform sampler2D tileset;
uniform int width;
uniform int height;
uniform int tileDataOffset;

layout(binding = 1) readonly buffer InputData {
    byte tiles[];
};

in vec2 modelPos;
in vec3 worldPos;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

void main() {
    float tilemapX = modelPos.x * width;
    float tilemapY = modelPos.y * height;

    int gridX = int(floor(tilemapX));
    int gridY = int(floor(tilemapY));

    int tileIndex = gridX + gridY * width;
    int tileValue = int(tiles[tileIndex + tileDataOffset]);

    int tileX = int(mod(tileValue, tilesetWidth));
    int tileY = (tileValue - tileX) / tilesetWidth;
    float tileBaseUVX = float(tileX) / float(tilesetWidth);
    float tileBaseUVY = float(tileY) / float(tilesetHeight);

    float tileUVX = mod(tilemapX, 1);
    float tileUVY = mod(tilemapY, 1);

    vec2 uv = vec2(
        tileBaseUVX + (tileUVX / float(tilesetWidth)),
        tileBaseUVY + (tileUVY / float(tilesetHeight))
    );

    // TODO: fill out properly
    gPosition = worldPos;
    gNormal = vec3(1, 0, 0);
	gAlbedo = texture(tileset, uv).xyz;
	// gAlbedo = vec3(uv, 0.0);
}
