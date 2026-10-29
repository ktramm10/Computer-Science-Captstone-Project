#include "Health.h"

/**
 * @brief Applies damage to the entity's health.
 * @param amount The amount of damage to apply.
 * @return True if the entity is still alive after taking damage, false if the entity is dead.
 */
bool Health::damage(float amount) // return true if still alive and false if dead
{
    if (currentHealth <= 0) return false;
    currentHealth -= amount;
    return currentHealth > 0;
}

/**
 * @brief Heals the entity's health by the specified amount, ensuring it does not exceed maxHealth.
 * @param amount The amount of health to restore.
 */
void Health::heal(float amount)
{
    if (currentHealth + amount > maxHealth) currentHealth = maxHealth;
    else currentHealth += amount;
}