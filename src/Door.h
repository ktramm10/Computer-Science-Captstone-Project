#pragma once
#include "Component.h"
#include <string>

/**
 * @class Door
 * @brief Door component class that inherits from Component.
 * @details This class represents the properties of a door in a game. It contains various properties related to the door.
 */
class Door : public Component {
    public:
    std::string sceneFilePath = "";
    std::string keyId = "default";
    bool isLocked = false;
    std::string unlockedTexID;

    
};