#include "Texture.h"
#include "AssetPath.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>

class Planet
{
private:
	static constexpr float defaultRotation = 0.0f;
	static constexpr float defaultOrbit = 0.0f;

	std::shared_ptr<AssetPath> mPath;
	std::unique_ptr<Texture> mTexture; // texture of the planet
	const float mOrbitRadius; // radius of the orbit
	const float mOrbitSpeed; // speed of the orbit
	const float mRotationSpeed; // speed of the rotation
	const float mTilt; // tilt of the planet
	const float mInclination; // inclination of the planet

	glm::mat4 mScale; // size of planet
	glm::vec3 mCenterOfOrbit; // center of the planets orbit
	glm::vec3 mPosition; // position of the planet before inclination
	float currentRotation; // current rotation of the planet 
	float currentOrbit; // current orbit of the planet 
	
	glm::mat4 mModel; // model matrix

public:
	int orbitCount = 0;
	int rotationCount = 0;
	Planet(std::string const& texture, float orbitRadius, float scale, float orbitSpeed, float rotationSpeed, float tilt, float inclination, glm::vec3 centerOfOrbit);

	void updateCenterOfOrbit(glm::vec3 center) { mCenterOfOrbit = center; }
	
	Texture* getTexture() const { return mTexture.get(); }

	glm::mat4& getModel() { return mModel; }

	glm::vec3 getPosition() const { return mModel[3]; }

	float getRotation() const { return currentRotation; }
	float getCurrentOrbit() const { return currentOrbit; }
	float getCurrentRotation() const { return currentRotation; }
	glm::vec3 getCenterOfOrbit() const { return mCenterOfOrbit; }

	void update(float deltaTime);
	void Reset();
};

