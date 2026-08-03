#pragma once
#include <algorithm>

#include "Component.h"
#include "MovementType.h"

class Input : public Component {
    public:
    MovementType movementType = MovementType::Platformer;
    // platformer movement vars
    float moveSpeed = 100.0f; // Movement speed of the entity
    float acceleration = 20.f;
    float deceleration = 20.f;
    float jumpForce = 300.0f; // Jump force of the entity
    float movementLockTimer = 0.0f;

    bool canMove = true;
    bool canJump = true;
   

    Input() = default;
    Input(float moveSpeed, float jumpForce, MovementType movementType = MovementType::Platformer) : movementType(movementType), moveSpeed(moveSpeed), jumpForce(jumpForce){}

    // Locks movement for at least duration seconds.
    void lockMovement(float duration) {
        movementLockTimer = std::max(movementLockTimer, duration);
        canMove = false;
    }
};