#version 440

#define byte unsigned char
#define width 3
#define height 3
#define tilesetWidth 73
#define tilesetHeight 16
#define tileSize 48

uniform sampler2D tileset;

layout(binding = 1) readonly buffer InputData {
    byte tiles[];
};

in vec2 pos;

out vec4 out_Color;

void main() {
    float tilemapX = pos.x * width;
    float tilemapY = pos.y * height;

    int gridX = int(floor(tilemapX));
    int gridY = int(floor(tilemapY));

    int tileIndex = gridX + gridY * width;
    int tileValue = int(tiles[tileIndex]);

    int tileX = int(mod(tileValue, tilesetWidth));
    int tileY = (tileValue - tileX) / tilesetWidth;
    float tileBaseUVX = float(tileX) / float(tilesetWidth);
    float tileBaseUVY = float(tileY) / float(tilesetHeight);

    float tileUVX = mod(tilemapX, 1);
    float tileUVY = mod(tilemapY, 1);

    vec2 uv = vec2(
        tileBaseUVX + (tileUVX / float(tilesetWidth)),
        tileBaseUVX + (tileUVY / float(tilesetHeight))
    );

	out_Color = texture(tileset, uv);
}
