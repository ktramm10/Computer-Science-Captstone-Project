#include "Health.h"

bool Health::damage(float amount) // return true if still alive and false if dead
{
    if (currentHealth <= 0) return false;
    currentHealth -= amount;
    return currentHealth > 0;
}

void Health::heal(float amount)
{
    if (currentHealth + amount > maxHealth) currentHealth = maxHealth;
    else currentHealth += amount;
}