#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
/*
 * This class handles loading and compiling shader files
 */

class Shader {
    public:
        GLuint ID; 

        Shader(const std::string& vertexPath, const std::string& fragmentPath);
       
        // function to set uniform 
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec2(const std::string& name, float x, float y) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        
        void use() const;
    private:
        std::string loadFile(const std::string& path) const;
        unsigned int compileShader(unsigned int type, const std::string& source) const;
};
