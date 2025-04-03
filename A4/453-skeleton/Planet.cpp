#include "Planet.h"
#include <iostream>

Planet::Planet(std::string texture, float orbitRadius, float scale, float orbitSpeed, float rotationSpeed, float tilt, float inclination, glm::vec3 centerOfOrbit)
	: mOrbitRadius(orbitRadius), mOrbitSpeed(orbitSpeed), mRotationSpeed(rotationSpeed), mTilt(tilt), mInclination(inclination), mCenterOfOrbit(centerOfOrbit)
{
	mPath = AssetPath::Instance();
	mTexture = std::make_unique<Texture>(mPath->Get(texture), GL_NEAREST);
	mScale = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale)); // scale matrix
	mCenterOfOrbit = centerOfOrbit;

	currentRotation = 0.0f;
	currentOrbit = 0.0f;
	currentIncline = 1.0f; // initial incline scalar
	mPosition = glm::vec3(orbitRadius * glm::cos(glm::radians(currentOrbit)), 0.0f, orbitRadius * glm::sin(glm::radians(currentOrbit))); // initial position of the planet
	
	update(0.0f); // update the model matrix
}

void Planet::update(float time)
{
	// update the currnet incline of the planet
	currentOrbit -= mOrbitSpeed * time;
	currentRotation += mRotationSpeed * time;	
	if (currentOrbit > 360.0f)
	{
		currentOrbit -= 360.0f;
	}
	if (currentRotation > 360.0f)
	{
		currentRotation -= 360.0f;
	}

	currentIncline = glm::cos(glm::radians(currentOrbit)); // update the current incline scalar
	mPosition.x = mOrbitRadius * glm::cos(glm::radians(currentOrbit)); // update the position of the planet
	mPosition.z = mOrbitRadius * glm::sin(glm::radians(currentOrbit)); // update the position of the planet

	glm::mat4 matPos = glm::translate(glm::mat4(1.0f), mPosition);
	glm::mat4 matTilt = glm::rotate(glm::mat4(1.0f), glm::radians(mTilt), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 matIncline = glm::rotate(glm::mat4(1.0f), glm::radians(mInclination), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 matNegIncline = glm::rotate(glm::mat4(1.0f), glm::radians(-mInclination), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 matRotation = glm::rotate(glm::mat4(1.0f), glm::radians(currentRotation), glm::vec3(0.0f, 1.0f, 0.0f)); // rotation matrix
	glm::mat4 matCenterPos = glm::translate(glm::mat4(1.0f), mCenterOfOrbit);

	mModel = matTilt * matCenterPos * matIncline * matPos * matNegIncline * matRotation * mScale;
}

glm::vec3 Planet::getPosition() const
{
	return mModel[3]; // return the position of the planet
}

float Planet::getRotation() const
{
	return currentRotation;
}

glm::vec3 Planet::getCenterPosition() const
{
	return mCenterOfOrbit;
}

float Planet::getCurrentIncline() const
{
	return currentIncline; // return the current incline
}

float Planet::getCurrentOrbit() const
{
	return currentOrbit; // return the current orbit
}

float Planet::getCurrentRotation() const
{
	return currentRotation; // return the current rotation
}

// returns the model matrix for the planet
glm::mat4 Planet::getModel() const
{
	return mModel;
}