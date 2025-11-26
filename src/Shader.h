#pragma once
#include <string>
#include <glad/glad.h>

/*
 * This class handles loading and compiling shader files
 */

class Shader {
    public:
        unsigned int ID;

        Shader(const std::string& vertexPath, const std::string& fragmentPath);
       
        // function to set uniform 
        void setVec2(const std::string& name, float x, float y) const;

        void use() const;

        void destroy();

    private:
        std::string loadFile(const std::string& path) const;
        unsigned int compileShader(unsigned int type, const std::string& source) const;
};
