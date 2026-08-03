#pragma once
#include "Component.h"

class Loot : public Component {
public:
    int gold = 0;
    int score = 0;
    float experience = 0;

    Loot() = default;

};