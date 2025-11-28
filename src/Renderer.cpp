#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Camera.h"
#include "BlackHole.h"
#include "FullscreenQuad.h"
#include "Renderer.h"

Renderer::Renderer(Shader& screenShader, Camera& cam, BlackHole& bh)
    : screenShader(screenShader), camera(cam), blackHole(bh), computeShader("../shaders/blackhole.comp")
{}

void Renderer::initTexture(int w, int h) {
    if (outputTexture != 0 && (w == texWidth && h == texHeight)) return;
    
    if (outputTexture != 0) glDeleteTextures(1, &outputTexture);

    texWidth = w;
    texHeight = h;

    glGenTextures(1, &outputTexture);
    glBindTexture(GL_TEXTURE_2D, outputTexture);
    glTexImage2D(
            GL_TEXTURE_2D, 0,
            GL_RGBA32F, w, h,
            0,
            GL_RGBA, GL_FLOAT,
            nullptr);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::render()
{
    int w, h;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);    
    initTexture(w, h);
    
    computeShader.use();
    computeShader.setVec2("resolution", (float)w, (float)h);

    camera.uploadToShader(computeShader); 
    blackHole.uploadToShader(computeShader);

    glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    const GLuint localSizeX = 8;
    const GLuint localSizeY = 8;
    GLuint groupsX = (w + localSizeX - 1) / localSizeX;
    GLuint groupsY = (h + localSizeY - 1) / localSizeY;

    glDispatchCompute(groupsX, groupsY, 1);
   
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT); 
    
    screenShader.use();
    screenShader.setInt("renderedImage", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, outputTexture);

    quad.draw();
}
