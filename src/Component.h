#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "ComponentType.h"

class Entity;

class Component
{
    private:
    std::weak_ptr<Entity> parentEntity;

    public:
    virtual ~Component() = default;
    std::shared_ptr<Entity> getParent() const { return parentEntity.lock(); }
    void setParent(std::shared_ptr<Entity> entity) { parentEntity = entity; }
};