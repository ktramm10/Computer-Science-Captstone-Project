#pragma once

#include <memory>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "Transform.h"
#include "Sprite.h"
#include "Physics.h"
#include "Collision.h"
#include "ComponentType.h"
#include "Input.h"
#include "Health.h"

class Scene{
    private:
    std::vector<std::shared_ptr<Entity>> entities;
    unsigned int nextEntityID = 1;

    public:
    std::shared_ptr<Entity> createEntity(const std::string& name);
    void initialize(); // Optional: Initialize the scene, load resources, etc.
    void clear();
    void destroyEntity(std::shared_ptr<Entity> entity);
    void cleanupDestroyedEntities(); // Optional: Remove destroyed entities from the scene

    const std::vector<std::shared_ptr<Entity>>& getEntities() const;
};