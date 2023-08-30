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

global_variable unsigned int shadowMap;
global_variable glm::mat4 lightSpace;
global_variable glm::vec4 lightPos;

// lighting
#define POINT_LIGHT_TEXTURE_SLOT GL_TEXTURE4

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
global_variable Mesh cube;

global_variable World world;


internal void setupGBuffer();
internal void setupPointLight(PointLight* pointLight, Transform* transform);

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

internal void setupCubeVao() {
    unsigned char data = 1;
    cube = generateVoxelMesh(1, 1, 1, &data, 1, 1, FEATURE_POS);
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
    Mesh chunkMesh = generateVoxelMesh(8, 3, 8, data, 10, 5, FEATURE_ALL);

    glm::mat4 transform1 = glm::mat4(1);
    Entity chunk1 = world.spawn();
    world.meshes.insert(chunk1, chunkMesh);
    world.transforms.insert(chunk1, Transform { transform1, glm::vec3(0) });

    // glm::mat4 transform2 = glm::mat4(1);
    // glm::vec3 pos2 = glm::vec3(0, 0, -8);
    // transform2 = glm::translate(transform2, pos2);
    // Entity chunk2 = world.spawn();
    // world.meshes.insert(chunk2, chunkMesh);
    // world.transforms.insert(chunk2, Transform { transform2, pos2 });

    glm::mat4 transform3 = glm::mat4(1);
    glm::vec3 pos3 = glm::vec3(-8, 0, 0);
    transform3 = glm::translate(transform3, pos3);
    Entity chunk3 = world.spawn();
    world.meshes.insert(chunk3, chunkMesh);
    world.transforms.insert(chunk3, Transform { transform3 });

    // glm::mat4 transform4 = glm::mat4(1);
    // glm::vec3 pos4 = glm::vec3(-8, 0, -8);
    // transform4 = glm::translate(transform4, pos4);
    // Entity chunk4 = world.spawn();
    // world.meshes.insert(chunk4, chunkMesh);
    // world.transforms.insert(chunk4, Transform { transform4 });

    free(data);

    Entity pointLight1 = world.spawn();
    PointLight light1;
    light1.shadowMapSize = 1024;
    light1.color = glm::vec3(1, 0.5, 0.5);
    glm::vec3 lightPos1 = glm::vec3(0, 1, 0);
    Transform pointLight1Transform = Transform { 
        glm::scale(glm::translate(glm::mat4(1), lightPos1), glm::vec3(10)), 
        lightPos1 
    };
    setupPointLight(&light1, &pointLight1Transform);
    world.pointLights.insert(pointLight1, light1);
    world.transforms.insert(pointLight1, pointLight1Transform);

    Entity pointLight2 = world.spawn();
    glm::vec3 lightPos2 = glm::vec3(5, 1, 5);
    Transform pointLight2Transform = Transform { 
        glm::scale(glm::translate(glm::mat4(1), lightPos2), glm::vec3(10)), 
        lightPos2 
    };
    PointLight light2;
    light2.shadowMapSize = 1024;
    light2.color = glm::vec3(0.5, 1, 0.5);
    setupPointLight(&light2, &pointLight2Transform);
    world.pointLights.insert(pointLight2, light2);
    world.transforms.insert(pointLight2, pointLight2Transform);

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

internal void setupPointLight(PointLight* pointLight, Transform* transform) {
    glGenTextures(1, &pointLight->shadowMapDepthBuffer);
    glBindTexture(GL_TEXTURE_2D, pointLight->shadowMapDepthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, pointLight->shadowMapSize, pointLight->shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &pointLight->shadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pointLight->shadowMap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    auto projection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 20.0f);
    for (uint32 i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, pointLight->shadowMapSize, pointLight->shadowMapSize, 0, GL_RED, GL_FLOAT, NULL);

        glm::mat4 view = glm::lookAt(glm::vec3(transform->pos), transform->pos + cubemapDirections[i].target, cubemapDirections[i].up);
        pointLight->viewProjectionMat[i] = projection * view;
    }

    glGenFramebuffers(1, &pointLight->framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, pointLight->framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pointLight->shadowMapDepthBuffer, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

internal void bindCubemapFace(PointLight* pointLight, GLenum face) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pointLight->framebuffer);
    updateViewport(pointLight->shadowMapSize, pointLight->shadowMapSize);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face, pointLight->shadowMap, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

internal void beginLightPasses() {
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    // Setup textures for lighting
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    // glActiveTexture(GL_TEXTURE3);
    // glBindTexture(GL_TEXTURE_2D, shadowMap);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
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
    setupCubeVao();
    setupWorld();
    setupCamera();

    ObjectShader objectShader = createObjectShader("../shader/objectVert.glsl", "../shader/objectFrag.glsl");
    PointLightShader pointLightShader = createPointLightShader("../shader/pointLightVert.glsl", "../shader/pointLightFrag.glsl");
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
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        useShader(shadowShader.id);
        // ignore this
        // setUniformMat4(shadowShader.uLightSpace, &lightSpace);

        auto pointLights = world.pointLights.list();
        while (pointLights.next()) {
            Transform* transform = world.transforms.get(pointLights.entity);
            glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
            setUniformVec3(shadowShader.uLightPos, &transform->pos);
            for (uint16 i = 0; i < 6; ++i) {
                bindCubemapFace(pointLights.current, cubemapDirections[i].cubemapFace);
                glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
                setUniformMat4(shadowShader.uLightSpace, pointLights.current->viewProjectionMat + i);
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
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);

        // Setup default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        beginLightPasses();

        useShader(pointLightShader.id);
        glActiveTexture(GL_TEXTURE3);
        setUniformVec3(pointLightShader.uCameraPos, &camera.pos);
        glBindVertexArray(cube.vao);
        glm::vec2 screenSize(globalWindow.width, globalWindow.height);
        setUniformVec2(
            pointLightShader.gScreenSize, 
            &screenSize
        );
        setUniformMat4(pointLightShader.uView, &camera.view);
        setUniformMat4(pointLightShader.uProjection, &camera.projection);
        pointLights = world.pointLights.list();
        while (pointLights.next()) {
            Transform* transform = world.transforms.get(pointLights.entity);

            glBindTexture(GL_TEXTURE_CUBE_MAP, pointLights.current->shadowMap);
            setUniformVec3(pointLightShader.uLightPos, &transform->pos);
            setUniformVec3(pointLightShader.uLightColor, &pointLights.current->color);
            setUniformMat4(pointLightShader.uModel, &transform->mat);

            glDrawElements(GL_TRIANGLES, cube.indexCount, GL_UNSIGNED_INT, 0);
            // TODO: set attenuation here
            // setUniformMat4(lightingShader.uLightSpace, &lightSpace);
        }

        glfwSwapBuffers(globalWindow.handle);
        glfwPollEvents();
    }

    glfwTerminate();
}
