#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
struct Camera {
    glm::vec3 pos;
    float radius;

    glm::mat4 projection;
    glm::mat4 view;

    void update(float time);
    void setScreenSize(float screenX, float screenY);
};

Camera createCamera(float screenX, float screenY);
