#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "ComponentType.h"

class Entity;
/**
 * @class Component
 * @brief Base class for all components in the entity-component system.
 * @details This class serves as the base for all components that can be attached to entities in the game.
 *  It provides functionality to manage the parent entity of the component and allows for polymorphic behavior through inheritance.
 */
class Component
{
    private:
    std::weak_ptr<Entity> parentEntity;

    public:
    virtual ~Component() = default;
    std::shared_ptr<Entity> getParent() const { return parentEntity.lock(); }
    void setParent(std::shared_ptr<Entity> entity) { parentEntity = entity; }
};