#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
    position = glm::vec3(0.0f, 0.0f, 3.0f);
    yaw = -99.0f;
    pitch = 0.0f;
}

glm::vec3 Camera::getForward() const {
    float cosYaw = cos(glm::radians(yaw));
    float sinYaw = sin(glm::radians(yaw));
    float cosPitch = cos(glm::radians(pitch));
    float sinPitch = sin(glm::radians(pitch));

    return glm::normalize(glm::vec3(cosYaw * cosPitch, sinPitch, sinYaw * cosPitch)); 
}

glm::vec3 Camera::getRight() const {
    return glm::normalize(glm::cross(getForward(), glm::vec3(0.0, 1.0, 0.0)));
}

glm::vec3 Camera::getUp() const {
    return glm::normalize(glm::cross(getRight(), getForward()));
}

void Camera::getMouse(float x, float y) {
    const float sensitivity = 0.1f;
    x *= sensitivity;
    y *= sensitivity;

    yaw += x;
    pitch += y;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

void Camera::getKeys(GLFWwindow* window, float deltaTime)
{
    float speed = 2.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += getForward() * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= getForward() * speed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += getRight() * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= getRight() * speed;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position.y += speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        position.y -= speed;
}
