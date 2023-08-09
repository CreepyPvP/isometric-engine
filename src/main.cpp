#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include <opengl.hpp>
#include <mesh_gen.hpp>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define internal static 
#define global_variable static 

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

// fbo
global_variable unsigned int shadowBuffer;
// depth buffer
global_variable unsigned int shadowMap;
global_variable glm::mat4 lightSpace;

global_variable unsigned int squareVao;

global_variable Mesh chunkMesh;


internal void updateViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

internal void frameBufferResizeCallback(GLFWwindow *window, int width, int height) {
    updateViewport(width, height);
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
    globalWindow.handle = glfwCreateWindow(globalWindow.width, globalWindow.height, "Isometric engine", NULL, NULL);
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

internal void setupShadowMap() {
    glGenFramebuffers(1, &shadowBuffer);
    const unsigned int shadowWidth = 1024, shadowHeight = 1024;
    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
            shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_BUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float nearPlane = 0.1f;
    float farPlane = 17;
    auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    auto lightView = glm::lookAt(glm::vec3(5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    lightSpace = lightProjection * lightView;
}

internal void setupGeometry() {
    byte* data = (byte*) malloc(8 * 8 * 3);

    byte* ptr = data;
    for (int y = 0; y < 3; ++y) {
        for (int z = 0; z < 8; ++z) {
            for (int x = 0; x < 8; ++x) {
                if (y == 0) {
                    *ptr = 0;
                } else if (x == 2) {
                    *ptr = 1;
                } else {
                    *ptr = 255;
                }
                ++ptr;
            }
        }
    }
    chunkMesh = generateVoxelMesh(8, 3, 8, data, 10, 5);
    free(data);
}

int main() {
    initWindow();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("failed to load required extensions");
    }
    glEnable(GL_DEPTH_TEST);
    // culling
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);

    setupGBuffer();
    setupSquareVao();
    setupShadowMap();
    setupGeometry();

    auto modelMatrix = glm::mat4(1);

    auto viewMatrix = glm::lookAt(
        glm::vec3(5, 3, 5), 
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    auto projectionMatrix = glm::perspective(45.0f, (float) globalWindow.width / (float) globalWindow.height, 1.0f, 150.0f);

    ObjectShader objectShader = createObjectShader("../shader/objectVert.glsl", "../shader/objectFrag.glsl");
    LightingShader lightingShader = createLightingShader("../shader/lightingVert.glsl", "../shader/lightingFrag.glsl");
    ShadowShader shadowShader = createShadowShader("../shader/shadowVert.glsl", "../shader/shadowFrag.glsl");

    Texture tileset = loadTexture("../assets/tileset.png");

    auto lightPos = glm::vec3(5);
    auto lightColor = glm::vec3(1, 1, 1);

    while (!glfwWindowShouldClose(globalWindow.handle)) {
        if (glfwGetKey(globalWindow.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(globalWindow.handle, true);
        }

        // Render shadow map
        glCullFace(GL_FRONT);
        updateViewport(1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);
        useShader(shadowShader.id);
        setUniformMat4(shadowShader.uniformLightSpace, &lightSpace);

        setUniformMat4(shadowShader.uniformModel, &modelMatrix);
        glBindVertexArray(chunkMesh.vao);
        glDrawElements(GL_TRIANGLES, chunkMesh.indexCount, GL_UNSIGNED_INT, 0);

        updateViewport(globalWindow.width, globalWindow.height);
        glCullFace(GL_BACK);

        // Setup geometry buffer
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup shader
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glActiveTexture(GL_TEXTURE0);
        bindTexture(tileset);
        useShader(objectShader.id);
        setUniformMat4(objectShader.uniformView, &viewMatrix);
        setUniformMat4(objectShader.uniformProjection, &projectionMatrix);

        setUniformMat4(objectShader.uniformModel, &modelMatrix);
        glBindVertexArray(chunkMesh.vao);
        glDrawElements(GL_TRIANGLES, chunkMesh.indexCount, GL_UNSIGNED_INT, 0);

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
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, shadowMap);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        useShader(lightingShader.id);
        setUniformVec3(lightingShader.uniformLightPos, &lightPos);
        setUniformVec3(lightingShader.uniformLightColor, &lightColor);
        setUniformMat4(lightingShader.uniformLightSpace, &lightSpace);
        glBindVertexArray(squareVao);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(globalWindow.handle);
        glfwPollEvents();
    }

    glfwTerminate();
}
