#include "Camera.h"
#include <iostream>

Camera::Camera(int width, int height)
	: model(glm::mat4(1.0f)), view(glm::mat4(1.0f)), proj(glm::mat4(1.0f))
{
	this->width = width;
	this->height = height;
	// set initial position of the camera
	radius = defaultRadius;
	theta = defaultTheta;
	phi = defaultPhi;

	float rTheta = glm::radians(theta);
	float rPhi = glm::radians(phi);
	float x = radius * glm::cos(rTheta) * glm::cos(rPhi);
	float y = radius * glm::sin(rPhi);
	float z = radius * glm::sin(rTheta) * glm::cos(rPhi);
	view = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 100.0f);
}

void Camera::Move(const float& dTheta, const float& dPhi) {
	// update theta and phi by the new delta values
	theta += dTheta * sensitivity;
	phi += dPhi * sensitivity;

	// clamps phi between -89.9 and 89.9 to prevent flipping
	if (phi < -89.9) {
		phi = -89.9f;
	}
	else if (phi > 89.9f) {
		phi = 89.9f;
	}

	UpdateView(); // update the view matrix
}

void Camera::Zoom(const float& zoom) {
	fov += zoom; // update the fov by the new delta value

	// clamps the fov between 1 and 179 to prevent flipping
	if (fov < 1) {
		fov = 1;
	}
	else if (fov > 179) {
		fov = 179;
	}

	// update the projection matrix
	proj = glm::perspective(glm::radians(fov), (float)(width / height), 0.1f, 100.0f);
}

void Camera::Reset() {
	// reset the camera to its initial position
	radius = defaultRadius;
	theta = defaultTheta;
	phi = defaultPhi;
	UpdateView();
}

void Camera::TogglePerspectiveMode() {
	// toggle between perspective and orthographic mode
	isPerspective = !isPerspective;
	if (isPerspective) {
		proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 100.0f);
	}
	else {
		proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	}
}

glm::mat4 Camera::getModel() { return model; }

glm::mat4 Camera::getProj() { return proj; }

glm::mat4 Camera::getView() { return view; }

void Camera::UpdateView() {
	float rTheta = glm::radians(theta);
	float rPhi = glm::radians(phi);

	// calculate the x y z position of the camera based on the radius, theta, and phi
	float x = radius * glm::cos(rTheta) * glm::cos(rPhi);
	float y = radius * glm::sin(rPhi);
	float z = radius * glm::sin(rTheta) * glm::cos(rPhi);

	// update the view matrix
	view = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}
