#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include <opengl.hpp>


#define internal static 
#define global_variable static 

typedef unsigned char byte;

struct Window {
    int width;
    int height;
    GLFWwindow* handle;
};

global_variable Window globalWindow;
global_variable unsigned int tilemapVao;

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

internal void createTilemapBuffers() {
    float vertices[] = {
        0, 0, 0,
        0, 1, 0,
        1, 0, 0,
        1, 1, 0
    };

    unsigned int indices[] = {
        0, 1, 2, 
        2, 1, 3
    };

    int width = 3;
    int height = 3;

    byte tileData[9] = {
        0, 1, 0,
        1, 0, 1,
        0, 1, 0
    };

    int indexCount = 6;
    int vertexCount = 4;

    glGenVertexArrays(1, &tilemapVao);
    glBindVertexArray(tilemapVao);

    unsigned int buffers[3];
    glGenBuffers(3, buffers);
    unsigned int vertexBuffer = buffers[0];
    unsigned int indexBuffer = buffers[1];
    unsigned int tileDataBuffer = buffers[2];

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount * 3, &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexCount, &indices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tileDataBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(byte) * width * height, &tileData[0], GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(int) * 3, 0);

    // tile data
    glEnableVertexAttribArray(1);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, tileDataBuffer);

    glBindVertexArray(0);
}

int main() {
    initWindow();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("failed to load required extensions");
    }
    glCullFace(GL_NONE);

    Shader shader = createShader("../shader/vert.glsl", "../shader/frag.glsl");

    Texture tileset = loadTexture("../assets/tileset.png");
    createTilemapBuffers();

    while (!glfwWindowShouldClose(globalWindow.handle)) {
        if (glfwGetKey(globalWindow.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(globalWindow.handle, true);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        useShader(&shader);
        bindTexture(tileset);

        glBindVertexArray(tilemapVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(globalWindow.handle);
        glfwPollEvents();
    }

    glfwTerminate();
}
