#include "Texture.h"
#include "AssetPath.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>

class Planet
{
private:
	std::shared_ptr<AssetPath> mPath;
	std::unique_ptr<Texture> mTexture; // texture of the planet
	const float mOrbitRadius; // radius of the orbit
	const float mOrbitSpeed; // speed of the orbit
	const float mRotationSpeed; // speed of the rotation
	const float mTilt; // tilt of the planet
	const float mInclination; // inclination of the planet

	glm::mat4 mScale; // scale matrix
	glm::vec3 mCenterOfOrbit; // center of orbit
	glm::vec3 mPosition; // position of the planet
	float currentRotation; // current rotation of the planet 
	float currentOrbit; // current orbit of the planet 
	float currentIncline; // current incline factor
	
	glm::mat4 mModel;

public:
	Planet(std::string texture, float orbitRadius, float scale, float orbitSpeed, float rotationSpeed, float tilt, float inclination, glm::vec3 centerOfOrbit);

	void updateCenterOfOrbit(glm::vec3 center) { mCenterOfOrbit = center; }
	
	Texture* getTexture() const { return mTexture.get(); }

	glm::mat4 getModel() const;

	glm::vec3 getPosition() const;
	// get pointer to position
	glm::vec3* getPositionPtr() { return &mPosition; }

	float getRotation() const;
	float getCurrentIncline() const;
	float getCurrentOrbit() const;
	float getCurrentRotation() const;
	glm::vec3 getCenterPosition() const;

	void update(float deltaTime);
};

