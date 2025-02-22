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
            mKeys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            mKeys[key] = false;
        }
    }

    // One way to handle input is to keep track of keys being pressed and released.
    bool IsKeyDown(int const key) const
    {
        return mKeys.find(key) != mKeys.end() && mKeys.at(key);
    }

    void ResetKeys()
    {
        mKeys.clear();
    }

private:
    std::unordered_map<int, bool> mKeys;
};

//======================================================================================================================

void Game::ResetGame()
{
    mInputManager->ResetKeys();
    // deallocate mTexture
    mTexture.reset();
    
    mScore = 0;
    mHealth = 3;
    mGameOver = false;
    cannonCooldown = 0;
    shipWaveTime = 0;

    player = {"textures/PNG/Retina/Ships/ship (6).png", Transformation{}, true};
    pirateShips.clear();
    cannonBalls.clear();

    pirateShips.push_back({"textures/PNG/Retina/Ships/ship (1).png", Transformation{}, true});
    pirateShips.push_back({"textures/PNG/Retina/Ships/ship (2).png", Transformation{}, true});
    pirateShips.push_back({"textures/PNG/Retina/Ships/ship (3).png", Transformation{}, true});
    pirateShips.push_back({"textures/PNG/Retina/Ships/ship (4).png", Transformation{}, true});
    pirateShips.push_back({"textures/PNG/Retina/Ships/ship (5).png", Transformation{}, true});
    pirateShips.push_back({"textures/PNG/Retina/Ships/ship (7).png", Transformation{}, true});
    pirateShips.push_back({"textures/PNG/Retina/Ships/ship (8).png", Transformation{}, true});
    
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
        if (!pirateShip.active) {
            continue;
        }
        pirateShip.t = Transformation{};
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
            pirateShip.t.rotate(-90.f);
        } else {
            pirateShip.t.rotate(90.f);
        }
    }
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
    srand((unsigned int)time(NULL));
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
        
        mTexture = std::make_unique<Texture>(mPath->Get(player.id), GL_NEAREST);
        
        Render(player.t); // render player

        for (auto& pirateShip : pirateShips) {
            if (!pirateShip.active) {
                continue;
            }
            mTexture = std::make_unique<Texture>(mPath->Get(pirateShip.id), GL_NEAREST);
            Render(pirateShip.t); // render pirate ship
        }

        for (auto& cannonBall : cannonBalls) {
            if (!cannonBall.active) {
                continue;
            }
            mTexture = std::make_unique<Texture>(mPath->Get(cannonBall.id), GL_NEAREST);
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

bool Game::CheckCollision(Transformation t1, Transformation t2)
{
    // get the center of the two objects
    glm::vec2 position1 = t1.getPosition();
    glm::vec2 position2 = t2.getPosition();
    
    // checks circular collision of radius 0.15 between two objects
    float distance = glm::distance(position1, position2);
    if (distance < 0.15f) {
        return true;
    }
    
    return false;
}

bool Game::CheckWallCollision(Transformation t)
{
    glm::vec2 position = t.getPosition();
    glm::vec2 scale = t.getScale();
    if (position.x - scale.x / 2 <= -1 || position.x + scale.x / 2 >= 1 || position.y - scale.y / 2 <= -1 || position.y + scale.y / 2 >= 1) {
        return true;
    }
    return false;
}

void Game::MovePlayer(float const deltaTime)
{
    float shipAngle = player.t.getAngle(); // returns angle of ship in degrees
    float moveX = PlayerMovementSpeed * deltaTime * glm::cos(glm::radians(shipAngle));
    float moveY = PlayerMovementSpeed * deltaTime * glm::sin(glm::radians(shipAngle));

    if (mInputManager->IsKeyDown(GLFW_KEY_W)) // move forward
    {
        player.t.move(moveX, moveY);
        if (CheckWallCollision(player.t)) {
            player.t.move(-moveX, -moveY);
        }
    } else if (mInputManager->IsKeyDown(GLFW_KEY_S)) // move backward
    {
        player.t.move(-moveX, -moveY);
        if (CheckWallCollision(player.t)) {
            player.t.move(moveX, moveY);
        }
    } 
    if (mInputManager->IsKeyDown(GLFW_KEY_A)) // rotate left
    {
        player.t.rotate(glm::degrees(PlayerRotationSpeed * deltaTime));
    } else if (mInputManager->IsKeyDown(GLFW_KEY_D)) // rotate right
    {
        player.t.rotate(-glm::degrees(PlayerRotationSpeed * deltaTime));
    }
}

void Game::MovePirateShips(float const deltaTime)
{
    for (auto& pirateShip : pirateShips) 
    {
        // skip inactive pirate ships
        if (!pirateShip.active) {
            continue;
        }
        // check if the pirate ship hits a player
        if (CheckCollision(pirateShip.t, player.t)) {
            // decrease the player's health
            mHealth--;
            // increase the player's score
            mScore++;
            // deactivate the pirate ship
            pirateShip.active = false;            
            continue;
        }
        
        // move the pirate ship across the screen
        float shipAngle = pirateShip.t.getAngle();
        float moveX = PirateShipMovementSpeed * deltaTime * glm::cos(glm::radians(shipAngle));
        float moveY = PirateShipMovementSpeed * deltaTime * glm::sin(glm::radians(shipAngle));
        pirateShip.t.move(moveX, moveY);

    }
}

void Game::MoveCannonBalls(float const deltaTime) 
{
    // if the player presses space, load a cannon ball
    if (mInputManager->IsKeyDown(GLFW_KEY_SPACE)) 
    {
        // make sure the cannon is reloaded
        if (cannonCooldown >= CannonReloadDuration) {
            cannonCooldown = 0.f; // reset the fire time

            // add a cannon ball to fire
            CannonBall cannonBall = {"textures/PNG/Retina/Ship Parts/cannonBall.png", player.t, 0.f, true};
            cannonBall.t.scale(0.02f, 0.02f);
            cannonBall.t.rotate(90.f);
            cannonBalls.push_back(cannonBall);
        }
    }

    // move all cannon balls
    for (auto& cannonBall : cannonBalls) 
    { 
        // skip inactive cannon balls
        if (!cannonBall.active) {
            continue;
        }
        cannonBall.lifeTime += deltaTime; // increase the life time of the cannon ball
        // if the cannon ball has been alive for too long, deactivate it
        if (cannonBall.lifeTime >= CannonBallLifeTime) {
            cannonBall.active = false;
            continue;
        }

        // add a delay before checking for collision with the player
        if (cannonBall.lifeTime >= 0.1f) {
            // check if the cannon ball hits the player
            if (CheckCollision(cannonBall.t, player.t)) {
                // decrease the player's health
                mHealth--;
                // deactivate the cannon ball
                cannonBall.active = false;                
                continue;
            }
        }

        // check if the cannon ball hits a pirate ship
        for (auto& pirateShip : pirateShips) 
        {
            // skip inactive pirate ships
            if (!pirateShip.active) {
                continue;
            }
            // check if the cannon ball hits a pirate ship
            if (CheckCollision(cannonBall.t, pirateShip.t)) {
                // increase the player's score
                mScore++;
                // deactivate the cannon ball
                cannonBall.active = false;
                // deactivate the pirate ship
                pirateShip.active = false;
                break;
            }
        }

        // move the cannon ball 
        float ballAngle = cannonBall.t.getAngle();
        float moveX = CannonBallSpeed * deltaTime * glm::cos(glm::radians(ballAngle));
        float moveY = CannonBallSpeed * deltaTime * glm::sin(glm::radians(ballAngle));
        cannonBall.t.move(moveX, moveY);

        // if the ball hits a wall, reflect it
        if (CheckWallCollision(cannonBall.t)) {
            glm::vec2 pos = cannonBall.t.getPosition();
            glm::vec2 scale = cannonBall.t.getScale();
            // if the ball hits a wall, bounce it off
            if (pos.x + scale.x / 2 >= 1) {
                cannonBall.t.rotate(-2 * (cannonBall.t.getAngle() - 90));
            } else if (pos.x - scale.x / 2 <= -1) {
                cannonBall.t.rotate(-2 * (cannonBall.t.getAngle() + 90));
            } else if (pos.y + scale.y / 2 >= 1) {
                cannonBall.t.rotate(-2 * (cannonBall.t.getAngle() - 180));
            } else if (pos.y - scale.y / 2 <= -1) {
                cannonBall.t.rotate(-2 * cannonBall.t.getAngle());
            } 
        }
    }
}

void Game::Update(float const deltaTime)
{
    // Reset the game if the player presses R
    if (mInputManager->IsKeyDown(GLFW_KEY_R))
        ResetGame();

    // Update the game
    if (!mGameOver) {
        MovePlayer(deltaTime); // move the player
        MovePirateShips(deltaTime); // move the pirate ships
        MoveCannonBalls(deltaTime); // move the cannon balls
        
        // check if a new wave should be spawned
        if (shipWaveTime >= PirateShipWaveCooldown) {
            shipWaveTime = 0.f;
            // spawn the remaining pirate ships
            SpawnPirateShips();
        }
        
        cannonCooldown += deltaTime; // increase reload time
        shipWaveTime += deltaTime; // increase the ship wave time
        
        // remove inactive cannon balls
        cannonBalls.erase(std::remove_if(cannonBalls.begin(), cannonBalls.end(), [](CannonBall& cannonBall) {
            return !cannonBall.active;
        }), cannonBalls.end());
        
        // remove inactive pirate ships
        pirateShips.erase(std::remove_if(pirateShips.begin(), pirateShips.end(), [](Ship& pirateShip) {
            return !pirateShip.active;
        }), pirateShips.end());

        // check if the game is over
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

    // GLint scale_loc = glGetUniformLocation(*mBasicShader, "scale");
    // glm::vec3 scale = glm::vec3(t.getScale(), 1.0);
    // glUniform3fv(scale_loc, 1, glm::value_ptr(scale));

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

    // show frame rate
    ImGui::Text("Frame rate: %.1f FPS", ImGui::GetIO().Framerate);
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
