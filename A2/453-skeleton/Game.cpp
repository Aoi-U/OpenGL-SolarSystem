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

    // check if a key is pressed
    bool IsKeyDown(int const key) const
    {
        return mKeys.find(key) != mKeys.end() && mKeys.at(key);
    }

    // clear the keys
    void ResetKeys()
    {
        mKeys.clear();
    }

private:
    std::unordered_map<int, bool> mKeys; // stores the keys that are pressed 
};

//======================================================================================================================

// resets the game
void Game::ResetGame()
{
    // clear key presses
    mInputManager->ResetKeys();
    
    // reset game variables
    mScore = 0;
    mHealth = 3;
    mGameOver = false;
    cannonCooldown = 0;
    shipWaveTime = 0;

    player = {"ship (6).png", "ship (24).png", Transformation{}, true, 0};
    pirateShips.clear();
    cannonBalls.clear();

    pirateShips = 
    {
        {"ship (1).png", "ship (19).png", Transformation{}, true, 0},
        {"ship (2).png", "ship (20).png", Transformation{}, true, 0},
        {"ship (3).png", "ship (21).png", Transformation{}, true, 0},
        {"ship (4).png", "ship (22).png", Transformation{}, true, 0},
        {"ship (5).png", "ship (23).png", Transformation{}, true, 0},
        {"ship (1).png", "ship (19).png", Transformation{}, true, 0},
        {"ship (2).png", "ship (20).png", Transformation{}, true, 0}
    };  

    // spawn the pirate ships
    SpawnPirateShips();
}

// spawns the pirate ships
void Game::SpawnPirateShips()
{
    float increment = 2.f / (pirateShips.size() + 1);
    float y = 1 - increment;

    // find and set positions for each pirate ship
    for (Ship& pirateShip : pirateShips) {
        // skip inactive ships
        if (!pirateShip.active) {
            continue;
        }
        pirateShip.t = Transformation{};

        // find a random x position and randomly choose the left or right side
        float x = (rand() % 2000) / 1000.f + 2.f;
        x *= (rand() % 2 == 0) ? 1 : -1;

        // set the transformation matrix for the pirate ship
        pirateShip.t.move(x, y);
        if (x < 0) {
            pirateShip.t.rotate(-90.f);
        } else {
            pirateShip.t.rotate(90.f);
        }
        y -= increment;
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

    srand((unsigned int)time(NULL));
    ResetGame(); // call reset game to initialize the game
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

// plays the explosion animation for a ship
void Game::AnimateExplosion(Ship ship) 
{
    Transformation t{};
    t.move(ship.t.getPosition().x, ship.t.getPosition().y);   
    // the explosion duration is 1 second split into 1/3s for each texture 
    if (ship.animationTime <= ExplosionDuration / 3)
    {
        mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Effects/explosion3.png"), GL_NEAREST);
        t.scale(0.042f, 0.042f);
        Render(t);
    } else if (ship.animationTime <= 2 * ExplosionDuration / 3)
    {
        mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Effects/explosion2.png"), GL_NEAREST);
        t.scale(0.06f, 0.06f);
        Render(t);
    } else if (ship.animationTime <= ExplosionDuration)
    {
        mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Effects/explosion1.png"), GL_NEAREST);
        t.scale(0.074f, 0.074f);
        Render(t);
    } 
}

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
        
        // set the texture for each pirate ship
        for (Ship& pirateShip : pirateShips) {
            // set the texture to the active or destroyed texture
            if (!pirateShip.active) {
                mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Ships/" + pirateShip.idDestroyed), GL_NEAREST);
                Render(pirateShip.t);
                AnimateExplosion(pirateShip); // play the explosion animation
            } else 
            {
                mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Ships/" + pirateShip.idActive), GL_NEAREST);
                Render(pirateShip.t); // render pirate ship
            }
        }

        // set the texture for the player
        if (player.active) 
        {
            // determine the player texture based on its current health
            switch (mHealth)
            {
                case 3:
                    player.idActive = "ship (6).png";
                    break;
                case 2:
                    player.idActive = "ship (12).png";
                    break;
                case 1:
                    player.idActive = "ship (18).png";
                    break;
            } 
            
            mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Ships/" + player.idActive), GL_NEAREST);
            Render(player.t); // render player ship
        } else 
        {
            // play the explosion animation for the player
            mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Ships/" + player.idDestroyed), GL_NEAREST);
            Render(player.t);
            AnimateExplosion(player); // play the explosion animation
        }


        // render active cannon balls
        for (CannonBall& cannonBall : cannonBalls) {
            // skip inactive cannon balls
            if (!cannonBall.active) {
                continue;
            }
            mTexture = std::make_unique<Texture>(mPath->Get("textures/PNG/Retina/Ship parts/cannonBall.png"), GL_NEAREST);
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

// checks if two objects are colliding
bool Game::CheckCollision(Transformation t1, Transformation t2)
{
    // get the center and size of the two objects
    glm::vec2 position1 = t1.getPosition();
    glm::vec2 position2 = t2.getPosition();

    glm::vec2 scale1 = t1.getScale();
    glm::vec2 scale2 = t2.getScale();
    
    // checks for circular collision between the two objects
    float distance = glm::distance(position1, position2);
    float radius = (scale1.y + scale2.y) / 2;
    if (distance <= radius) {
        return true;
    }
    
    return false;
}

// checks if an object is colliding with the wall
bool Game::CheckWallCollision(Transformation t)
{
    // get the position and scale of the object
    glm::vec2 position = t.getPosition();
    glm::vec2 scale = t.getScale();

    if (position.x - scale.x / 2 <= -1 || position.x + scale.x / 2 >= 1 || position.y - scale.y / 2 <= -1 || position.y + scale.y / 2 >= 1) {
        return true;
    }
    return false;
}

// controls player movement
void Game::MovePlayer(float const deltaTime)
{
    // calculate the movement vector of the player
    float shipAngle = player.t.getAngle(); 
    float moveX = PlayerMovementSpeed * deltaTime * glm::cos(glm::radians(shipAngle));
    float moveY = PlayerMovementSpeed * deltaTime * glm::sin(glm::radians(shipAngle));

    if (mInputManager->IsKeyDown(GLFW_KEY_W)) // move forward
    {
        player.t.move(moveX, moveY);
        // if it hits a wall, move the player back
        if (CheckWallCollision(player.t)) {
            player.t.move(-moveX, -moveY);
        }
    } else if (mInputManager->IsKeyDown(GLFW_KEY_S)) // move backward
    {
        player.t.move(-moveX, -moveY);
        // if it hits a wall, move the player back
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
    for (Ship& pirateShip : pirateShips) 
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
        if (cannonCooldown >= CannonReloadDuration) 
        {
            cannonCooldown = 0.f; // reset the fire time

            // add a cannon ball to fire
            CannonBall cannonBall = {player.t, 0.f, true};
            cannonBall.t.scale(0.02f, 0.02f);
            cannonBall.t.rotate(90.f);
            cannonBalls.push_back(cannonBall);
        }
    }

    // move all cannon balls
    for (CannonBall& cannonBall : cannonBalls) 
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
        for (Ship& pirateShip : pirateShips) 
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

        // if the ball hits a wall, bounce it off
        if (CheckWallCollision(cannonBall.t)) {
            glm::vec2 pos = cannonBall.t.getPosition();
            glm::vec2 scale = cannonBall.t.getScale();
            // rotate the ball to the opposite direction
            if (pos.x - scale.x / 2 <= -1 || pos.x + scale.x / 2 >= 1) {
                cannonBall.t.rotate(-2 * (ballAngle - 90));
            } else if (pos.y - scale.y / 2 <= -1 || pos.y + scale.y / 2 >= 1) {
                cannonBall.t.rotate(-2 * ballAngle);
            }
        }
    }
}

void Game::Update(float const deltaTime)
{
    // Reset the game if the player presses R
    if (mInputManager->IsKeyDown(GLFW_KEY_R))
        ResetGame();

    for (Ship& pirateShip : pirateShips) {
        if (!pirateShip.active) {
            pirateShip.animationTime += deltaTime;
        }
    }
    if (!player.active) {
        player.animationTime += deltaTime;
    }
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
        std::vector<CannonBall> newCannonBalls;
        for (CannonBall& cannonBall : cannonBalls) {
            if (cannonBall.active) {
                newCannonBalls.push_back(cannonBall);
            }
        }
        cannonBalls = newCannonBalls;

        // check if the game is over
        if (mHealth <= 0) 
        {
            mGameOver = true;
            player.active = false;
        } else if (mScore >= PirateShipCount) 
        {
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

    GLint t_loc = glGetUniformLocation(*mBasicShader, "transformation");
    glUniformMatrix4fv(t_loc, 1, GL_FALSE, glm::value_ptr(t.getTransformationMatrix()));

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
