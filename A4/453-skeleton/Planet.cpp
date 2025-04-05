#include "Planet.h"
#include <iostream>

Planet::Planet(std::string const& texture, float orbitRadius, float scale, float orbitSpeed, float rotationSpeed, float tilt, float inclination, glm::vec3 centerOfOrbit)
	: mOrbitRadius(orbitRadius), mOrbitSpeed(orbitSpeed), mRotationSpeed(rotationSpeed), mTilt(tilt), mInclination(inclination), mCenterOfOrbit(centerOfOrbit)
{
	mPath = AssetPath::Instance();
	mTexture = std::make_unique<Texture>(mPath->Get(texture), GL_NEAREST);
	mScale = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale)); // scale matrix
	mCenterOfOrbit = centerOfOrbit;

	currentRotation = 0.0f;
	currentOrbit = 0.0f;
	mPosition = glm::vec3(orbitRadius * glm::cos(glm::radians(currentOrbit)), 0.0f, orbitRadius * glm::sin(glm::radians(currentOrbit))); // initial position of the planet
	
	update(0.0f); // update the model matrix
}

void Planet::update(float time)
{
	// update the current orbit and rotation of the planet
	currentOrbit += mOrbitSpeed * time;
	currentRotation += mRotationSpeed * time;	
	if (currentOrbit > 360.0f)
	{
		currentOrbit -= 360.0f;
		orbitCount++;
	}
	if (currentRotation > 360.0f)
	{
		currentRotation -= 360.0f;
		rotationCount++;
	}

	mPosition.x = mOrbitRadius * glm::cos(glm::radians(-currentOrbit)); // update the position of the planet
	mPosition.z = mOrbitRadius * glm::sin(glm::radians(-currentOrbit)); // update the position of the planet

	glm::mat4 matPos = glm::translate(glm::mat4(1.0f), mPosition);
	glm::mat4 matTilt = glm::rotate(glm::mat4(1.0f), glm::radians(mTilt), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 matIncline = glm::rotate(glm::mat4(1.0f), glm::radians(mInclination), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 matNegIncline = glm::rotate(glm::mat4(1.0f), glm::radians(-mInclination), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 matRotation = glm::rotate(glm::mat4(1.0f), glm::radians(currentRotation), glm::vec3(0.0f, 1.0f, 0.0f)); // rotation matrix
	glm::mat4 matCenterPos = glm::translate(glm::mat4(1.0f), mCenterOfOrbit); // translation matrix to the center of the orbit

	mModel = matCenterPos * matIncline * matPos * matNegIncline * matTilt * matRotation * mScale;
}

void Planet::Reset()
{
	currentRotation = defaultRotation;
	currentOrbit = defaultOrbit;
}