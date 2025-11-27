#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
/*
 * This class handles loading and compiling shader files
 */

class Shader {
    public:
        // constructor for vertex / fragment shaders
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
               
        // constructor for compute shader
        explicit Shader(const std::string& computePath);

        // functions to set uniform 
        void setVec2(const std::string& name, float x, float y) const;
        
        void setVec3(const std::string& name, const glm::vec3& value) const;

        void use() const;

        void destroy();

    private:
        unsigned int ID;        
        std::string loadFile(const std::string& path) const;
        unsigned int compileShader(unsigned int type, const std::string& source) const;
};
