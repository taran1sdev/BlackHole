#include "Renderer.h"

Renderer::Renderer(Shader& shader)
    : shader(shader)
{}

void Renderer::render()
{
    shader.use();
    quad.draw();
}
