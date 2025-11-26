#include "Renderer.h"
#include <glad/glad.h>

Renderer::Renderer(Shader& shader, GLuint outputTexture)
    : shader(shader), texture(outputTexture), width(1280), height(720)
{
}


void Renderer::setResolution(int w, int h) {
    width = w;
    height = h;
}

void Renderer::render()
{
    shader.use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    shader.setInt("screenTexture", 0);

    quad.draw();
}
