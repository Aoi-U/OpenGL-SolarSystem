#include "Texture.h"
#include "AssetPath.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>

class Planet
{
private:
	std::shared_ptr<AssetPath> mPath;
	std::unique_ptr<Texture> mTexture; // texture of the planet
	float mOrbitRadius; // radius of the orbit
	float mScale; // scale of the planet
	float mOrbitSpeed; // speed of the orbit
	float mRotationSpeed; // speed of the rotation
	float mTilt; // tilt of the planet
	float mInclination; // inclination of the planet
	glm::vec3 mCenterOfOrbit; // center of orbit

	glm::mat4 mModel;

public:
	Planet(std::string texture, float orbitRadius, float scale, float orbitSpeed, float rotationSpeed, float tilt, float inclination, glm::vec3 centerOfOrbit);

	void updateCenterOfOrbit(glm::vec3 center) { mCenterOfOrbit = center; }
	
	Texture* getTexture() const { return mTexture.get(); }

	glm::mat4 getModel() const;

	glm::vec3 getPosition() const;

	void update(float deltaTime);
};

