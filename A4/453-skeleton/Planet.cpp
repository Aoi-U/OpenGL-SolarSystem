#include "Planet.h"
#include <iostream>

Planet::Planet(std::string texture, float orbitRadius, float scale, float orbitSpeed, float rotationSpeed, float tilt, glm::vec3 centerOfOrbit)
{
	mPath = AssetPath::Instance();
	mTexture = std::make_unique<Texture>(mPath->Get(texture), GL_NEAREST);
	mOrbitRadius = orbitRadius;
	mScale = scale;
	mOrbitSpeed = orbitSpeed;
	mRotationSpeed = rotationSpeed;
	mTilt = tilt;
	mCenterOfOrbit = centerOfOrbit;

	// create the model matrix for the planet
	//glm::vec3 position = glm::vec3(orbitRadius + centerOfOrbit.x, centerOfOrbit.y, centerOfOrbit.z);
	glm::vec3 position = centerOfOrbit + glm::vec3(orbitRadius, 0.0f, 0.0f);
	mModel = glm::identity<glm::mat4>();
	mModel = glm::translate(mModel, position);
	mModel = glm::rotate(mModel, glm::radians(mTilt), glm::vec3(0.0f, 0.0f, 1.0f));
	mModel = glm::scale(mModel, glm::vec3(mScale));
}

void Planet::update(float deltaTime)
{
	// update the orbit of the planet around the center of orbit
	glm::vec3 currPos = getPosition();
	float newX = mCenterOfOrbit.x + (currPos.x - mCenterOfOrbit.x) * glm::cos(mOrbitSpeed * deltaTime) - (currPos.z - mCenterOfOrbit.z) * sin(mOrbitSpeed * deltaTime);
	float newZ = mCenterOfOrbit.z + (currPos.x - mCenterOfOrbit.x) * glm::sin(mOrbitSpeed * deltaTime) + (currPos.z - mCenterOfOrbit.z) * cos(mOrbitSpeed * deltaTime);
	
	mModel[3].x = newX;
	mModel[3].z = newZ;

	
	// update the rotation of the planet
	mModel = glm::rotate(mModel, glm::radians(mRotationSpeed * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
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