#pragma once

#include <memory>
#include <filesystem>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Factions.h"
#include "Entity.h"
#include "Transform.h"
#include "Sprite.h"
#include "Physics.h"
#include "Collision.h"
#include "ComponentType.h"
#include "Input.h"
#include "Health.h"
#include "Interactable.h"
#include "PickUp.h"
#include "Hazard.h"
#include "ContactDamage.h"
#include "AI.h"
#include "Shooter.h"
#include "LifeTime.h"
#include "Spawner.h"
#include "Door.h"
#include "Key.h"
#include "Script.h"
#include "Text.h"
#include "Switch.h"
#include "PowerUp.h"
#include "PopText.h"
#include "Loot.h"
#include "ResourceManager.h"

const static std::filesystem::path sceneFolderPath = "../../src/json/saved_scenes";

struct GameState {
    int coins = 0;
    int score = 0;
    float experience = 0.0f;
    std::vector<std::string> keyIDs;
};

struct SceneFile {
    std::string name;
    std::filesystem::path path;
};

class Scene {
    
    private:
    std::vector<std::shared_ptr<Entity>> entities;
    std::shared_ptr<Entity> playerEntity;
    GameState gameState;
    std::filesystem::path currentScenePath;
    ResourceManager rm;
    unsigned int nextEntityID = 1;

    public:
    std::vector<std::filesystem::path> getSceneFiles(const std::filesystem::path &folder);
    std::vector<SceneFile> loadSceneList(const std::filesystem::path& folder);
    GameState& getGameState() { return gameState; }
    const GameState& getGameState() const { return gameState; }
    std::shared_ptr<Entity> createEntity(const std::string& name);
    std::shared_ptr<Entity> createEntityWithID(unsigned int id, const std::string& name);
    const std::vector<std::shared_ptr<Entity>>& getEntities() const;
    std::shared_ptr<Entity> getPlayerEntity() const;
    const std::filesystem::path& getCurrentScenePath() const { return currentScenePath; }
    void setCurrentScenePath(const std::filesystem::path& scenePath) { currentScenePath = scenePath; }
    void clearCurrentScenePath() { currentScenePath.clear(); }
    sf::Color backgroundColor = sf::Color::Black;
    std::string requestedSceneFilePath = "";
    void setPlayerEntity(std::shared_ptr<Entity> entity) { playerEntity = entity; }
    void requestSceneLoad(const std::string& sceneFilePath) { requestedSceneFilePath = sceneFilePath; }
    bool hasRequestedSceneLoad() const { return !requestedSceneFilePath.empty(); }
    std::string consumeRequestedSceneLoad() { auto path = requestedSceneFilePath; requestedSceneFilePath.clear(); return path; }
    void updateDoorLockStates();
    void initialize(ResourceManager& ResourceManager);
    void clear();
    void destroyEntity(std::shared_ptr<Entity> entity);
    void cleanupDestroyedEntities();
    void debugPrintEntities() const;
    std::shared_ptr<Entity> getEntityByID(unsigned int id) const;
    sf::Vector2f getWorldPosition(std::shared_ptr<Entity> entity) const;
    void moveEntity(std::shared_ptr<Entity> entity, sf::Vector2f delta);
    void setParent(unsigned int childID, unsigned int parentID);
    void clearParent(unsigned int childID);
    void removeChild(unsigned int parentID, unsigned int childID);
    bool isChildOf(unsigned int childID, unsigned int parentID) const;
};
