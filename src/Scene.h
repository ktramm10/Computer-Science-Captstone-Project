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
#include "Interactable.h"
#include "Collectable.h"
struct GameState {
    int coins;
};
class Scene {
    private:
    std::vector<std::shared_ptr<Entity>> entities;
    std::shared_ptr<Entity> playerEntity;
    GameState gameState;
    unsigned int nextEntityID = 1;

    public:
    GameState& getGameState() { return gameState; }
    const GameState& getGameState() const { return gameState; }
    std::shared_ptr<Entity> createEntity(const std::string& name);
    void initialize(); // Optional: Initialize the scene, load resources, etc.
    void clear();
    void destroyEntity(std::shared_ptr<Entity> entity);
    void cleanupDestroyedEntities(); // Optional: Remove destroyed entities from the scene
    void debugPrintEntities() const;

    const std::vector<std::shared_ptr<Entity>>& getEntities() const;
    std::shared_ptr<Entity> getPlayerEntity() const;
    void setPlayerEntity(std::shared_ptr<Entity> entity) { playerEntity = entity; }
};
