#pragma once
#include "Shader.h"
#include "Camera.h"
#include "BlackHole.h"
#include "FullscreenQuad.h"

class Renderer {
    public:
        Renderer(Shader& screenShader, Camera& cam, BlackHole& blackHole);
        void render();
    
    private:
        Shader& screenShader;
        Camera& camera;
        BlackHole& blackHole;

        Shader computeShader;
        FullscreenQuad quad;
        
        GLuint outputTexture = 0;
        int texWidth = 0;
        int texHeight = 0;

        void initTexture(int w, int h);
};
