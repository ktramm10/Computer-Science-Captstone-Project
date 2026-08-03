#pragma once
#include "Component.h"

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