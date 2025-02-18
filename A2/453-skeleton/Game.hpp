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

class InputManager;

class Game
{
private:

    struct Ship;
    struct CannonBall;
    struct Explosion;
    struct CrocodileCircle;

public:

    explicit Game();

    ~Game();

    void Run();

private:

    void Update(float deltaTime);

    void Render();

    void UpdateGameStatus() const;

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
};
