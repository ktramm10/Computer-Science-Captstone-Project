#pragma once
#include "Component.h"
#include <string>

class Door : public Component {
    public:
    std::string sceneFilePath = "";
    std::string keyId = "default";
    bool isLocked = false;
    std::string unlockedTexID;

    
};