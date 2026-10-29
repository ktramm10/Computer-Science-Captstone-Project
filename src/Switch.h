#pragma once
#include "Component.h"

/**
 * @class Switch
 * @brief Component that stores activation state and textures for switch entities.
 */
class Switch : public Component {
    public:
    bool activated = false;
    std::string activatedTextureID = "";
    std::string deactivatedTextureID = "";
    float lockout = 0.0f;
    float lockoutTimer = .35f;

};