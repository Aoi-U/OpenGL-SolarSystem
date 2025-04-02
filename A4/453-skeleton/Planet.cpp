#include "Planet.h"

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
	mPosition = mCenterOfOrbit + glm::vec3(mOrbitRadius, 0.0f, 0.0f);

	// create the model matrix for the planet
	mModel = glm::identity<glm::mat4>();
	mModel = glm::translate(mModel, mPosition);
	mModel = glm::rotate(mModel, glm::radians(mTilt), glm::vec3(0.0f, 0.0f, 1.0f));
	mModel = glm::scale(mModel, glm::vec3(mScale));
}

void Planet::update(float deltaTime)
{

}

// returns the model matrix for the planet
glm::mat4 Planet::getModel() const
{
	return mModel;
}