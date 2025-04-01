#include "Texture.h"
#include "AssetPath.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>

class Planet
{
private:
	std::shared_ptr<AssetPath> mPath;
	std::unique_ptr<Texture> mTexture; // texture of the planet
	glm::vec3 mPosition; // position of the planet
	float mScale; // scale of the planet
	float mOrbitSpeed; // orbit speed of the planet
	float mRotationSpeed; // rotation speed of the planet
	float mTilt; // axis tilt of the planet
	glm::vec3 mCenter; // center of orbit

public:
	Planet(std::string texture, glm::vec3 position, float scale, float orbitSpeed, float rotationSpeed, float tilt, glm::vec3 center);
	
	Texture* getTexture() const { return mTexture.get(); }

	glm::mat4 getModel() const;

	glm::vec3 getPosition() const { return mPosition; }

	void update(float deltaTime);
};

