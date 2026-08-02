#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:

    glm::vec3 position = glm::vec3(10.0f, 10.0f, 10.0f);

    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);


    float fov = glm::radians(45.0f);
    float aspect = 16.0f / 9.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, target, up);
    }

    glm::mat4 getProjectionMatrix() const {
        glm::mat4 proj = glm::perspective(fov, aspect, nearPlane, farPlane);

        proj[1][1] *= -1; 
        
        return proj;
    }
};