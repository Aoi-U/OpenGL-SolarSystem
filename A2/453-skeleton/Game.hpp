//
// Created by mohammad on 27/01/25.
//

#pragma once

#include "ShaderProgram.h"
#include "AssetPath.h"
#include "Window.h"
#include "Geometry.h"
#include "Time.hpp"
#include "Texture.h"
#include "Transformation.h"

class InputManager;

class Game
{
private:

    // Structs for the game objects
    struct Ship {
        std::string idActive; // file location
        std::string idDestroyed; // file location
        Transformation t; // transformation of the object
        bool active; // if the object is still active
        float animationTime;
    };
    struct CannonBall 
    {
        Transformation t; // transformation of the object
        float lifeTime; // how long the object has been alive
        bool active; // if the object is still active
    };
    struct Explosion;
    struct CrocodileCircle;

public:

    explicit Game();

    ~Game();

    void Run();

private:

    void Update(float deltaTime);

    void Render(Transformation t);

    void UpdateGameStatus() const;

    void ResetGame();

    bool CheckCollision(Transformation t1, Transformation t2); // checks if two objects are colliding
    bool CheckWallCollision(Transformation t); // checks if an object is colliding with the wall

    void AnimateExplosion(Ship ship); // animates the explosion of a ship
    void SpawnPirateShips(); // spawns the pirate ships
    void MovePlayer(float const deltaTime); // controls player movement
    void MovePirateShips(float const deltaTime); // moves the pirate ship across the screen
    void MoveCannonBalls(float const deltaTime); // moves the cannon balls in the shot direction

    // TODO Important, Make sure to use these values for your assignment
    static constexpr float PlayerMovementSpeed = 1.0f;
    static constexpr float PlayerRotationSpeed = 10.0f;
    static constexpr float CannonReloadDuration = 0.25f;

    static constexpr float CannonBallSpeed = 2.0f;
    static constexpr float CannonBallLifeTime = 2.0f;

    static constexpr int PirateShipCount = 7;
    static constexpr float PirateShipMovementSpeed = 1.0f;
    static constexpr float PirateShipWaveCooldown = 10.0f;

    static constexpr float ExplosionDuration = 1.0f;

    static constexpr float WindLoopDuration = 1.0f;
    static constexpr float WindAngleMin = glm::radians(-15.0f);
    static constexpr float WindAngleMax = glm::radians(15.0f);

    static constexpr float CrocodileCircleRotationSpeedMin = 0.5f;
    static constexpr float CrocodileCircleRotationSpeedMax = 0.5f;

    std::shared_ptr<AssetPath> mPath{};
    std::shared_ptr<Time> mTime{};
    std::unique_ptr<Window> mWindow{};
    std::shared_ptr<InputManager> mInputManager{};

    std::unique_ptr<ShaderProgram> mBasicShader{};
    std::unique_ptr<GPU_Geometry> mQuadGeometry{};
    std::unique_ptr<Texture> mTexture{};

    int mScore = 0;
    int mHealth = 3;
    int activeShips = 7;
    bool mGameOver;
    float cannonCooldown;
    float shipWaveTime;
    
    Ship player;
    std::vector<CannonBall> cannonBalls;
    std::vector<Ship> pirateShips;
};
