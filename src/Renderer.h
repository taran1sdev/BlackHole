#pragma once
#include "Shader.h"
#include "Camera.h"
#include "BlackHole.h"
#include "FullscreenQuad.h"

class Renderer {
    public:
        Renderer(Shader& screenShader, Camera& cam, BlackHole& blackHole);
        void init();
        void render();
    
    private:
        Shader& screenShader;
        Camera& camera;
        BlackHole& blackHole;

        Shader computeShader;
        Shader diskVolumeShader;
        FullscreenQuad quad;
        
        GLuint outputTexture = 0;
        int texWidth = 0;
        int texHeight = 0;
        
        GLuint diskVolumeTexture = 1;

        int diskNR = 128;
        int diskNTheta = 256;
        int diskNZ = 64;

        float diskRMin = 2.0f;
        float diskRMax = 40.0f;
        float diskZMin = -2.0f;
        float diskZMax = 2.0f;

        void initTexture(int w, int h);
        void initDiskVolumeTexture();
};
