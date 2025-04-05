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

	mBasicShader = std::make_unique<ShaderProgram>(
		mPath->Get("shaders/test.vert"),
		mPath->Get("shaders/test.frag")
	);

	// all planets initial parameters are scaled relative to 365 seconds = one earth year, or 1 second = 1 day
	background = std::make_unique<Planet>("textures/8k_stars_milky_way.jpg", 0.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	sun = std::make_unique<Planet>("textures/2k_sun.jpg", 0.0f, 1.0f, 0.0f, 10.1f, 0.0f, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	earth = std::make_unique<Planet>("textures/2k_earth_daymap.jpg", 6.0f, 0.5f, 1.0f, 365.0f, 30.0f, 15.0f, sun->getPosition());
	moon = std::make_unique<Planet>("textures/2k_moon.jpg", 1.0f, 0.2f, 13.4f, 13.4f, 20.0f, 10.0f, earth->getPosition());

	mTurnTableCamera = std::make_unique<TurnTableCamera>(sun->getModel());

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
		Update(mTime->DeltaTimeSec());

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

		//mTurnTableCamera->ChangeTarget()

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

	switch (selectedTarget)
	{
	case 0:
		mTurnTableCamera->ChangeTarget(sun->getModel());
		break;
	case 1:
		mTurnTableCamera->ChangeTarget(earth->getModel());
		break;
	case 2:
		mTurnTableCamera->ChangeTarget(moon->getModel());
		break;
	}

	float currTime = static_cast<float>(glfwGetTime());
	float changeInTime = currTime - prevTime;
	//std::cout << "Current time: " << currTime << " Previous time: " << prevTime << " Change in time: " << changeInTime << std::endl;
	prevTime = currTime;
	// 1s = 1 day
	UpdatePlanets(changeInTime);
	//std::cout << "Current earth orbit: " << earth->getCurrentOrbit() << std::endl;
}

void SolarSystem::UpdatePlanets(float time)
{
	sun->update(time);
	earth->update(time);
	moon->updateCenterOfOrbit(earth->getPosition());
	moon->update(time);

	// every second check earths orbit and rotation
	if (lastSecond != static_cast<int>(glfwGetTime()))
	{
		lastSecond = static_cast<int>(glfwGetTime());
		std::cout << "Earth orbit: " << earth->orbitCount << std::endl;
		std::cout << "Earth rotation: " << earth->rotationCount << std::endl;
		std::cout << "Sun rotation: " << sun->rotationCount << std::endl;
	}


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

	// render sun
	sun->getTexture()->bind();
	auto const sunModel = sun->getModel();
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&sunModel));
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

	// render earth
	earth->getTexture()->bind();
	auto const earthModel = earth->getModel();
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&earthModel));
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

	// render moon
	moon->getTexture()->bind();
	auto const moonModel = moon->getModel();
	glUniformMatrix4fv(glGetUniformLocation(*mBasicShader, "model"), 1, GL_FALSE, reinterpret_cast<float const*>(&moonModel));
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
	
	// Hint: Use glDrawElements for using the index buffer (EBO)

}

//======================================================================================================================

void SolarSystem::UI()
{
	ImGui::Begin("Options");
	ImGui::Text("FPS: %f", 1.0f / mTime->DeltaTimeSec());
	ImGui::Combo("Select planet target", &selectedTarget, planetsTarget, IM_ARRAYSIZE(planetsTarget));
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
