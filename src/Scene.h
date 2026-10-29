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
#include "Sound.h"
#include "ResourceManager.h"
#include "ProjectPaths.h"

/**
 * @brief Folder path where saved scene JSON files are stored.
 */
const static std::filesystem::path sceneFolderPath = resolveProjectPath("src/json/saved_scenes");

/**
 * @brief Runtime game state values tracked by a scene.
 */
struct GameState {
    int coins = 0;
    int score = 0;
    float experience = 0.0f;
    std::vector<std::string> keyIDs;
};

/**
 * @brief Display metadata for a saved scene file.
 */
struct SceneFile {
    std::string name;
    std::filesystem::path path;
};

/**
 * @class Scene
 * @brief Owns entities, hierarchy, game state, and scene-level runtime data.
 */
class Scene {
    
    private:
    std::vector<std::shared_ptr<Entity>> entities;
    std::shared_ptr<Entity> playerEntity;
    GameState gameState;
    std::filesystem::path currentScenePath;
    ResourceManager rm;
    unsigned int nextEntityID = 1;

    public:
    /**
     * @brief Retrieves all scene JSON file paths from a folder.
     * @param folder The folder to search for scene files.
     * @return A vector containing paths for each JSON scene file found.
     */
    std::vector<std::filesystem::path> getSceneFiles(const std::filesystem::path &folder);

    /**
     * @brief Loads scene metadata for display in the scene selection UI.
     * @param folder The folder containing saved scene JSON files.
     * @return A vector of SceneFile entries containing display names and paths.
     */
    std::vector<SceneFile> loadSceneList(const std::filesystem::path& folder);

    /**
     * @brief Retrieves mutable game state for the scene.
     * @return A mutable reference to the scene game state.
     */
    GameState& getGameState() { return gameState; }

    /**
     * @brief Retrieves read-only game state for the scene.
     * @return A const reference to the scene game state.
     */
    const GameState& getGameState() const { return gameState; }

    /**
     * @brief Creates a new entity with the next available scene ID.
     * @param name The name to assign to the entity.
     * @return A shared pointer to the newly created entity.
     */
    std::shared_ptr<Entity> createEntity(const std::string& name);

    /**
     * @brief Creates a new entity with a specific ID.
     * @param id The ID to assign to the entity.
     * @param name The name to assign to the entity.
     * @return A shared pointer to the created entity, or the existing entity if the ID is already in use.
     */
    std::shared_ptr<Entity> createEntityWithID(unsigned int id, const std::string& name);

    /**
     * @brief Retrieves the scene's active entity collection.
     * @return A const reference to the vector of active entities.
     */
    const std::vector<std::shared_ptr<Entity>>& getEntities() const;

    /**
     * @brief Retrieves the entity currently assigned as the player.
     * @return A shared pointer to the player entity, or nullptr if none is assigned.
     */
    std::shared_ptr<Entity> getPlayerEntity() const;

    /**
     * @brief Retrieves the current scene file path.
     * @return A const reference to the current scene path.
     */
    const std::filesystem::path& getCurrentScenePath() const { return currentScenePath; }

    /**
     * @brief Sets the current scene file path.
     * @param scenePath The path to store as the current scene file.
     */
    void setCurrentScenePath(const std::filesystem::path& scenePath) { currentScenePath = scenePath; }

    /**
     * @brief Clears the current scene file path.
     */
    void clearCurrentScenePath() { currentScenePath.clear(); }
    sf::Color backgroundColor = sf::Color::Black;
    std::string requestedSceneFilePath = "";

    /**
     * @brief Assigns the scene's player entity.
     * @param entity The entity to assign as the player.
     */
    void setPlayerEntity(std::shared_ptr<Entity> entity) { playerEntity = entity; }

    /**
     * @brief Requests a scene load to be processed by the engine loop.
     * @param sceneFilePath The scene file path to load.
     */
    void requestSceneLoad(const std::string& sceneFilePath) { requestedSceneFilePath = sceneFilePath; }

    /**
     * @brief Checks whether a scene load has been requested.
     * @return True if a requested scene file path is pending, false otherwise.
     */
    bool hasRequestedSceneLoad() const { return !requestedSceneFilePath.empty(); }

    /**
     * @brief Retrieves and clears the pending scene load request.
     * @return The requested scene file path.
     */
    std::string consumeRequestedSceneLoad() { auto path = requestedSceneFilePath; requestedSceneFilePath.clear(); return path; }

    /**
     * @brief Updates door lock states based on the keys collected in the scene game state.
     */
    void updateDoorLockStates();

    /**
     * @brief Initializes scene-level resource access used by runtime state updates.
     * @param ResourceManager The resource manager to copy into the scene.
     */
    void initialize(ResourceManager& ResourceManager);

    /**
     * @brief Clears all scene entities, player state, scene load requests, and game state values.
     */
    void clear();

    /**
     * @brief Marks an entity for destruction.
     * @param entity The entity to destroy.
     */
    void destroyEntity(std::shared_ptr<Entity> entity);

    /**
     * @brief Removes entities that have been marked as no longer alive.
     */
    void cleanupDestroyedEntities();

    /**
     * @brief Prints debug information for all entities and their component masks.
     */
    void debugPrintEntities() const;

    /**
     * @brief Finds an entity by its unique scene ID.
     * @param id The ID to search for.
     * @return A shared pointer to the matching entity, or nullptr if none is found.
     */
    std::shared_ptr<Entity> getEntityByID(unsigned int id) const;

    /**
     * @brief Calculates an entity's world position, accounting for parent transforms.
     * @param entity The entity whose world position should be calculated.
     * @return The entity's world position.
     */
    sf::Vector2f getWorldPosition(std::shared_ptr<Entity> entity) const;

    /**
     * @brief Moves an entity in either local or world space depending on whether it has a parent.
     * @param entity The entity to move.
     * @param delta The movement delta to apply.
     */
    void moveEntity(std::shared_ptr<Entity> entity, sf::Vector2f delta);

    /**
     * @brief Sets one entity as the parent of another while preserving the child's world position.
     * @param childID The ID of the child entity.
     * @param parentID The ID of the parent entity.
     */
    void setParent(unsigned int childID, unsigned int parentID);

    /**
     * @brief Removes an entity's parent while preserving its world position.
     * @param childID The ID of the child entity to detach.
     */
    void clearParent(unsigned int childID);

    /**
     * @brief Removes a child relationship from a parent entity.
     * @param parentID The ID of the parent entity.
     * @param childID The ID of the child entity to remove.
     */
    void removeChild(unsigned int parentID, unsigned int childID);

    /**
     * @brief Checks whether one entity is a descendant of another entity.
     * @param childID The ID of the entity to test as the child.
     * @param parentID The ID of the possible ancestor entity.
     * @return True if the child is descended from the parent, false otherwise.
     */
    bool isChildOf(unsigned int childID, unsigned int parentID) const;
};
