#pragma once
#include <glm/glm.hpp>

class Camera {
    public:
        glm::vec3 position;
        float yaw;
        float pitch;

        Camera();

        glm::vec3 getForward() const;
        glm::vec3 getRight() const;
        glm::vec3 getUp() const;

        void getMouse(float x, float y);
};
