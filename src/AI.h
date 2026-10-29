#pragma once
#include "Component.h"
#include "MovementType.h"
#include "cmath"

/**
 * @enum class PlatformerMovementStates
 * @brief Enum representing different movement states that NPC's can move between.
 * 
 */
enum class PlatformerMovementStates {
    Strafe,
    Follow,
    Idle
};

/**
 * @class AI
 * @brief AI component class that inherits from Component.
 * 
 * @details This class represents the AI behavior of an entity in a game. It contains various properties and
 *  methods related to movement, including movement type, speed, acceleration, deceleration, and specific behaviors for both platformer and top-down movement types.
 */
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