#pragma once
#include "FullscreenQuad.h"
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>

class Renderer {
    public:
        Renderer(Shader& screenShader, Camera& cam);
        void render();
    
    private:
        Shader& screenShader;
        Camera& camera;

        Shader computeShader;
        FullscreenQuad quad;
        
        GLuint outputTexture = 0;
        int texWidth = 0;
        int texHeight = 0;

        void initTexture(int w, int h);
};
