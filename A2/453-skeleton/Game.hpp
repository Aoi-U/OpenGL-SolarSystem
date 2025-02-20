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

    struct Ship {
        std::string id;
        Transformation t;
    };
    struct CannonBall 
    {
        float lifeTime;
        Transformation t;
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

    bool CheckCollision(Transformation& t1, Transformation& t2);
    bool CheckWallCollision(Transformation& t);

    void SpawnPirateShips();
    void SpawnCannonBall();
    void MovePlayer(float const deltaTime);
    void MovePirateShips(float const deltaTime);
    void MoveCannonBalls(float const deltaTime);

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
    bool mGameOver;
    float fireTime;
    float shipWaveTime;
    
    Transformation player;
    std::vector<CannonBall> cannonBalls;
    std::vector<Ship> pirateShips;
};
