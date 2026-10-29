#pragma once

#include "Scene.h"
#include "ResourceManager.h"

/**
 * @class InputSystem
 * @brief Input system class that handles player and AI input for entities in a scene.
 * @details This class provides functionality to update the input system for a given scene, processing player and AI input, updating UI text, component timers, and handling entity spawning.
 */
class InputSystem {
    public:
    void update(Scene& scene, float dt, sf::RenderWindow& window, ResourceManager& resourceManager, AudioSystem& audioSystem);

};