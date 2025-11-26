#include "Renderer.h"
#include <GLFW/glfw3.h>

Renderer::Renderer(Shader& shader)
    : shader(shader)
{}

void Renderer::render()
{
    shader.use();
    
    shader.setVec2("resolution", 800.0f, 600.0f);

    quad.draw();
}
