//
// Created by mohammad on 29/01/25.
//

#include "Game.hpp"

#include "GLDebug.h"
#include "Log.h"

#include <functional>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

//======================================================================================================================

class InputManager final : public CallbackInterface
{
public:

    explicit InputManager() = default;

    virtual ~InputManager() = default;

    void keyCallback(
        int const key,
        int const scancode,
        int const action,
        int const mods
    ) override
    {
    }

    // One way to handle input is to keep track of keys being pressed and released.
    // bool IsKeyDown(int const key) const;

private:

};

//======================================================================================================================

Game::Game()
{
    mPath = AssetPath::Instance();
    mTime = Time::Instance();

    glfwWindowHint(GLFW_SAMPLES, 32);
    mWindow = std::make_unique<Window>(800, 800, "Pirate ship game");

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

    // Initializing shaders
    mBasicShader = std::make_unique<ShaderProgram>(
        mPath->Get("shaders/basic.vert"),
        mPath->Get("shaders/basic.frag")
    );

    mInputManager = std::make_shared<InputManager>();
    mWindow->setCallbacks(mInputManager);

    {// Quad geometry
        mQuadGeometry = std::make_unique<GPU_Geometry>();

        CPU_Geometry cpuGeometry{};
        // Note: We have to reuse the same quad with different matrices
        cpuGeometry.positions.emplace_back(-1.f, 1.f, 0.f);
        cpuGeometry.positions.emplace_back(-1.f, -1.f, 0.f);
        cpuGeometry.positions.emplace_back(1.f, -1.f, 0.f);
        cpuGeometry.positions.emplace_back(-1.f, 1.f, 0.f);
        cpuGeometry.positions.emplace_back(1.f, -1.f, 0.f);
        cpuGeometry.positions.emplace_back(1.f,1.f, 0.f);

        cpuGeometry.uvs.emplace_back(0.f, 1.f);
        cpuGeometry.uvs.emplace_back(0.f, 0.f);
        cpuGeometry.uvs.emplace_back(1.f, 0.f);
        cpuGeometry.uvs.emplace_back(0.f, 1.f);
        cpuGeometry.uvs.emplace_back(1.f, 0.f);
        cpuGeometry.uvs.emplace_back(1.f, 1.f);

        mQuadGeometry->Update(cpuGeometry);
    }

    mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Ships/ship (6).png"), GL_NEAREST);
}

//======================================================================================================================

Game::~Game()
{
    // ImGui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

//======================================================================================================================

void Game::Run()
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

        Render();

        // glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui (if used)

        // Starting the new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        UpdateGameStatus();

        ImGui::Render();	// Render the ImGui window
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Some middleware thing

        mWindow->swapBuffers(); //Swap the buffers while displaying the previous
    }
}

//======================================================================================================================

void Game::Update(float const deltaTime)
{
    // if (mInputManager->IsKeyDown(GLFW_KEY_R))
    // {
    //     ResetGame();
    // }
}

//======================================================================================================================

void Game::Render()
{
    mBasicShader->use(); // Use "this" shader to render
    mQuadGeometry->bind();
    mTexture->bind();
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1);
}

//======================================================================================================================

void Game::UpdateGameStatus() const
{
    // Putting the text-containing window in the top-left of the screen.
    ImGui::SetNextWindowPos(ImVec2(5, 5));

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Green text
    // Setting flags
    ImGuiWindowFlags textWindowFlags = ImGuiWindowFlags_NoMove | // text "window" should not move
        ImGuiWindowFlags_NoResize | // should not resize
        ImGuiWindowFlags_NoCollapse | // should not collapse
        ImGuiWindowFlags_NoSavedSettings | // don't want saved settings mucking things up
        ImGuiWindowFlags_AlwaysAutoResize | // window should auto-resize to fit the text
        ImGuiWindowFlags_NoBackground | // window should be transparent; only the text should be visible
        ImGuiWindowFlags_NoDecoration | // no decoration; only the text should be visible
        ImGuiWindowFlags_NoTitleBar; // no title; only the text should be visible

    // Begin a new window with these flags. (bool *)0 is the "default" value for its argument.
    ImGui::Begin("scoreText", (bool *)0, textWindowFlags);

    // Scale up text a little, and set its value
    ImGui::SetWindowFontScale(1.5f);
    if (mHealth <= 0)
    {
        ImGui::Text("You lost the game! Press R to reload the game.");
    }
    else if (mScore >= PirateShipCount)
    {
        ImGui::Text("You won! Press R to reload the game.");
    }
    else
    {
        ImGui::Text("Destroy enemy pirate ship and survive!");
    }
    ImGui::Text("Score: %d", mScore); // Second parameter gets passed into "%d"
    ImGui::Text("Health: %d", mHealth);
    // End the window.
    ImGui::End();

    ImGui::PopStyleColor();
}

//======================================================================================================================
