#include "Camera.h"

Camera::Camera(Window& window) 
    : window(window), model(glm::mat4(1.0f)), view(glm::mat4(1.0f)), proj(glm::mat4(1.0f))
{
    int width = window.getWidth();
    int height = window.getHeight();
    proj = glm::perspective(glm::radians(45.0f), (float)(width/height), 0.1f, 100.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));
}

void Camera::Move(float theta, float phi) {
    view = glm::rotate(view, glm::radians(theta), glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::rotate(view, glm::radians(phi), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::Zoom(float zoom) {

}

glm::mat4 Camera::getModel() 
{
    return model;
}

glm::mat4 Camera::getProj()
{
    return proj;
}

glm::mat4 Camera::getView()
{
    return view;
}
