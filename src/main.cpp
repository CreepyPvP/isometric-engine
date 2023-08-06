#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include <opengl.hpp>
#include <chunk.hpp>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define internal static 
#define global_variable static 

typedef unsigned char byte;

struct Window {
    int width;
    int height;
    GLFWwindow* handle;
};

global_variable Window globalWindow;
global_variable unsigned int gBuffer;
global_variable unsigned int gPosition;
global_variable unsigned int gNormal;
global_variable unsigned int gAlbedo;

global_variable unsigned int squareVao;
global_variable Chunk chunk;


internal void frameBufferResizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    globalWindow.width = width;
    globalWindow.height = height;
}

internal void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    globalWindow.width = 1280;
    globalWindow.height = 720;
    globalWindow.handle = glfwCreateWindow(globalWindow.width, globalWindow.height, "My Game", NULL, NULL);
    if (!globalWindow.handle) {
        printf("Failed to create window\n");
    }
    glfwMakeContextCurrent(globalWindow.handle);
    glfwSetFramebufferSizeCallback(globalWindow.handle, frameBufferResizeCallback);
}

internal void setupSquareVao() {
    float vertexData[] = {
        -1,  1,
         1,  1,
        -1, -1,

         1,  1,
         1, -1,
        -1, -1
    };

    glGenVertexArrays(1, &squareVao);
    glBindVertexArray(squareVao);
    unsigned int vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 6, vertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(int) * 2, 0);
}

internal void setupGBuffer() {
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // position
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, globalWindow.width, globalWindow.height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // normal
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, globalWindow.width, globalWindow.height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // albedo
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, globalWindow.width, globalWindow.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

    unsigned int buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, buffers);

    unsigned int depth;
    glGenRenderbuffers(1, &depth);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, globalWindow.width, globalWindow.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
}

void setupChunk() {
    chunk = createChunk(5);

    byte ground[64];
    for (int i = 0; i < 64; ++i) {
        ground[i] = 0;
    }
    chunk.createTilemap(8, 8, ground, glm::vec3(0, 0, 0), glm::vec3(90.0f, 0, 0));

    byte wall[] = { 1, 1, 1 };
    chunk.createTilemap(3, 1, wall, glm::vec3(100, -100, -100), glm::vec3(180, 0, 0));
    chunk.createTilemap(3, 1, wall, glm::vec3(100, -100, 100), glm::vec3(180, 90, 0));
    chunk.createTilemap(3, 1, wall, glm::vec3(100, -100, -400), glm::vec3(180, 0, 0));
    chunk.createTilemap(3, 1, wall, glm::vec3(100, -100, 400), glm::vec3(180, 90, 0));

    chunk.build();
}

int main() {
    initWindow();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("failed to load required extensions");
    }
    glEnable(GL_DEPTH_TEST);
    // culling
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    setupGBuffer();
    setupSquareVao();
    setupChunk();

    auto viewMatrix = glm::lookAt(
        glm::vec3(1000.0f), 
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    auto projectionMatrix = glm::ortho(
        (float) -globalWindow.width / 2, 
        (float) globalWindow.width / 2,
        (float) -globalWindow.height / 2, 
        (float) globalWindow.height / 2, .1f,
        100000.0f
    );

    TilemapShader tilemapShader = createTilemapShader("../shader/tilemapVert.glsl", "../shader/tilemapFrag.glsl");
    LightingShader lightingShader = createLightingShader("../shader/lightingVert.glsl", "../shader/lightingFrag.glsl");

    Texture tileset = loadTexture("../assets/tileset.png");

    while (!glfwWindowShouldClose(globalWindow.handle)) {
        if (glfwGetKey(globalWindow.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(globalWindow.handle, true);
        }

        // Setup geometry buffer
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup tilemap shader
        glActiveTexture(GL_TEXTURE0);
        bindTexture(tileset);
        useShader(tilemapShader.id);
        setUniformMat4(tilemapShader.uniformView, &viewMatrix);
        setUniformMat4(tilemapShader.uniformProjection, &projectionMatrix);

        // call chunk.render() here
        chunk.render(&tilemapShader);

        // Setup default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup textures for lighting shader
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);

        glBindVertexArray(squareVao);
        useShader(lightingShader.id);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(globalWindow.handle);
        glfwPollEvents();
    }

    glfwTerminate();
}
