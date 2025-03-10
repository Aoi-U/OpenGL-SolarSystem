#include "Camera.h"
#include <iostream>

Camera::Camera(Window& window) 
    : window(window), model(glm::mat4(1.0f)), view(glm::mat4(1.0f)), proj(glm::mat4(1.0f))
{
    int width = window.getWidth();
    int height = window.getHeight();
    // set initial position of the camera
    radius = 2.0f;   
    theta = 45.0f;
    phi = 45.0f;

    float rTheta = glm::radians(theta);
    float rPhi = glm::radians(phi);
    float x = radius * glm::cos(rTheta) * glm::cos(rPhi);
    float y = radius * glm::sin(rPhi);
    float z = radius * glm::sin(rTheta) * glm::cos(rPhi);
    view = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    proj = glm::perspective(glm::radians(45.0f), (float)(width/height), 0.1f, 100.0f);  
}

void Camera::Move(float dTheta, float dPhi) { 
    theta += dTheta;
    phi += dPhi;

    // clamps phi between -90 and 90 to prevent flipping
    if (phi < -90.0) {
        phi = -90.0f;
    } else if (phi > 90.0f) {
        phi = 90.0f;
    }

    float rTheta = glm::radians(theta);
    float rPhi = glm::radians(phi);

    // calculate the new position of the camera
    float x = radius * glm::cos(rTheta) * glm::cos(rPhi);
    float y = radius * glm::sin(rPhi);
    float z = radius * glm::sin(rTheta) * glm::cos(rPhi);
    view = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::Zoom(float zoom) {
    radius += zoom;
    if (radius < 10e-6) {
        radius = 10e-6f;
    } else if (radius > 100.0f) {
        radius = 100.0f;
    } 

    float rTheta = glm::radians(theta);
    float rPhi = glm::radians(phi);
    // calculate the new position of the camera
    float x = radius * glm::cos(rTheta) * glm::cos(rPhi);
    float y = radius * glm::sin(rPhi);
    float z = radius * glm::sin(rTheta) * glm::cos(rPhi);
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
