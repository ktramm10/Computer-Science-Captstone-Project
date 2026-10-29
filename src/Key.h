#pragma once
#include "Component.h"
#include <string>

/**
 * @class Key
 * @brief Key component class that inherits from Component.
 * @details This class represents the properties of a key in a game. It contains a keyId property that can be used to identify the key and its corresponding door.
 */
class Key : public Component {
    public:
    std::string keyId = "default";
};