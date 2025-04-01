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

	mBasicShader = std::make_unique<ShaderProgram>(
		mPath->Get("shaders/test.vert"),
		mPath->Get("shaders/test.frag")
	);
	mTurnTableCamera = std::make_unique<TurnTableCamera>();

	mPlanets.push_back(Planet("textures/2k_stars_milky_way.jpg", { 0.0f, 0.0f, 0.0f }, 20.0f, 0.0f, 0.0f, 0.0f, { 0.0f, 0.0f, 0.0f }));
	mPlanets.push_back(Planet("textures/2k_sun.jpg", { 0.0f, 0.0f, 0.0f }, 1.0f, 0.0f, 0.0f, 0.0f, { 0.0f, 0.0f, 0.0f }));
	mPlanets.push_back(Planet("textures/2k_earth_daymap.jpg", { 3.0f, 0.0f, 0.0f }, 0.5f, 1.0, 1.0f, 45.0f, { 0.0f, 0.0f, 0.0f }));
	mPlanets.push_back(Planet("textures/2k_moon.jpg", { 5.0f, 0.0f, 0.0f }, 0.25f, 2.0, 2.0f, 0.0f, { 3.0f, 0.0f, 0.0f }));

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
		Update(mTime->DeltaTimeSec());

		// glEnable(GL_FRAMEBUFFER_SRGB); // Expect Colour to be encoded in sRGB standard (as opposed to RGB)
		glClearColor(0.2f, 0.6f, 0.8f, 1.0f);
		// https://www.viewsonic.com/library/creative-work/srgb-vs-adobe-rgb-which-one-to-use/
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear render screen (all zero) and depth (all max depth)
		glViewport(0, 0, mWindow->getWidth(), mWindow->getHeight());

		Render(mPlanets[0]);
		Render(mPlanets[1]);
		Render(mPlanets[2]);
		Render(mPlanets[3]);

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
}

//======================================================================================================================

void SolarSystem::Render(const Planet& planet)
{
	mBasicShader->use();
	planet.getTexture()->bind();

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	float const aspectRatio = static_cast<float>(mWindow->getWidth()) / static_cast<float>(mWindow->getHeight());
	auto const projection = glm::perspective(mFovY, aspectRatio, mZNear, mZFar);
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "projection"), 1, GL_FALSE, reinterpret_cast<float const*>(&projection));

	auto const view = mTurnTableCamera->ViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "view"), 1, GL_FALSE, reinterpret_cast<float const*>(&view));

	//auto const model = glm::identity<glm::mat4>();
	auto const model = planet.getModel();

	// NOTE: UPDATE PLANET POSITIONS / SCALES HERE

	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&model));

	mUnitSphereGeometry->bind();

	glDrawArrays(GL_TRIANGLES, 0, mUnitSphereIndexCount);
	// Hint: Use glDrawElements for using the index buffer (EBO)
}

//======================================================================================================================

void SolarSystem::UI()
{
	ImGui::Begin("Options");
	ImGui::Text("FPS: %f", 1.0f / mTime->DeltaTimeSec());
	ImGui::End();
}

//======================================================================================================================

void SolarSystem::PrepareUnitSphereGeometry()
{
	mUnitSphereGeometry = std::make_unique<GPU_Geometry>();

	auto const unitSphere = ShapeGenerator::Sphere(1.0f, 50, 50);

	mUnitSphereGeometry->Update(unitSphere);

	mUnitSphereIndexCount = static_cast<int>(unitSphere.positions.size());
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
