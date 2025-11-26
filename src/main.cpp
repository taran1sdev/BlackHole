#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Shader.h"
#include "Renderer.h"
#include "Camera.h"

Camera camera;
float lastX = 400, lastY = 300;
bool firstMouse = true;

// We register this callback function with GLFW to process the mouse position
void mouseCallback(GLFWwindow* window, double x, double y) {
    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float xOffset = x - lastX;
    float yOffset = lastY - y;

    lastX = x;
    lastY = y;

    camera.getMouse(xOffset, yOffset);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Black Hole", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    // Register callback for mouse 
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Use glad to load OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr <<"Failed to initialize GLAD\n";
        return -1;
    }
    
    Shader shader("../shaders/fullscreen.vert", "../shaders/fullscreen.frag");
    Renderer renderer(shader);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        renderer.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    shader.destroy();
    glfwTerminate();
    return 0;
}
