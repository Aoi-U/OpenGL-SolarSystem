#pragma once

#include "AssetPath.h"
#include "Geometry.h"
#include "InputManager.hpp"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Time.hpp"
#include "TurnTableCamera.hpp"
#include "Planet.h"

class SolarSystem
{
public:

	explicit SolarSystem();

	~SolarSystem();

	void Run();

private:

	void Update(float deltaTime);

	void UpdatePlanets(float deltaTime);

	void Render();

	void UI();

	void PrepareUnitSphereGeometry();

	void PrepareBackgroundSphereGeometry();

	void OnResize(int width, int height);

	void OnMouseWheelChange(double xOffset, double yOffset) const;

	// GPU GEOMETRY AXIS FOR DEBUG
	std::unique_ptr<GPU_Geometry> xAxisGPU{};
	std::unique_ptr<GPU_Geometry> yAxisGPU{};
	std::unique_ptr<GPU_Geometry> zAxisGPU{};
	// END DEBUG

	std::shared_ptr<AssetPath> mPath{};
	std::shared_ptr<Time> mTime{};
	std::unique_ptr<Window> mWindow;
	std::shared_ptr<InputManager> mInputManager{};

	std::unique_ptr<ShaderProgram> mBasicShader{};
	std::unique_ptr<Texture> mTexture{};

	std::unique_ptr<GPU_Geometry> mUnitSphereGeometry{};
	int mUnitSphereIndexCount{};

	std::unique_ptr<GPU_Geometry> mBackgroundSphereGeometry{};
	int mBackgroundSphereIndexCount{};

	std::unique_ptr<Planet> background{};
	std::unique_ptr<Planet> sun{};
	std::unique_ptr<Planet> earth{};
	std::unique_ptr<Planet> moon{};
	int lastSecond = 0;


	std::unique_ptr<TurnTableCamera> mTurnTableCamera{};
	glm::dvec2 mPreviousCursorPosition{};
	bool mCursorPositionIsSetOnce = false;

	glm::mat4 mProjectionMatrix{};

	float mFovY = 120.0f;
	float mZNear = 0.01f;
	float mZFar = 100.0f;
	float mZoomSpeed = 20.0f;
	float mRotationSpeed = 0.25f;

	float prevTime = 0.0f;


	// GUI stuff
	const char* planetsTarget[3]{ "Sun", "Earth", "Moon" };
	int selectedTarget = 0;
};
