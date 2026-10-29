#pragma once
#include "Component.h"

/**
 * @class Health
 * @brief Health component class that inherits from Component.
 * @details This class represents the health properties of an entity in a game. It contains various properties related to health, including maximum health and current health.
 */
class Health : public Component {
    public:
    float maxHealth;
    float currentHealth;
    Health() = default;
    Health(float maxHealth, float currentHealth) : maxHealth(maxHealth), currentHealth(currentHealth) {}
    Health(float maxHealth) {
        this->maxHealth = maxHealth;
        currentHealth = maxHealth;
    }
    bool damage(float amount);
    void heal(float amount);
    
};