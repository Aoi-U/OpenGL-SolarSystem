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
	Planet(std::string const& texture, float orbitRadius, float scale, float orbitSpeed, float rotationSpeed, float tilt, float inclination, glm::vec3 centerOfOrbit);

	void updateCenterOfOrbit(glm::vec3 center) { mCenterOfOrbit = center; } // update the center of orbit (used for moons, orbiting relative to a planet)
	
	Texture* getTexture() const { return mTexture.get(); } // returns the texture of the planet

	glm::mat4& getModel() { return mModel; } // returns the model matrix of the planet

	glm::vec3 getPosition() const { return mModel[3]; } // returns the position of the planet

	void update(float deltaTime); // updates the model matrix of the planet
	void Reset(); // resets the planet to its default position
};

