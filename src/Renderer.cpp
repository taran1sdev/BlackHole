#include "Renderer.h"
#include <GLFW/glfw3.h>

extern Camera camera;

Renderer::Renderer(Shader& shader)
    : shader(shader)
{}

void Renderer::render()
{
    shader.use();
    int w, h;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);    
    shader.setVec2("resolution", (float)w, float(h));
    
    glm::vec3 forwrd = camera.getForward();
    glm::vec3 right = camera.getRight();
    glm::vec3 up = camera.getUp();
    glm::vec3 pos = camera.position;

    shader.setVec3("camForward", forwrd);
    shader.setVec3("camRight", right);
    shader.setVec3("camUp", up);
    shader.setVec3("camPos", pos);
    
    quad.draw();
}
