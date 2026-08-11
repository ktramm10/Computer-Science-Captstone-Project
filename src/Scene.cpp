#include "Scene.h"

#include <algorithm>
#include <iostream>

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

std::shared_ptr<Entity> Scene::createEntity(const std::string& name) {
    auto entity = std::make_shared<Entity>(nextEntityID++, name);
    entities.push_back(entity);
    return entity;
}

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

const std::vector<std::shared_ptr<Entity>>& Scene::getEntities() const {
    return entities;
}

std::shared_ptr<Entity> Scene::getPlayerEntity() const
{
    return playerEntity;
}

void Scene::destroyEntity(std::shared_ptr<Entity> entity) {
    entity->destroy();
}

void Scene::cleanupDestroyedEntities() {
    entities.erase(std::remove_if(entities.begin(), entities.end(),
        [](const std::shared_ptr<Entity>& entity) {
            return !entity->isAlive();
        }), entities.end());
}

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

std::shared_ptr<Entity> Scene::getEntityByID(unsigned int id) const
{
    for (const auto& entity : entities) {
        if (entity && entity->getID() == id) {
            return entity;
        }
    }
    return nullptr;
}

sf::Vector2f Scene::getWorldPosition(std::shared_ptr<Entity> entity) const
{
    if (entity->getParentEntity().lock()) {
        auto parent = entity->getParentEntity().lock();
        return getWorldPosition(parent) + entity->getComponent<Transform>()->getLocalPosition();
    }
    return entity->getComponent<Transform>()->getWorldPosition();
}

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

bool Scene::isChildOf(unsigned int childID, unsigned int parentID) const
{
    auto child = getEntityByID(childID);
    if (!child) {
        return false;
    }

    auto parent = child->getParentEntity().lock();
    while (parent) {
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

void Scene::initialize(ResourceManager& resourceManager) {
    rm = resourceManager;
}

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
