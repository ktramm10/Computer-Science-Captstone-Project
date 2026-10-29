#pragma once

#include "Scene.h"

/**
 * @class Renderer
 * @brief Renderer class responsible for rendering entities in a scene onto an SFML render window.
 * @details This class provides a method to render the entities in a given scene onto the specified SFML render window.
 *  It handles the rendering of entities with Transform, Sprite, and Text components.
 */
class Renderer
{
public:
    void render(const Scene& scene, sf::RenderWindow& window);
};
