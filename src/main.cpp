#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <float.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <opengl.hpp>
#include <mesh_gen.hpp>
#include <world.hpp>
#include <camera.hpp>



#define internal static 
#define global_variable static 

struct Window {
    int width;
    int height;
    GLFWwindow* handle;
};

global_variable Window globalWindow;
global_variable Camera camera;
global_variable unsigned int gBuffer;
global_variable unsigned int gPosition;
global_variable unsigned int gNormal;
global_variable unsigned int gAlbedo;
global_variable unsigned int gDepth;

// input
float lastMousePosX, lastMousePosY;

// fbo
global_variable unsigned int shadowBuffer;

// TODO: clean the lighting shit up
// depth buffer
global_variable unsigned int shadowMap;
global_variable unsigned int shadowCubeDepth;
global_variable glm::mat4 lightSpace;
global_variable glm::vec4 lightPos;
global_variable glm::mat4 cubemapLightSpace[6];

struct CubemapDirection {
    GLenum cubemapFace;
    glm::vec3 target;
    glm::vec3 up;
};

CubemapDirection cubemapDirections[6] = {
    { GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1, 0, 0), glm::vec3(0, 1, 0) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0) },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0, 1, 0), glm::vec3(0, 0, -1) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0, -1, 0), glm::vec3(0, 0, 1) },
    // Maybe z axis target needs to be flipped
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0, 0, 1), glm::vec3(0, 1, 0) },
};

global_variable unsigned int squareVao;

global_variable World world;


internal void setupGBuffer();

internal void updateViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

internal void frameBufferResizeCallback(GLFWwindow *window, int width, int height) {
    updateViewport(width, height);
    globalWindow.width = width;
    globalWindow.height = height;
    
    setupGBuffer();
    camera.setScreenSize((float) width, (float) height);
}

internal void mouseCallback(GLFWwindow* window, double posX, double posY) {
    float xOffset = posX - lastMousePosX;
    float yOffset = posY - lastMousePosY;
    lastMousePosX = posX;
    lastMousePosY = posY;
    const float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;
    camera.processMouseInput(xOffset, yOffset);
}

internal void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    globalWindow.width = 1280;
    globalWindow.height = 720;
    globalWindow.handle = glfwCreateWindow(globalWindow.width, globalWindow.height, "Isometric engine", NULL, NULL);
    if (!globalWindow.handle) {
        printf("Failed to create window\n");
    }
    glfwMakeContextCurrent(globalWindow.handle);
    glfwSetFramebufferSizeCallback(globalWindow.handle, frameBufferResizeCallback);

    lastMousePosX = (float) globalWindow.width / 2;
    lastMousePosY = (float) globalWindow.height / 2;
    glfwSetInputMode(globalWindow.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(globalWindow.handle, mouseCallback);
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
    if (gBuffer) {
        glDeleteFramebuffers(1, &gBuffer);
        glDeleteTextures(1, &gPosition);
        glDeleteTextures(1, &gNormal);
        glDeleteTextures(1, &gAlbedo);
        glDeleteRenderbuffers(1, &gDepth);
    }

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

    unsigned int buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, buffers);

    glGenRenderbuffers(1, &gDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, globalWindow.width, globalWindow.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth);
}

internal void setupWorld() {
    world = createWorld(1000);

    uchar* data = (uchar*) malloc(8 * 8 * 3);

    uchar* ptr = data;
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
    Mesh chunkMesh = generateVoxelMesh(8, 3, 8, data, 10, 5);

    glm::mat4 transform1 = glm::mat4(1);
    Entity chunk1 = world.spawn();
    world.meshes.insert(chunk1, chunkMesh);
    world.transforms.insert(chunk1, Transform { transform1 });

    glm::mat4 transform2 = glm::mat4(1);
    transform2 = glm::translate(transform2, glm::vec3(0, 0, -8));
    Entity chunk2 = world.spawn();
    world.meshes.insert(chunk2, chunkMesh);
    world.transforms.insert(chunk2, Transform { transform2 });

    glm::mat4 transform3 = glm::mat4(1);
    transform3 = glm::translate(transform3, glm::vec3(-8, 0, 0));
    Entity chunk3 = world.spawn();
    world.meshes.insert(chunk3, chunkMesh);
    world.transforms.insert(chunk3, Transform { transform3 });

    glm::mat4 transform4 = glm::mat4(1);
    transform4 = glm::translate(transform4, glm::vec3(-8, 0, -8));
    Entity chunk4 = world.spawn();
    world.meshes.insert(chunk4, chunkMesh);
    world.transforms.insert(chunk4, Transform { transform4 });

    free(data);
}

internal void setupCamera() {
    camera = createCamera(globalWindow.width, globalWindow.height);
}

internal void setupDirectionalShadowMap() {
    lightPos = glm::vec4(100, 100, 100, 0);

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
    float farPlane = 200;
    auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    // auto lightProjection = glm::perspective(360.0f, 1.0f, 1.0f, 10.0f);
    auto lightView = glm::lookAt(glm::vec3(lightPos), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    lightSpace = lightProjection * lightView;
}

internal void setupPointShadowMap() {
    lightPos = glm::vec4(5, 2, 0, 1);

    const unsigned int size = 1024;
    glGenTextures(1, &shadowCubeDepth);
    glBindTexture(GL_TEXTURE_2D, shadowCubeDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    auto projection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 20.0f);
    for (uint32 i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, size, size, 0, GL_RED, GL_FLOAT, NULL);

        auto view = glm::lookAt(glm::vec3(lightPos), glm::vec3(lightPos) + cubemapDirections[i].target, cubemapDirections[i].up);
        cubemapLightSpace[i] = projection * view;
    }

    glGenFramebuffers(1, &shadowBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowCubeDepth, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

internal void bindCubemapFace(GLenum face) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowBuffer);
    updateViewport(1024, 1024);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face, shadowMap, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
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
    setupWorld();
    setupCamera();
    setupPointShadowMap();

    ObjectShader objectShader = createObjectShader("../shader/objectVert.glsl", "../shader/objectFrag.glsl");
    LightingShader lightingShader = createLightingShader("../shader/lightingVert.glsl", "../shader/lightingFrag.glsl");
    ShadowShader shadowShader = createShadowShader("../shader/shadowVert.glsl", "../shader/shadowFrag.glsl");

    Texture tileset = loadTexture("../assets/tileset.png");

    auto lightColor = glm::vec3(1, 1, 1);

    float delta = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(globalWindow.handle)) {
        if (glfwGetKey(globalWindow.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(globalWindow.handle, true);
        }

        float currentFrame = glfwGetTime();
        delta = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        camera.processKeyInput(globalWindow.handle, delta);

        // Render shadow map
        // glClear(GL_DEPTH_BUFFER_BIT);
        useShader(shadowShader.id);
        // ignore this
        // setUniformMat4(shadowShader.uLightSpace, &lightSpace);

        glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
        setUniformVec4(shadowShader.uLightPos, &lightPos);
        for (uint16 i = 0; i < 6; ++i) {
            bindCubemapFace(cubemapDirections[i].cubemapFace);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            setUniformMat4(shadowShader.uLightSpace, cubemapLightSpace + i);
            {
                auto shadowCasters = world.meshes.list();
                while (shadowCasters.next()) {
                    Transform* transform = world.transforms.get(shadowCasters.entity);
                    setUniformMat4(shadowShader.uModel, &transform->mat);
                    glBindVertexArray(shadowCasters.current->vao);
                    glDrawElements(GL_TRIANGLES, shadowCasters.current->indexCount, GL_UNSIGNED_INT, 0);
                }
            }

        }
        updateViewport(globalWindow.width, globalWindow.height);

        // Setup geometry buffer
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup shader
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glActiveTexture(GL_TEXTURE0);
        bindTexture(tileset);
        useShader(objectShader.id);
        setUniformMat4(objectShader.uView, &camera.view);
        setUniformMat4(objectShader.uProjection, &camera.projection);
        {
            auto models = world.meshes.list();
            while (models.next()) {
                Transform* transform = world.transforms.get(models.entity);
                setUniformMat4(objectShader.uModel, &transform->mat);
                glBindVertexArray(models.current->vao);
                glDrawElements(GL_TRIANGLES, models.current->indexCount, GL_UNSIGNED_INT, 0);
            }
        }

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
        // glActiveTexture(GL_TEXTURE3);
        // glBindTexture(GL_TEXTURE_2D, shadowMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        useShader(lightingShader.id);
        setUniformVec4(lightingShader.uLightPos, &lightPos);
        setUniformVec3(lightingShader.uLightColor, &lightColor);
        setUniformMat4(lightingShader.uLightSpace, &lightSpace);
        setUniformVec3(lightingShader.uCameraPos, &camera.pos);
        glBindVertexArray(squareVao);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(globalWindow.handle);
        glfwPollEvents();
    }

    glfwTerminate();
}
