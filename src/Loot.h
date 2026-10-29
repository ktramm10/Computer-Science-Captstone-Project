#pragma once
#include "Component.h"

/**
 * @class Loot
 * @brief Loot component class that inherits from Component.
 * @details This class represents the loot properties of an entity in a game. It contains various
 */
class Loot : public Component {
public:
    int gold = 0;
    int score = 0;
    float experience = 0;

    Loot() = default;

};