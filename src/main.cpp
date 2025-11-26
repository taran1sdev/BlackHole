#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Shader.h"
#include "Renderer.h"
#include "Camera.h"

// load file helper
static std::string loadTextFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// Compile compute shader
static GLuint createComputeProgram(const std::string& path) {
    std::string srcStr = loadTextFile(path);
    const char* src = srcStr.c_str();

    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(cs, 1, &src, nullptr);
    glCompileShader(cs);

    GLint success;
    glGetShaderiv(cs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetShaderInfoLog(cs, 1024, nullptr, log);
        std::cerr << "shader failed to compile:\n" << log << "\n";
    }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, cs);
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetProgramInfoLog(prog, 1024, nullptr, log);
        std::cerr << "linking error:\n" << log << "\n";
    }

    glDeleteShader(cs);
    return prog;
}

Camera camera;
float lastX = 400, lastY = 300, deltaTime = 0.0f, lastFrame = 0.0f;
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

void framebuffer(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

GLuint computeProgram = 0;
GLuint rayTexture = 0;
const int RENDER_WIDTH = 1280;
const int RENDER_HEIGHT = 720;

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Black Hole", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    glfwSetFramebufferSizeCallback(window, framebuffer);

    // Register callback for mouse 
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Use glad to load OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr <<"Failed to initialize GLAD\n";
        return -1;
    }
    
    glEnable(GL_TEXTURE_2D);

    computeProgram = createComputeProgram("../shaders/geodesic.comp");

    glGenTextures(1, &rayTexture);
    glBindTexture(GL_TEXTURE_2D, rayTexture);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            RENDER_WIDTH,
            RENDER_HEIGHT,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr
    );


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    Shader shader("../shaders/fullscreen.vert", "../shaders/fullscreen.frag");
    shader.use();
    shader.setInt("screenTexture", 0);

    Renderer renderer(shader, rayTexture);
    renderer.setResolution(1280, 720);

    glm::vec3 bHPos(0.0f, 0.0f, 0.0f);
    float blackHoleMass = 1.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check every frame for pressed key - using a callback doesn't allow keys
        // to be held and results in choppy movements
        camera.getKeys(window, deltaTime);
        
        glUseProgram(computeProgram);

        GLint loc;

        loc = glGetUniformLocation(computeProgram, "uResolution");
        glUniform2i(loc, RENDER_WIDTH, RENDER_HEIGHT);

        loc = glGetUniformLocation(computeProgram, "camPos");
        glUniform3f(loc, camera.position.x, camera.position.y, camera.position.z);

        glm::vec3 f = camera.getForward();
        glm::vec3 r = camera.getRight();
        glm::vec3 u = camera.getUp();


        glUniform3f(glGetUniformLocation(computeProgram, "camForward"), f.x, f.y, f.z);
        glUniform3f(glGetUniformLocation(computeProgram, "camRight"), r.x, r.y, r.z);
        glUniform3f(glGetUniformLocation(computeProgram, "camUp"), u.x, u.y, u.z);
        
        glUniform3f(glGetUniformLocation(computeProgram, "bhPos"), bHPos.x, bHPos.y, bHPos.z);

        glUniform1f(glGetUniformLocation(computeProgram, "bhMass"), blackHoleMass);

        glUniform1f(glGetUniformLocation(computeProgram, "diskInner"), 3.0f);
        glUniform1f(glGetUniformLocation(computeProgram, "diskOuter"), 8.0f);

        glBindImageTexture(
                0,
                rayTexture,
                0,
                GL_FALSE,
                0,
                GL_WRITE_ONLY,
                GL_RGBA8
        );

        const int LOCAL = 16;

        GLuint groupsX = (RENDER_WIDTH + LOCAL - 1) / LOCAL;
        GLuint groupsY = (RENDER_HEIGHT + LOCAL - 1) / LOCAL;
        glDispatchCompute(groupsX, groupsY, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        
        glClear(GL_COLOR_BUFFER_BIT);         
        renderer.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
