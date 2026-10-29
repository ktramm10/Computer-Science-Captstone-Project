#include "Scene.h"

#include <algorithm>
#include <iostream>

/**
 * @brief Retrieves all scene JSON file paths from a folder.
 * @param folder The folder to search for scene files.
 * @return A vector containing paths for each JSON scene file found.
 */
std::vector<std::filesystem::path> Scene::getSceneFiles(const std::filesystem::path &folder)
{
    std::vector<std::filesystem::path> sceneFiles;
    if (!std::filesystem::exists(folder) || !std::filesystem::is_directory(folder)) {
        std::cerr << "Scene folder does not exists: " << folder << std::endl;
        return sceneFiles;
    }
    for (const auto& file : std::filesystem::directory_iterator(folder)) {
        if (!file.is_regular_file()) continue;
        if (file.path().extension() == ".json") sceneFiles.push_back(file.path());
    }
    return sceneFiles;
}

/**
 * @brief Loads scene metadata for display in the scene selection UI.
 * @param folder The folder containing saved scene JSON files.
 * @return A vector of SceneFile entries containing display names and paths.
 */
std::vector<SceneFile> Scene::loadSceneList(const std::filesystem::path &folder)
{
    std::vector<SceneFile> scenes;
    if (!std::filesystem::exists(folder) || !std::filesystem::is_directory(folder)) {
        std::cerr << "Scene folder does not exist: " << folder << std::endl;
        return scenes;
    }

    for (const auto& file : std::filesystem::directory_iterator(folder)) {
        if (!file.is_regular_file()) continue;
        auto path = file.path();
        if (path.extension() == ".json") {
            scenes.push_back({path.stem().string(),path});
        }
    }
    return scenes;
}

/**
 * @brief Creates a new entity with the next available scene ID.
 * @param name The name to assign to the entity.
 * @return A shared pointer to the newly created entity.
 */
std::shared_ptr<Entity> Scene::createEntity(const std::string& name) {
    auto entity = std::make_shared<Entity>(nextEntityID++, name);
    entities.push_back(entity);
    return entity;
}

/**
 * @brief Creates a new entity with a specific ID.
 * @param id The ID to assign to the entity.
 * @param name The name to assign to the entity.
 * @return A shared pointer to the created entity, or the existing entity if the ID is already in use.
 */
std::shared_ptr<Entity> Scene::createEntityWithID(unsigned int id, const std::string& name) {
    auto existingEntity = getEntityByID(id);
    if (existingEntity) {
        std::cerr << "Entity ID already exists: " << id << std::endl;
        return existingEntity;
    }

    auto entity = std::make_shared<Entity>(id, name);
    entities.push_back(entity);
    nextEntityID = std::max(nextEntityID, id + 1);
    return entity;
}

/**
 * @brief Retrieves the scene's active entity collection.
 * @return A const reference to the vector of active entities.
 */
const std::vector<std::shared_ptr<Entity>>& Scene::getEntities() const {
    return entities;
}

/**
 * @brief Retrieves the entity currently assigned as the player.
 * @return A shared pointer to the player entity, or nullptr if none is assigned.
 */
std::shared_ptr<Entity> Scene::getPlayerEntity() const
{
    return playerEntity;
}

/**
 * @brief Marks an entity for destruction.
 * @param entity The entity to destroy.
 */
void Scene::destroyEntity(std::shared_ptr<Entity> entity) {
    entity->destroy();
}

/**
 * @brief Removes entities that have been marked as no longer alive.
 */
void Scene::cleanupDestroyedEntities() {
    entities.erase(std::remove_if(entities.begin(), entities.end(),
        [](const std::shared_ptr<Entity>& entity) {
            return !entity->isAlive();
        }), entities.end());
}

/**
 * @brief Prints debug information for all entities and their component masks.
 */
void Scene::debugPrintEntities() const {
    std::cout << "Scene entities (" << entities.size() << ")" << std::endl;

    for (const auto& entity : entities) {
        const uint32_t componentMask = entity->getComponentMask();

        std::cout << "Entity " << entity->getID()
            << " [" << entity->getName() << "]"
            << " alive=" << (entity->isAlive() ? "true" : "false")
            << " player=" << (entity == playerEntity ? "true" : "false")
            << " blueprint=" << (entity->isBlueprint() ? "true" : "false")
            << std::endl;

        std::cout << "  Components:";
        bool hasAnyComponent = false;

        auto printComponent = [&](ComponentTypes::Value type, const char* name) {
            if ((componentMask & type) == type) {
                std::cout << " " << name;
                hasAnyComponent = true;
            }
        };

        printComponent(ComponentTypes::Transform, "Transform");
        printComponent(ComponentTypes::Sprite, "Sprite");
        printComponent(ComponentTypes::Physics, "Physics");
        printComponent(ComponentTypes::Collision, "Collision");
        printComponent(ComponentTypes::Input, "Input");
        printComponent(ComponentTypes::Health, "Health");
        printComponent(ComponentTypes::PickUp, "PickUp");
        printComponent(ComponentTypes::ContactDamage, "ContactDamage");
        printComponent(ComponentTypes::AI, "AI");
        printComponent(ComponentTypes::Shooter, "Shooter");
        printComponent(ComponentTypes::LifeTime, "LifeTime");
        printComponent(ComponentTypes::Spawner, "Spawner");
        printComponent(ComponentTypes::Door, "Door");
        printComponent(ComponentTypes::Key, "Key");
        printComponent(ComponentTypes::Script, "Script");

        if (!hasAnyComponent) {
            std::cout << " None";
        }

        std::cout << std::endl;
    }
}

/**
 * @brief Finds an entity by its unique scene ID.
 * @param id The ID to search for.
 * @return A shared pointer to the matching entity, or nullptr if none is found.
 */
std::shared_ptr<Entity> Scene::getEntityByID(unsigned int id) const
{
    for (const auto& entity : entities) {
        if (entity && entity->getID() == id) {
            return entity;
        }
    }
    return nullptr;
}

/**
 * @brief Calculates an entity's world position, accounting for parent transforms.
 * @param entity The entity whose world position should be calculated.
 * @return The entity's world position.
 */
sf::Vector2f Scene::getWorldPosition(std::shared_ptr<Entity> entity) const
{
    if (entity->getParentEntity().lock()) {
        auto parent = entity->getParentEntity().lock();
        return getWorldPosition(parent) + entity->getComponent<Transform>()->getLocalPosition();
    }
    return entity->getComponent<Transform>()->getWorldPosition();
}

/**
 * @brief Moves an entity in either local or world space depending on whether it has a parent.
 * @param entity The entity to move.
 * @param delta The movement delta to apply.
 */
void Scene::moveEntity(std::shared_ptr<Entity> entity, sf::Vector2f delta)
{
    if (!entity) return;

    auto transform = entity->getComponent<Transform>();
    if (!transform) return;
    if (entity->getParentEntity().lock()) {
        transform->setLocalPosition(transform->getLocalPosition() + delta);
    } 
    else {
        transform->moveWorldPosition(delta);
    }
}


/**
 * @brief Sets one entity as the parent of another while preserving the child's world position.
 * @param childID The ID of the child entity.
 * @param parentID The ID of the parent entity.
 */
void Scene::setParent(unsigned int childID, unsigned int parentID)
{
    if (childID == parentID) {
        std::cerr << "Cannot set entity as its own parent: " << childID << std::endl;
        return;
    }
    auto child = getEntityByID(childID);
    auto parent = getEntityByID(parentID);
    if (!child) {
        std::cerr << "Child entity not found: " << childID << std::endl;
        return;
    }
    if (!parent) {
        std::cerr << "Parent entity not found: " << parentID << std::endl;
        return;
    }
    auto currentParent = child->getParentEntity().lock();
    if (currentParent && currentParent->getID() == parentID) {
        return;
    }
    if (isChildOf(parentID, childID)) {
        std::cerr << "Cannot set entity as parent because it would create a cycle: childID=" << childID << ", parentID=" << parentID << std::endl;
        return;
    }

    auto childTransform = child->getComponent<Transform>();
    sf::Vector2f childWorldPosition = childTransform ? getWorldPosition(child) : sf::Vector2f{0.0f, 0.0f};
    sf::Vector2f parentWorldPosition = parent->getComponent<Transform>() ? getWorldPosition(parent) : sf::Vector2f{0.0f, 0.0f};

    if (child->getParentEntity().lock()) {
        clearParent(childID);
    }
    child->setParentEntity(parent);
    auto& children = parent->getChildEntitiesIDs();
    if (std::find(children.begin(), children.end(), childID) == children.end()) {
        children.push_back(childID);
    }
    if (childTransform) {
        childTransform->setLocalPosition(childWorldPosition - parentWorldPosition);
    }
    
}

/**
 * @brief Removes an entity's parent while preserving its world position.
 * @param childID The ID of the child entity to detach.
 */
void Scene::clearParent(unsigned int childID)
{
    auto child = getEntityByID(childID);
    if (!child) {
        std::cerr << "Child entity not found: " << childID << std::endl;
        return;
    }
    auto transform = child->getComponent<Transform>();
    sf::Vector2f worldPosition = transform ? getWorldPosition(child) : sf::Vector2f{0.0f, 0.0f};
    auto parent = child->getParentEntity().lock();
    if (parent) {
        auto& siblings = parent->getChildEntitiesIDs();
        siblings.erase(std::remove(siblings.begin(), siblings.end(), childID), siblings.end());
    }
    child->setParentEntity(nullptr);
    if (transform) {
        transform->setWorldPosition(worldPosition);
        transform->setLocalPosition({0.0f, 0.0f});
    }
}

/**
 * @brief Removes a child relationship from a parent entity.
 * @param parentID The ID of the parent entity.
 * @param childID The ID of the child entity to remove.
 */
void Scene::removeChild(unsigned int parentID, unsigned int childID)
{
    auto parent = getEntityByID(parentID);
    auto child = getEntityByID(childID);
    if (!parent) {
        std::cerr << "Parent entity not found: " << parentID << std::endl;
        return;
    }
    if (!child) {
        std::cerr << "Child entity not found: " << childID << std::endl;
        return;
    }
    for (auto id : parent->getChildEntitiesIDs()) {
        if (id == childID) {
            auto& siblings = parent->getChildEntitiesIDs();
            siblings.erase(std::remove(siblings.begin(), siblings.end(), childID), siblings.end());
            child->setParentEntity(nullptr);
            return;
        }
    }
    std::cerr << "Child entity " << childID << " is not a child of parent entity " << parentID << std::endl;
}

/**
 * @brief Checks whether one entity is a descendant of another entity.
 * @param childID The ID of the entity to test as the child.
 * @param parentID The ID of the possible ancestor entity.
 * @return True if the child is descended from the parent, false otherwise.
 */
bool Scene::isChildOf(unsigned int childID, unsigned int parentID) const
{
    auto child = getEntityByID(childID);
    if (!child) {
        return false;
    }

    auto parent = child->getParentEntity().lock();
    while (parent) {

/**
 * @brief Updates door lock states based on the keys collected in the scene game state.
 */
        if (parent->getID() == parentID) {
            return true;
        }
        parent = parent->getParentEntity().lock();
    }
    return false;
}

void Scene::updateDoorLockStates()
{
    for (auto& entity: entities) {
        auto componentMask = entity->getComponentMask();
        if ((componentMask & ComponentTypes::Door) == 0 || (componentMask & ComponentTypes::Sprite) == 0) {
            continue;
        }
        auto door = entity->getComponent<Door>();
        auto sprite = entity->getComponent<Sprite>();
        for (auto keyID : gameState.keyIDs) {
            if (door->keyId == keyID) {
                door->isLocked = false;
                sprite->texID = door->unlockedTexID;
                sprite->sprite->setTexture(rm.getTexture(door->unlockedTexID));
            }
        }
    }
}

/**
 * @brief Initializes scene-level resource access used by runtime state updates.
 * @param resourceManager The resource manager to copy into the scene.
 */
void Scene::initialize(ResourceManager& resourceManager) {
    rm = resourceManager;
}

/**
 * @brief Clears all scene entities, player state, scene load requests, and game state values.
 */
void Scene::clear() {
    entities.clear();
    playerEntity.reset();
    requestedSceneFilePath.clear();
    nextEntityID = 1;
    gameState.coins = 0;
    gameState.score = 0;
    gameState.experience = 0.0f;
    gameState.keyIDs.clear();
}
