#pragma once

#include "FullscreenQuad.h"
#include "Shader.h"
#include <glad/glad.h>
/*
 * This just wraps the call to draw the fullscreen quad
 */

class Renderer {
    public:
        Renderer(Shader& shader, GLuint outputTexture);
        
        void setResolution(int width, int height);

        void render();
    private:
        Shader& shader;
        FullscreenQuad quad;
        GLuint texture;
        int width, height;
};
