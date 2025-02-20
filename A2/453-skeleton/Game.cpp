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
#include <glm/gtc/type_ptr.hpp>
#include <ctime>

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
        if (action == GLFW_PRESS)
        {
            mKeysPressed.push_back(key);
        }
        else if (action == GLFW_RELEASE)
        {
            mKeysPressed.erase(
                std::remove(mKeysPressed.begin(), mKeysPressed.end(), key),
                mKeysPressed.end()
            );
        }
    }

    // One way to handle input is to keep track of keys being pressed and released.
    bool IsKeyDown(int const key) const
    {
        return std::find(mKeysPressed.begin(), mKeysPressed.end(), key) != mKeysPressed.end();
    }

private:
    std::vector<int> mKeysPressed;
};

//======================================================================================================================

void Game::ResetGame()
{
    cannonBalls.clear();
    pirateShips.clear();
    // create a new player
    player = Transformation(0.f, 0.f, 0.f, 0.116f, 0.2f);
    pirateShips = {
        {"1", Transformation(0.f, 0.f, 0.f, 0.116f, 0.2f)},
        {"2", Transformation(0.f, 0.f, 0.f, 0.116f, 0.2f)},
        {"3", Transformation(0.f, 0.f, 0.f, 0.116f, 0.2f)},
        {"4", Transformation(0.f, 0.f, 0.f, 0.116f, 0.2f)},
        {"5", Transformation(0.f, 0.f, 0.f, 0.116f, 0.2f)},
        {"7", Transformation(0.f, 0.f, 0.f, 0.116f, 0.2f)},
        {"8", Transformation(0.f, 0.f, 0.f, 0.116f, 0.2f)},
    };
    
    mScore = 0;
    mHealth = 3;
    fireTime = 0.f;
    shipWaveTime = 0.f;
    mGameOver = false;
    SpawnPirateShips();
}

bool overlaps(std::vector<float> vec, float val) {
    for (auto& v : vec) {
        if (v < val + 0.2 && v > val - 0.2) {
            return true;
        }
    }
    return false;
}

void Game::SpawnPirateShips()
{
    std::vector<float> yValues;
    for (auto& pirateShip : pirateShips) 
    {
        pirateShip.t = Transformation{0.f, 0.f, 0.f, 0.116f, 0.2f};
        // choose x value between 2 and 4
        float x = 2 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (4 - 2)));
        x = rand() % 2 == 0 ? x : -x;

        // choose y value between 0.95 and -0.95
        float y = -0.9f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.9f - -0.9f)));
        // make sure y does not overlap with another pirate ship
        do
        {
            y = -0.9f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.9f - -0.9f)));
        } while (overlaps(yValues, y));
        
        yValues.push_back(y);

        pirateShip.t.move(x, y);
        if (x < 0) {
            pirateShip.t.rotate(glm::radians(-90.f));
            // pirateShip.second.setOrientation('L');
        } else {
            pirateShip.t.rotate(glm::radians(90.f));
            // pirateShip.second.setOrientation('R');
        }
    }
}

void Game::SpawnCannonBall() 
{
    // fire cannon ball
    Transformation ball = player;
    ball.scale(0.04f, 0.04f);
    ball.rotate(glm::radians(90.f));
    cannonBalls.push_back({0.f, ball});
}

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
        // top left triangle 
        cpuGeometry.positions.emplace_back(-1.f, 1.f, 0.f);
        cpuGeometry.positions.emplace_back(-1.f, -1.f, 0.f);
        cpuGeometry.positions.emplace_back(1.f, -1.f, 0.f);
        // bottom right triangle
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

    // mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Ships/ship (6).png"), GL_NEAREST);
    ResetGame();
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

        mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Ships/ship (6).png"), GL_NEAREST);
        Render(player); // render player

        for (auto& pirateShip : pirateShips) {

            mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Ships/ship (" + pirateShip.id + ").png"), GL_NEAREST);
            Render(pirateShip.t); // render pirate ship
        }

        for (auto& cannonBall : cannonBalls) {
            mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Ship Parts/cannonBall.png"), GL_NEAREST);
            Render(cannonBall.t); // render cannon ball
        }

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

bool Game::CheckCollision(Transformation& t1, Transformation& t2)
{
    glm::vec2 position1 = t1.getPosition();
    glm::vec2 position2 = t2.getPosition();
    
    float distance = glm::distance(position1, position2);
    if (distance < 0.15f) {
        return true;
    }
    
    return false;
}

bool Game::CheckWallCollision(Transformation& t)
{
    glm::vec2 position = t.getPosition();
    if (position.x < -1 || position.x > 1 || position.y < -1 || position.y > 1) {
        return true;
    }
    return false;
}

void Game::MovePlayer(float const deltaTime)
{
    float shipAngle = player.getAngle();
    float moveX = PlayerMovementSpeed * deltaTime * glm::cos(glm::radians(90.f + shipAngle));
    float moveY = PlayerMovementSpeed * deltaTime * glm::sin(glm::radians(90.f + shipAngle));

    if (mInputManager->IsKeyDown(GLFW_KEY_W))
    {
        player.move(moveX, moveY);
    }
    if (mInputManager->IsKeyDown(GLFW_KEY_S))
    {
        player.move(-moveX, -moveY);
    }
    if (mInputManager->IsKeyDown(GLFW_KEY_A))
    {
        player.rotate(PlayerRotationSpeed * deltaTime);
    }
    if (mInputManager->IsKeyDown(GLFW_KEY_D))
    {
        player.rotate(-PlayerRotationSpeed * deltaTime);
    }
    if (mInputManager->IsKeyDown(GLFW_KEY_SPACE)) 
    {
        if (fireTime >= CannonReloadDuration) {
            fireTime = 0.f; // reset the fire time
            SpawnCannonBall();
        }
    }
}

void Game::MovePirateShips(float const deltaTime)
{
    for (auto& pirateShip : pirateShips) 
    {
        // check if the pirate ship hits a player
        if (CheckCollision(pirateShip.t, player)) {
            // decrease the player's health
            mHealth -= 1;
            // remove the pirate ship
            for (int i = 0; i < pirateShips.size(); i++) {
                if (pirateShips[i].id == pirateShip.id) {
                    pirateShips.erase(pirateShips.begin() + i);
                    break;
                }
            }
            continue;
        }
        
        // move the pirate ship across the screen
        float shipAngle = pirateShip.t.getAngle();
        float moveX = PirateShipMovementSpeed * deltaTime * glm::cos(glm::radians(90.f + shipAngle));
        float moveY = PirateShipMovementSpeed * deltaTime * glm::sin(glm::radians(90.f + shipAngle));
        pirateShip.t.move(moveX, moveY);
    }
}

void Game::MoveCannonBalls(float const deltaTime) 
{
    for (auto& cannonBall : cannonBalls) 
    { 
        cannonBall.lifeTime += deltaTime;
        // if the cannon ball has been alive for too long, remove it
        if (cannonBall.lifeTime >= CannonBallLifeTime) {
            // note: it suffices to remove the first cannon ball in the vector because 
            // oldest cannon balls are at the beginning of the vector
            cannonBalls.erase(cannonBalls.begin());
            continue;
        }

        // add a delay before checking for collision with the player
        if (cannonBall.lifeTime >= 0.1f) {
            // check if the cannon ball hits the player
            if (CheckCollision(cannonBall.t, player)) {
                // decrease the player's health
                mHealth -= 1;
                // remove the cannon ball
                for (int i = 0; i < cannonBalls.size(); i++) {
                    if (cannonBalls[i].lifeTime == cannonBall.lifeTime) {
                        cannonBalls.erase(cannonBalls.begin() + i);
                        break;
                    }
                }
                continue;
            }
        }

        // check if the cannon ball hits a pirate ship
        bool hitPirateShip = false;
        for (auto& pirateShip : pirateShips) 
        {
            if (CheckCollision(cannonBall.t, pirateShip.t)) {
                // if the cannon ball hits a pirate ship, increase the player's score
                mScore += 1;
                // remove the cannon ball
                for (int i = 0; i < cannonBalls.size(); i++) {
                    if (cannonBalls[i].lifeTime == cannonBall.lifeTime) {
                        cannonBalls.erase(cannonBalls.begin() + i);
                        break;
                    }
                }
                // remove the pirate ship
                for (int i = 0; i < pirateShips.size(); i++) {
                    if (pirateShips[i].id == pirateShip.id) {
                        pirateShips.erase(pirateShips.begin() + i);
                        break;
                    }
                }
                hitPirateShip = true;
                break;
            }
        }

        if (hitPirateShip) {
            continue;
        }

        // check if the cannon ball hits a wall, bounce it off 
        glm::vec2 position = cannonBall.t.getPosition();
        glm::vec2 scale = cannonBall.t.getScale();
        float ballAngle = cannonBall.t.getAngle();
        if (position.x <= -1) 
        {
            cannonBall.t.rotate(glm::radians(45.f));
        } 
        if (position.x >= 1) 
        {
            cannonBall.t.rotate(glm::radians(45.f));
        }
        if (position.y <= -1) 
        {
            cannonBall.t.rotate(glm::radians(-45.f));
        }
        if (position.y >= 1) 
        {
            cannonBall.t.rotate(glm::radians(-45.f));
        }

        // move the cannon ball across the screen
        ballAngle = cannonBall.t.getAngle();
        float moveX = CannonBallSpeed * deltaTime * glm::cos(glm::radians(90.f + ballAngle));
        float moveY = CannonBallSpeed * deltaTime * glm::sin(glm::radians(90.f + ballAngle));
        cannonBall.t.move(moveX, moveY);
    }
}

void Game::Update(float const deltaTime)
{
    if (mInputManager->IsKeyDown(GLFW_KEY_R))
    {
        ResetGame();
    }

    if (!mGameOver) {
        MovePlayer(deltaTime);
        MovePirateShips(deltaTime);
        MoveCannonBalls(deltaTime);
        
        if (shipWaveTime >= PirateShipWaveCooldown) {
            shipWaveTime = 0.f;
            // respawn the remaining pirate ships
            SpawnPirateShips();
        }
        
        fireTime += deltaTime;
        shipWaveTime += deltaTime;
        
        if (mHealth <= 0 || mScore >= PirateShipCount) {
            mGameOver = true;
        }
    }
}

//======================================================================================================================

void Game::Render(Transformation t)
{
    mBasicShader->use(); // Use "this" shader to render
    mQuadGeometry->bind();
    mTexture->bind();

    GLint scale_loc = glGetUniformLocation(*mBasicShader, "scale");
    glm::vec3 scale = glm::vec3(t.getScale(), 1.0);
    glUniform3fv(scale_loc, 1, glm::value_ptr(scale));

    GLint transformation_loc = glGetUniformLocation(*mBasicShader, "transformation");
    glUniformMatrix4fv(transformation_loc, 1, GL_FALSE, glm::value_ptr(t.getTransformationMatrix()));

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
