#pragma once
#include <algorithm>

#include "Component.h"
/**
 * @brief Enum representing different methods for applying knockback.
 */
enum class KnockbackMethod {
    Auto,
    Fixed
};
/**
 * @class ContactDamage
 * @brief ContactDamage component class that inherits from Component.
 * @details This class represents the contact damage properties of an entity in a game. It contains various properties related to contact damage,
 *  including damage amount, knockback distance, knockback method, knockback direction, and whether to destroy the entity on contact.
 */
class ContactDamage : public Component {
    public:
    ContactDamage() = default;
    ContactDamage(float damage, float knockback) : damage(damage), knockback(std::max(knockback, 0.0f)) {}
    float damage = 0.f;
    float knockback = 0.0f;
    KnockbackMethod knockbackMethod = KnockbackMethod::Auto;
    sf::Vector2f knockbackDirection = {0.f, 0.f};
    bool destroyEntityOnContact = false;
};