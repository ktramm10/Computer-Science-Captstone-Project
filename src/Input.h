#pragma once

#include "Component.h"

class Input : public Component {
    public:
    float moveSpeed = 100.0f; // Movement speed of the entity
    float jumpForce = 300.0f; // Jump force of the entity

    bool canMove = true;
    bool canJump = true;

    Input() = default;
    Input(float moveSpeed, float jumpForce) : moveSpeed(moveSpeed), jumpForce(jumpForce){}
};