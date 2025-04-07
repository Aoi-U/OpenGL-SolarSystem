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

	PrepareUnitSphereGeometry(); // create a unit sphere geometry for the planets/moons
	PrepareBackgroundSphereGeometry(); // create a background sphere geometry for the stars
	PrepareSaturnRingGeometry(); // create ring geometry for saturn

	mBasicShader = std::make_unique<ShaderProgram>(
		mPath->Get("shaders/test.vert"),
		mPath->Get("shaders/test.frag")
	);

	// create planets
	// note: all planets parameters are scaled relative to 365 seconds = one earth year, or 1 second = 1 day
	background = std::make_unique<Planet>("textures/8k_stars_milky_way.jpg", 0.0f, 85.0f, 0.0f, 0.0f, 0.0f, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	planets.emplace_back("textures/2k_sun.jpg", 0.0f, 1.5f, 0.0f, 13.5f, 0.0f, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f)); // sun
	planets.emplace_back("textures/2k_mercury.jpg", 2.5f, 0.2f, 4.15f, 2.07f, 0.0f, 7.0f, planets[0].getPosition()); // mercury
	planets.emplace_back("textures/2k_venus_surface.jpg", 4.0f, 0.5f, 1.62f, 1.56f, 177.4f, 3.0f, planets[0].getPosition()); // venus
	planets.emplace_back("textures/2k_earth_daymap.jpg", 6.0f, 0.5f, 1.0f, 365.0f, 30.0f, 15.0f, planets[0].getPosition()); // earth
	planets.emplace_back("textures/2k_moon.jpg", 1.0f, 0.125f, 13.4f, 13.4f, 20.0f, 10.0f, planets[2].getPosition()); // earths moon
	planets.emplace_back("textures/2k_mars.jpg", 8.0f, 0.25f, 0.53f, 365.0f, 25.2f, 1.85f, planets[0].getPosition()); // mars
	planets.emplace_back("textures/2k_jupiter.jpg", 20.0f, 5.5f, 0.08f, 884.0f, 3.1f, 0.0f, planets[0].getPosition()); // jupiter
	planets.emplace_back("textures/2k_saturn.jpg", 45.0f, 4.5f, 0.03f, 819.0f, 26.73f, 2.48f, planets[0].getPosition()); // saturn
	planets.emplace_back("textures/2k_uranus.jpg", 60.0f, 2.0f, 0.01f, 515.0f, 97.77f, 0.0f, planets[0].getPosition()); // uranus
	planets.emplace_back("textures/2k_neptune.jpg", 70.0f, 2.0f, 0.006f, 544.0f, 28.0f, 1.7f, planets[0].getPosition()); // netpune

	// create moons
	// mars moons
	planets.emplace_back("textures/2k_moon.jpg", 1.0f, 0.01f, 1100.0f, 1100.0f, 0.0f, 1.0f, planets[5].getPosition()); // phobos
	planets.emplace_back("textures/2k_moon.jpg", 1.0f, 0.01f, 300.0f, 300.0f, 0.0f, 27.58f, planets[5].getPosition()); // deimos

	// jupiter moons
	planets.emplace_back("textures/2k_moon.jpg", 7.0f, 0.2f, 51.0f, 51.0f, 0.0f, 2.2f, planets[6].getPosition()); // ganymede
	planets.emplace_back("textures/2k_moon.jpg", 10.5f, 0.18f, 21.5f, 21.5f, 0.0f, 2.0f, planets[6].getPosition()); // callisto
	planets.emplace_back("textures/2k_moon.jpg", 5.6f, 0.06f, 206.0f, 206.0f, 0.0f, 2.2f, planets[6].getPosition()); // io

	// saturn moons
	planets.emplace_back("textures/2k_moon.jpg", 9.0f, 0.2f, 22.0f, 22.0f, 27.0f, 0.0f, planets[7].getPosition()); // titan
	planets.emplace_back("textures/2k_moon.jpg", 5.0f, 0.1f, 81.0f, 81.0f, 0.0f, 0.0f, planets[7].getPosition()); // rhea
	planets.emplace_back("textures/2k_moon.jpg", 16.2f, 0.09f, 4.6f, 4.6f, 0.0f, 17.28f, planets[7].getPosition()); // lapetus

	// uranus moons
	planets.emplace_back("textures/2k_moon.jpg", 3.0f, 0.06f, 42.0f, 42.0f, 0.0f, 0.0f, planets[8].getPosition()); // titania
	planets.emplace_back("textures/2k_moon.jpg", 3.4f, 0.05f, 28.0f, 28.0f, 0.0f, 0.0f, planets[8].getPosition()); // oberon
	planets.emplace_back("textures/2k_moon.jpg", 2.4f, 0.04f, 91.0f, 91.0f, 0.0f, 0.0f, planets[8].getPosition()); // umbriel

	// neptune moons
	planets.emplace_back("textures/2k_moon.jpg", 4.0f, 0.05f, 63.0f, 63.0f, 0.0f, 130.0f, planets[9].getPosition()); // triton
	planets.emplace_back("textures/2k_moon.jpg", 2.8f, 0.01f, 330.0f, 330.0f, 0.0f, 0.0f, planets[9].getPosition()); // proteus
	planets.emplace_back("textures/2k_moon.jpg", 30.0f, 0.01f, 1.01f, 1.01f, 0.0f, 7.0f, planets[9].getPosition()); // nereid

	mSaturnRingTexture = std::make_unique<Texture>(mPath->Get("textures/2k_saturn_ring_alpha.png"), GL_NEAREST);

	mTurnTableCamera = std::make_unique<TurnTableCamera>(planets[0].getModel());

	mLightModel = glm::mat4(1.0f);
	mLightModel = glm::translate(mLightModel, glm::vec3(0.0f, 0.0f, 0.0f));
	mLightModel = glm::scale(mLightModel, glm::vec3(0.2f));
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

	mTurnTableCamera->ChangeTarget(planets[selectedTarget].getModel()); // change or update the cameras target planet

	// update the simulation if not paused
	if (playAnimation)
	{
		UpdatePlanets(deltaTime * timeScale);
	}

	// reset the simulation if reset is pressed
	if (reset)
	{
		ResetDefaults();
	}
}

// updates the planets/moons position, rotations and center of orbit if moon
void SolarSystem::UpdatePlanets(float time)
{
	for (size_t i = 0; i < planets.size(); i++)
	{
		switch (i) {
		case 4:
			planets[i].updateCenterOfOrbit(planets[3].getPosition()); // update moons center of orbit to earth
			break;
		case 10:
			planets[i].updateCenterOfOrbit(planets[5].getPosition()); // update phobos center of orbit to mars
			planets[i + 1].updateCenterOfOrbit(planets[5].getPosition()); // update deimos center of orbit to mars
			break;
		case 12:
			planets[i].updateCenterOfOrbit(planets[6].getPosition()); // update ganymede center of orbit to jupiter
			planets[i + 1].updateCenterOfOrbit(planets[6].getPosition()); // update callisto center of orbit to jupiter
			planets[i + 2].updateCenterOfOrbit(planets[6].getPosition()); // update io center of orbit to jupiter
			break;
		case 15:
			planets[i].updateCenterOfOrbit(planets[7].getPosition()); // update titan center of orbit to saturn
			planets[i + 1].updateCenterOfOrbit(planets[7].getPosition()); // update rhea center of orbit to saturn
			planets[i + 2].updateCenterOfOrbit(planets[7].getPosition()); // update lapetus center of orbit to saturn
			break;
		case 18:
			planets[i].updateCenterOfOrbit(planets[8].getPosition()); // update titania center of orbit to uranus
			planets[i + 1].updateCenterOfOrbit(planets[8].getPosition()); // update oberon center of orbit to uranus
			planets[i + 2].updateCenterOfOrbit(planets[8].getPosition()); // update umbriel center of orbit to uranus
			break;
		case 21:
			planets[i].updateCenterOfOrbit(planets[9].getPosition()); // update triton center of orbit to neptune
			planets[i + 1].updateCenterOfOrbit(planets[9].getPosition()); // update proteus center of orbit to neptune
			planets[i + 2].updateCenterOfOrbit(planets[9].getPosition()); // update nereid center of orbit to neptune
			break;
		}

		planets[i].update(time); // update all planets
	}
}

// resets the simulation
void SolarSystem::ResetDefaults()
{
	// reset planets
	for (Planet& planet : planets)
	{
		planet.Reset();
	}
	UpdatePlanets(0.0f);
	selectedTarget = 0;

	// reset camera
	mTurnTableCamera->Reset();
	mTurnTableCamera->ChangeTarget(planets[0].getModel());
}

//======================================================================================================================

void SolarSystem::Render()
{
	mBasicShader->use();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	float const aspectRatio = static_cast<float>(mWindow->getWidth()) / static_cast<float>(mWindow->getHeight());
	auto const projection = glm::perspective(mFovY, aspectRatio, mZNear, mZFar);
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "projection"), 1, GL_FALSE, reinterpret_cast<float const*>(&projection));

	auto const view = mTurnTableCamera->ViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "view"), 1, GL_FALSE, reinterpret_cast<float const*>(&view));

	// render background
	background->getTexture()->bind();
	auto const bgModel = background->getModel();
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&bgModel));
	mBackgroundSphereGeometry->bind();
	glDrawArrays(GL_TRIANGLES, 0, mBackgroundSphereIndexCount);

	// render sun 
	planets[0].getTexture()->bind();
	auto const sunModel = planets[0].getModel();
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&sunModel));
	glUniform1i(glGetUniformLocation(*mBasicShader, "isSun"), 1);
	mUnitSphereGeometry->bind();
	glDrawArrays(GL_TRIANGLES, 0, mUnitSphereIndexCount);

	// render planets
	for (size_t i = 1; i < planets.size(); i++)
	{
		planets[i].getTexture()->bind();
		auto const model = planets[i].getModel();
		glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&model));
		glUniform1i(glGetUniformLocation(*mBasicShader, "isSun"), 0);
		mUnitSphereGeometry->bind();
		glDrawArrays(GL_TRIANGLES, 0, mUnitSphereIndexCount);
	}

	// render saturn ring
	mSaturnRingTexture->bind();
	auto ringModel = planets[7].getModel();
	ringModel = glm::scale(ringModel, glm::vec3(1.3f, 0.0f, 1.3f));
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&ringModel));
	mSaturnRingGeometry->bind();
	glDrawArrays(GL_TRIANGLES, 0, mSaturnRingIndexCount);

	// render the bottom side of the ring by flipping it 
	ringModel = glm::rotate(ringModel, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&ringModel));
	mSaturnRingGeometry->bind();
	glDrawArrays(GL_TRIANGLES, 0, mSaturnRingIndexCount);	

	// render point light
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&mLightModel));
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glUniform3fv(glGetUniformLocation(*mBasicShader, "lightColor"), 1, reinterpret_cast<float const*>(&lightColor));
	glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glUniform3fv(glGetUniformLocation(*mBasicShader, "lightPos"), 1, reinterpret_cast<float const*>(&lightPos));
	glm::vec3 viewPos = mTurnTableCamera->Position();
	glUniform3fv(glGetUniformLocation(*mBasicShader, "viewPos"), 1, reinterpret_cast<float const*>(&viewPos));
	glDrawArrays(GL_POINTS, 0, 1);
	
	// Hint: Use glDrawElements for using the index buffer (EBO)

}

//======================================================================================================================

void SolarSystem::UI()
{
	ImGui::Begin("Options");
	ImGui::Text("FPS: %f", 1.0f / mTime->DeltaTimeSec());

	// planet target selection
	ImGui::Combo("Select planet target", &selectedTarget, planetsTarget, IM_ARRAYSIZE(planetsTarget));
	if (ImGui::Button(playAnimation ? "Pause" : "Play"))
	{
		playAnimation = !playAnimation;
	}

	// reset button
	reset = ImGui::Button("Reset animation");

	// time scaling slider
	ImGui::SliderFloat("Time scale", &timeScale, 0.001f, 50.0f);
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
	auto const saturnRing = ShapeGenerator::Ring(1.0f, 0.5f, 200);
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
