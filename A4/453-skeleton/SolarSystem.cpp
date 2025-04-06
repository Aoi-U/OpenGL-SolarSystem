#include "SolarSystem.hpp"

#include <filesystem>

#include "GLDebug.h"
#include "Log.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "ShapeGenerator.hpp"

// Step 1: Create a sphere with positions, indices, and uv values
// Step 2: Create the solar system with sun, earth and moon
// Step 3: Add cube map texture for background and

//======================================================================================================================

SolarSystem::SolarSystem()
{
	mPath = AssetPath::Instance();
	mTime = Time::Instance();

	glfwWindowHint(GLFW_SAMPLES, 32);
	mWindow = std::make_unique<Window>(800, 800, "Solar system");

	// Standard ImGui/GLFW middleware
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(mWindow->getGLFWwindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	int samples = 0;
	glGetIntegerv(GL_SAMPLES, &samples);
	Log::info("MSAA Samples: {0}", samples);

	GLDebug::enable(); // ON Submission you may comments this out to avoid unnecessary prints to the console

	mInputManager = std::make_shared<InputManager>(
		[this](int const width, int const height)->void {OnResize(width, height); },
		[this](double const xOffset, double const yOffset)->void {OnMouseWheelChange(xOffset, yOffset); }
	);

	mWindow->setCallbacks(mInputManager);

	PrepareUnitSphereGeometry();
	PrepareBackgroundSphereGeometry();
	PrepareSaturnRingGeometry();

	mBasicShader = std::make_unique<ShaderProgram>(
		mPath->Get("shaders/test.vert"),
		mPath->Get("shaders/test.frag")
	);

	// all planets initial parameters are scaled relative to 365 seconds = one earth year, or 1 second = 1 day
	background = std::make_unique<Planet>("textures/8k_stars_milky_way.jpg", 0.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	planets.emplace_back("textures/8k_sun.jpg", 0.0f, 1.0f, 0.0f, 13.5f, 0.0f, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	planets.emplace_back("textures/2k_mercury.jpg", 2.0f, 0.2f, 4.15f, 2.07f, 0.0f, 7.0f, planets[0].getPosition());
	planets.emplace_back("textures/2k_venus_surface.jpg", 4.0f, 0.5f, 1.62f, 1.56f, 177.4f, 3.0f, planets[0].getPosition());
	planets.emplace_back("textures/8k_earth_daymap.jpg", 6.0f, 0.5f, 1.0f, 365.0f, 30.0f, 15.0f, planets[0].getPosition());
	planets.emplace_back("textures/2k_moon.jpg", 1.0f, 0.125f, 13.4f, 13.4f, 20.0f, 10.0f, planets[2].getPosition());
	planets.emplace_back("textures/2k_mars.jpg", 8.0f, 0.25f, 0.53f, 365.0f, 25.2f, 1.85f, planets[0].getPosition());
	planets.emplace_back("textures/2k_jupiter.jpg", 15.0f, 5.5f, 0.08f, 884.0f, 3.1f, 0.0f, planets[0].getPosition());
	planets.emplace_back("textures/2k_saturn.jpg", 30.0f, 4.5f, 0.03f, 819.0f, 26.73f, 2.48f, planets[0].getPosition());
	planets.emplace_back("textures/2k_uranus.jpg", 40.0f, 2.0f, 0.01f, 515.0f, 97.77f, 0.0f, planets[0].getPosition());
	planets.emplace_back("textures/2k_neptune.jpg", 46.0f, 2.0f, 0.006f, 544.0f, 28.0f, 1.7f, planets[0].getPosition());

	mSaturnRingTexture = std::make_unique<Texture>(mPath->Get("textures/2k_saturn_ring_alpha.png"), GL_NEAREST);

	mTurnTableCamera = std::make_unique<TurnTableCamera>(planets[0].getModel());

	// AXIS FOR DEBUG REMOVE LATER
	CPU_Geometry xAxis{};
	CPU_Geometry yAxis{};
	CPU_Geometry zAxis{};

	xAxis.positions.emplace_back(glm::vec3(-3.0f, 0.0f, 0.0f));
	xAxis.positions.emplace_back(glm::vec3(3.0f, 0.0f, 0.0f));
	xAxis.colors.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
	xAxis.colors.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
	xAxis.normals.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
	xAxis.normals.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
	
	yAxis.positions.emplace_back(glm::vec3(0.0f, -3.0f, 0.0f));
	yAxis.positions.emplace_back(glm::vec3(0.0f, 3.0f, 0.0f));
	yAxis.colors.emplace_back(glm::vec3(0.0f, 3.0f, 0.0f));
	yAxis.colors.emplace_back(glm::vec3(0.0f, 3.0f, 0.0f));
	yAxis.normals.emplace_back(glm::vec3(0.0f, 1.0f, 0.0f));
	yAxis.normals.emplace_back(glm::vec3(0.0f, 1.0f, 0.0f));
	
	zAxis.positions.emplace_back(glm::vec3(0.0f, 0.0f, -3.0f));
	zAxis.positions.emplace_back(glm::vec3(0.0f, 0.0f, 3.0f));
	zAxis.colors.emplace_back(glm::vec3(0.0f, 0.0f, 1.0f));
	zAxis.colors.emplace_back(glm::vec3(0.0f, 0.0f, 1.0f));
	zAxis.normals.emplace_back(glm::vec3(0.0f, 0.0f, 1.0f));
	zAxis.normals.emplace_back(glm::vec3(0.0f, 0.0f, 1.0f));
	
	xAxisGPU = std::make_unique<GPU_Geometry>();
	yAxisGPU = std::make_unique<GPU_Geometry>();
	zAxisGPU = std::make_unique<GPU_Geometry>();

	xAxisGPU->Update(xAxis);
	yAxisGPU->Update(yAxis);
	zAxisGPU->Update(zAxis);
	// DEBUG END HERE
}

//======================================================================================================================

SolarSystem::~SolarSystem()
{
	// ImGui cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

//======================================================================================================================

void SolarSystem::Run()
{
	while (mWindow->shouldClose() == false)
	{
		glfwPollEvents(); // Propagate events to the callback class

		mTime->Update();
		//Update(mTime->DeltaTimeSec());

		float currTime = static_cast<float>(glfwGetTime());
		float dt = currTime - prevTime;
		prevTime = currTime;
		Update(dt);


		// glEnable(GL_FRAMEBUFFER_SRGB); // Expect Colour to be encoded in sRGB standard (as opposed to RGB)
		//glClearColor(0.2f, 0.6f, 0.8f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
		// https://www.viewsonic.com/library/creative-work/srgb-vs-adobe-rgb-which-one-to-use/
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear render screen (all zero) and depth (all max depth)
		glViewport(0, 0, mWindow->getWidth(), mWindow->getHeight());

		Render();

		// glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui (if used)

		// Starting the new ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		UI();

		ImGui::Render(); // Render the ImGui window
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Some middleware thing


		mWindow->swapBuffers(); // Swap the buffers while displaying the previous
	}
}

//======================================================================================================================

void SolarSystem::Update(float const deltaTime)
{
	auto const cursorPosition = mInputManager->CursorPosition();

	if (mCursorPositionIsSetOnce == true)
	{
		if (mInputManager->IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT) == true)
		{
			auto const deltaPosition = cursorPosition - mPreviousCursorPosition;
			mTurnTableCamera->ChangeTheta(-static_cast<float>(deltaPosition.x) * mRotationSpeed * deltaTime);
			mTurnTableCamera->ChangePhi(-static_cast<float>(deltaPosition.y) * mRotationSpeed * deltaTime);
		}
	}

	mCursorPositionIsSetOnce = true;
	mPreviousCursorPosition = cursorPosition;

	mTurnTableCamera->ChangeTarget(planets[selectedTarget].getModel());

	if (playAnimation)
	{
		UpdatePlanets(deltaTime * timeScale);
	}

	if (reset)
	{
		ResetDefaults();
	}
}

void SolarSystem::UpdatePlanets(float time)
{
	for (size_t i = 0; i < planets.size(); i++)
	{
		if (i == 4)
		{
			planets[i].updateCenterOfOrbit(planets[3].getPosition()); // update moons center of orbit to the earth
		}
		planets[i].update(time); // update all planets
	}
}

void SolarSystem::ResetDefaults()
{
	for (Planet& planet : planets)
	{
		planet.Reset();
	}
	UpdatePlanets(0.0f);
	selectedTarget = 0;
	mTurnTableCamera->Reset();
	mTurnTableCamera->ChangeTarget(planets[0].getModel());
}

//======================================================================================================================

void SolarSystem::Render()
{
	mBasicShader->use();

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	float const aspectRatio = static_cast<float>(mWindow->getWidth()) / static_cast<float>(mWindow->getHeight());
	auto const projection = glm::perspective(mFovY, aspectRatio, mZNear, mZFar);
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "projection"), 1, GL_FALSE, reinterpret_cast<float const*>(&projection));

	auto const view = mTurnTableCamera->ViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "view"), 1, GL_FALSE, reinterpret_cast<float const*>(&view));

	// render background
	background->getTexture()->bind();
	auto const model = background->getModel();
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&model));
	mBackgroundSphereGeometry->bind();
	glDrawArrays(GL_TRIANGLES, 0, mBackgroundSphereIndexCount);

	for (Planet& planet : planets)
	{
		// render planets
		planet.getTexture()->bind();
		auto const model = planet.getModel();
		glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&model));
		mUnitSphereGeometry->bind();
		glDrawArrays(GL_TRIANGLES, 0, mUnitSphereIndexCount);

		// DEBUG AXIS LINES
		xAxisGPU->bind();
		glDrawArrays(GL_LINES, 0, 2);
		yAxisGPU->bind();
		glDrawArrays(GL_LINES, 0, 2);
		zAxisGPU->bind();
		glDrawArrays(GL_LINES, 0, 2);
		// END DEBUG
	}

	// render saturn ring
	mSaturnRingTexture->bind();
	auto ringModel = planets[7].getModel();
	ringModel = glm::scale(ringModel, glm::vec3(1.4f, 0.0f, 1.4f));
	//ringModel = glm::rotate(ringModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&ringModel));
	mSaturnRingGeometry->bind();
	glDrawArrays(GL_TRIANGLES, 0, mSaturnRingIndexCount);
	ringModel = glm::rotate(ringModel, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ringModel = glm::rotate(ringModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&ringModel));
	mSaturnRingGeometry->bind();
	glDrawArrays(GL_TRIANGLES, 0, mSaturnRingIndexCount);
	
	
	
	// Hint: Use glDrawElements for using the index buffer (EBO)

}

//======================================================================================================================

void SolarSystem::UI()
{
	ImGui::Begin("Options");
	ImGui::Text("FPS: %f", 1.0f / mTime->DeltaTimeSec());
	ImGui::Combo("Select planet target", &selectedTarget, planetsTarget, IM_ARRAYSIZE(planetsTarget));
	if (ImGui::Button(playAnimation ? "Pause" : "Play"))
	{
		playAnimation = !playAnimation;
	}
	reset = ImGui::Button("Reset animation");
	ImGui::SliderFloat("Time scale", &timeScale, -100.0f, 100.0f);
	ImGui::End();

}

//======================================================================================================================

void SolarSystem::PrepareUnitSphereGeometry()
{
	mUnitSphereGeometry = std::make_unique<GPU_Geometry>();

	auto const unitSphere = ShapeGenerator::Sphere(1.0f, 100, 100);

	mUnitSphereGeometry->Update(unitSphere);

	mUnitSphereIndexCount = static_cast<int>(unitSphere.positions.size());
}

void SolarSystem::PrepareBackgroundSphereGeometry()
{
	mBackgroundSphereGeometry = std::make_unique<GPU_Geometry>();
	auto const backgroundSphere = ShapeGenerator::BackgroundSphere(1.0f, 100, 100);
	mBackgroundSphereGeometry->Update(backgroundSphere);
	mBackgroundSphereIndexCount = static_cast<int>(backgroundSphere.positions.size());
}

void SolarSystem::PrepareSaturnRingGeometry()
{
	mSaturnRingGeometry = std::make_unique<GPU_Geometry>();
	auto const saturnRing = ShapeGenerator::Ring(1.0f, 0.2f, 100);
	mSaturnRingGeometry->Update(saturnRing);
	mSaturnRingIndexCount = static_cast<int>(saturnRing.positions.size());
}

//======================================================================================================================

void SolarSystem::OnResize(int width, int height)
{
	// TODO
}

//======================================================================================================================

void SolarSystem::OnMouseWheelChange(double const xOffset, double const yOffset) const
{
	mTurnTableCamera->ChangeRadius(-static_cast<float>(yOffset) * mZoomSpeed * mTime->DeltaTimeSec());
}

//======================================================================================================================
