#include "Camera.h"
#include <iostream>

Camera::Camera(Window& window) 
    : window(window), model(glm::mat4(1.0f)), view(glm::mat4(1.0f)), proj(glm::mat4(1.0f))
{
    int width = window.getWidth();
    int height = window.getHeight();
    radius = 2.0f;   
    theta = 45.0f;
    phi = 0.0f;

    view = glm::rotate(view, glm::radians(theta), glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::rotate(view, glm::radians(phi), glm::vec3(0.0f, 1.0f, 0.0f));
    float x = radius * glm::sin(glm::radians(phi)) * glm::cos(glm::radians(theta));
    float y = radius * glm::sin(glm::radians(phi)) * glm::sin(glm::radians(theta));
    float z = radius * glm::cos(glm::radians(phi));
    view = glm::translate(view, glm::vec3(x, y, z));
    view = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    proj = glm::perspective(glm::radians(45.0f), (float)(width/height), 0.1f, 100.0f);  
}

void Camera::Move(float x, float z) {   
    theta += x * 0.1f;
    phi += z * 0.1f;
    
    // translate view by its inverse
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
    // rotate view
    view = glm::rotate(view, glm::radians(theta), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(phi), glm::vec3(1.0f, 0.0f, 0.0f));
    // translate back
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
    
}

void Camera::Zoom(float zoom) {
    radius += zoom;
    if (radius < 10e-6) {
        radius = 10e-6f;
    } else if (radius > 100.0f) {
        radius = 100.0f;
    } 

    float x = radius * glm::sin(glm::radians(phi)) * glm::cos(glm::radians(theta));
    float y = radius * glm::sin(glm::radians(phi)) * glm::sin(glm::radians(theta));
    float z = radius * glm::cos(glm::radians(phi));
    view = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    
    
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
