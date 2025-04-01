#include "Planet.h"

Planet::Planet(std::string texture, glm::vec3 position, float scale, float orbitSpeed, float rotationSpeed, float tilt, glm::vec3 center)
{
	mPath = AssetPath::Instance();
	mTexture = std::make_unique<Texture>(mPath->Get(texture), GL_NEAREST);
	mPosition = position;
	mScale = scale;
	mOrbitSpeed = orbitSpeed;
	mRotationSpeed = rotationSpeed;
	mTilt = tilt;
	mCenter = center;
}

void Planet::update(float deltaTime)
{

}

// returns the model matrix for the planet
glm::mat4 Planet::getModel() const
{
	glm::mat4 model = glm::identity<glm::mat4>();
	model = glm::translate(model, mPosition);
	model = glm::rotate(model, glm::radians(mTilt), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(mScale));
	return model;
}