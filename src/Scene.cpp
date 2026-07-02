#include "Scene.h"

#include <iostream>

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
        printComponent(ComponentTypes::Interactable, "Interactable");
        printComponent(ComponentTypes::Collectable, "Collectable");

        if (!hasAnyComponent) {
            std::cout << " None";
        }

        std::cout << std::endl;
    }
}

void Scene::initialize(){}
void Scene::clear() {
    entities.clear();
    playerEntity.reset();
    nextEntityID = 1;
}
