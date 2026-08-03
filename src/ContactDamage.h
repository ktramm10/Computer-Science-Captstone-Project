#pragma once
#include <algorithm>

#include "Component.h"

enum class KnockbackMethod {
    Auto,
    Fixed
};

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