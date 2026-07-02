#pragma once
#include "Component.h"

class Health : public Component {
    private:
    float maxHealth;
    float currentHealth;

    public:
    Health() = default;
    Health(float maxHealth, float currentHealth) : maxHealth(maxHealth), currentHealth(currentHealth) {}
    Health(float maxHealth) {
        this->maxHealth = maxHealth;
        currentHealth = maxHealth;
    }
    bool damage(float amount);
    void heal(float amount);
    float getCurrentHealth() const { return currentHealth; }
    float getMaxHealth() const { return maxHealth; }
    void setCurrentHealth(float health) { currentHealth = health; }
    void setMaxHealth(float health) { maxHealth = health; }
};