#include "Scene.h"

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
