#pragma once
#include "FullscreenQuad.h"
#include "Shader.h"

/*
 * This just wraps the call to draw the fullscreen quad
 */

class Renderer {
    public:
        Renderer(Shader& shader);
        void render();
    private:
        Shader& shader;
        FullscreenQuad quad;
};
