#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"
#include <GLFW/glfw3.h>

Renderer::Renderer(Shader& screenShader, Camera& cam)
    : screenShader(screenShader), camera(cam), computeShader("../shaders/blackhole.comp")
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
    
    glm::vec3 f = camera.getForward();
    glm::vec3 r = camera.getRight();
    glm::vec3 up = camera.getUp();
    glm::vec3 pos = camera.position;
    
    computeShader.use();
    computeShader.setVec2("resolution", (float)w, (float)h);
    computeShader.setVec3("camForward", f);
    computeShader.setVec3("camRight", r);
    computeShader.setVec3("camUp", up);
    computeShader.setVec3("camPos", pos);
    
    glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    const GLuint localSizeX = 8;
    const GLuint localSizeY = 8;
    GLuint groupsX = (w + localSizeX - 1) / localSizeX;
    GLuint groupsY = (h + localSizeY - 1) / localSizeY;

    glDispatchCompute(groupsX, groupsY, 1);
   
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT); 
    
    screenShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, outputTexture);

    quad.draw();
}
