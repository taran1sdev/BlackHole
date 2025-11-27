#pragma once
#include <glm/glm.hpp>

class BlackHole {
    public:
        explicit BlackHole(const glm::vec3& position = glm::vec3(0.0f));

        glm::vec3 getPosition() const;
        float getSchildRadius() const;
        float getGravRadius() const;
        
        void uploadToShader(class Shader& computeShader) const;
    private:
        glm::vec3 position;
};
