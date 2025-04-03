#include "Planet.h"
#include <iostream>

Planet::Planet(std::string texture, float orbitRadius, float scale, float orbitSpeed, float rotationSpeed, float tilt, float inclination, glm::vec3 centerOfOrbit)
{
	mPath = AssetPath::Instance();
	mTexture = std::make_unique<Texture>(mPath->Get(texture), GL_NEAREST);
	mOrbitRadius = orbitRadius;
	mScale = scale;
	mOrbitSpeed = orbitSpeed;
	mRotationSpeed = rotationSpeed;
	mTilt = tilt;
	mInclination = inclination;
	mCenterOfOrbit = centerOfOrbit;

	// create the model matrix for the planet
	mModel = glm::mat4(1.0f);
	mModel = glm::translate(mModel, mCenterOfOrbit);
	mModel = glm::rotate(mModel, glm::radians(mInclination), glm::vec3(0.0f, 1.0f, 0.0f));
	mModel = glm::rotate(mModel, glm::radians(mTilt), glm::vec3(1.0f, 0.0f, 0.0f));
	mModel = glm::rotate(mModel, glm::radians(mRotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
	mModel = glm::translate(mModel, glm::vec3(mOrbitRadius, 0.0f, 0.0f));
	mModel = glm::scale(mModel, glm::vec3(mScale, mScale, mScale));
}

void Planet::update(float deltaTime)
{

}

glm::vec3 Planet::getPosition() const
{
	glm::vec3 pos = glm::vec3(mModel[3].x, mModel[3].y, mModel[3].z);
	return pos;
}

// returns the model matrix for the planet
glm::mat4 Planet::getModel() const
{
	return mModel;
}