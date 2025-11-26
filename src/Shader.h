#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
/*
 * This class handles loading and compiling shader files
 */

class Shader {
    public:
        unsigned int ID;

        Shader(const std::string& vertexPath, const std::string& fragmentPath);
       
        // function to set uniform 
        void setVec2(const std::string& name, float x, float y) const;
        
        void setVec3(const std::string& name, const glm::vec3& value) const;

        void use() const;

        void destroy();

    private:
        std::string loadFile(const std::string& path) const;
        unsigned int compileShader(unsigned int type, const std::string& source) const;
};
