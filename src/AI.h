#pragma once
#include "Component.h"
#include "MovementType.h"
#include "cmath"

enum class PlatformerMovementStates {
    Strafe,
    Follow,
    Idle
};


class AI : public Component {
    public:
    MovementType movementType = MovementType::Platformer;
    float moveSpeed = 300.f;
    float acceleration = 80.f;
    float deceleration = 80.f;

    // 2d movement vars
    bool canFollow = false;
    bool verticalStrafe = false;
    float aggroRange = 300.f;
    PlatformerMovementStates currentMovementState = PlatformerMovementStates::Idle;
    PlatformerMovementStates defaultMovementState = PlatformerMovementStates::Idle;
    float strafeDistance = 150.f;
    float strafeStartX = 0.f;
    float strafeStartY = 0.f;
    float strafeDirection = 0.f;
    bool hasStrafeStart = false;

    // top down movement vars
    sf::Vector2f goalPosition = {-1.f, -1.f};
    float shootTimeLeft = 0.0f;
    float shootFrequency = 2.f;
    float errorMargin = 30.f;
    bool decelerating = false;



    AI() = default;
    
};