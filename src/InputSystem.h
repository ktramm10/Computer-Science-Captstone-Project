#pragma once

#include "Scene.h"
#include "ResourceManager.h"

class InputSystem {
    public:
    void update(Scene& scene, float dt, sf::RenderWindow& window, ResourceManager& resourceManager);

};