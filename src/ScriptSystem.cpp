#include "ScriptSystem.h"
#include "TimerManager.h"
#include "Physics.h"
#include "Transform.h"
#include "PopText.h"
#include "Loot.h"
#include "Random.h"
#include <iostream>

void ScriptSystem::runEvent(Scene& scene, std::shared_ptr<Entity> entity, Script& script, ScriptEvent event, float dt)
{
    for (const auto& rule : script.rules) {
        if (rule.event != event) {
            continue;
        }

        for (const auto& action : rule.actions) {
            switch (action.type) {
            case ScriptActionType::RequestSceneLoad:
                if (!action.target.empty()) {
                    scene.requestSceneLoad(action.target);
                }
                break;
            case ScriptActionType::SetRotation:
                if (auto transform = entity->getComponent<Transform>()) {
                    transform->setRotation(action.value);
                }
                break;
            case ScriptActionType::AddRotation:
                if (auto transform = entity->getComponent<Transform>()) {
                    transform->setRotation(transform->getRotation() + action.value * dt);
                }
                break;
            case ScriptActionType::SetVelocity:
                if (auto physics = entity->getComponent<Physics>()) {
                    physics->velocity = {action.x, action.y};
                }
                break;
            case ScriptActionType::AddVelocity:
                if (auto physics = entity->getComponent<Physics>()) {
                    sf::Vector2f dir = sf::Vector2f{action.x, action.y};
                    physics->velocity += dir * action.value * dt;
                }
                break;
            case ScriptActionType::AddVelocityInRandomDirection:
                if (auto physics = entity->getComponent<Physics>()) {
                    sf::Vector2f dir = sf::Vector2f{Random::range(-1.f, 1.f), Random::range(-1.f, 1.f)};
                    physics->velocity += dir * action.value * dt;
                }
                break;
            case ScriptActionType::SetVelocityInRandomDirection: 
                if (auto physics = entity->getComponent<Physics>()) {
                    physics->velocity = sf::Vector2f{Random::range(-1.f, 1.f), Random::range(-1.f, 1.f)} * action.value;
                }
                break;
            case ScriptActionType::SetSpriteOrigin:
                if (auto sprite = entity->getComponent<Sprite>()) {
                    sprite->sprite->setOrigin({action.x, action.y});
                }
                break;
            case ScriptActionType::SetLocalSpritePosition: {
                auto transform = entity->getComponent<Transform>();
                if (transform) {
                    transform->setLocalPosition({action.x, action.y});
                }
                }
                break;
            case ScriptActionType::StartTimer:
               TimerManager::addTimer(action.value, entity);
                break;
            case ScriptActionType::SetVelocityTowardsPlayer: {
                auto player = scene.getPlayerEntity();
                auto physics = entity->getComponent<Physics>();
                auto transform = entity->getComponent<Transform>();
                if (player && physics && transform) {
                    auto playerTransform = player->getComponent<Transform>();
                    if (playerTransform) {
                        sf::Vector2f entityToPlayerDirection = (playerTransform->getWorldPosition() - transform->getWorldPosition());
                        if (entityToPlayerDirection.lengthSquared() > 0.0f) entityToPlayerDirection = entityToPlayerDirection.normalized();
                        physics->velocity = entityToPlayerDirection * action.value;
                    }
                }
                }
                break;
            case ScriptActionType::UnlockDoor: 
                scene.getGameState().keyIDs.push_back(action.target);
                scene.updateDoorLockStates();        
                break;
            case ScriptActionType::DisableEntity:
                for (auto& entity : scene.getEntities()) {
                    if (action.target == entity->getName()) {
                        entity->setBlueprint(true);
                    }
                }
                break;
            case ScriptActionType::EnableEntity:
                for (auto& entity : scene.getEntities()) {
                    if (action.target == entity->getName()) {
                        entity->setBlueprint(false);
                    }
                }
            break;
            case ScriptActionType::DestroyEntity:
                for (auto& entity : scene.getEntities()) {
                    if (action.target == entity->getName()) {
                        entity->destroy();
                    }
                }
                break;
            case ScriptActionType::SpawnPopText:
                if (auto popText = entity->getComponent<PopText>()) {
                    if (auto loot = entity->getComponent<Loot>()) {
                        std::string lootDropString = popText->textString;
                        if (loot->gold > 0) lootDropString += std::to_string(loot->gold);
                        if (loot->score > 0) lootDropString += std::to_string(loot->score);
                        if (loot->experience > 0) lootDropString += std::to_string(loot->experience);
                        if (!lootDropString.empty()) {
                            popText->textString = lootDropString;
                        }
                    }
                    else if (auto powerUp = entity->getComponent<PowerUp>()) {
                        popText->textString = powerUp->getPowerUpName();
                    }
                    popText->applyProperties();
                    auto popTextEntity = scene.createEntity("PopText " + entity->getName());
                    auto transform = std::make_shared<Transform>(*popText->transform);
                    auto text = std::make_shared<Text>(*popText->text);
                    text->text = std::make_shared<sf::Text>(*popText->text->text);
                    auto lifeTime = std::make_shared<LifeTime>(popText->lifeTimeDuration);
                    popTextEntity->addComponent(transform, ComponentTypes::Transform);
                    popTextEntity->addComponent(text, ComponentTypes::Text);
                    popTextEntity->addComponent(lifeTime, ComponentTypes::LifeTime);
                }
                break;
            case ScriptActionType::CollectLoot:
                if (auto loot = entity->getComponent<Loot>()) {
                    scene.getGameState().coins += loot->gold;
                    scene.getGameState().score += loot->score;
                    scene.getGameState().experience += loot->experience;
                }
                break;
            }
            
            

        }
    }
}

void ScriptSystem::update(Scene &scene, float dt)
{
    for (auto& entity : scene.getEntities()) {
        if (!entity || !entity->isAlive() || entity->isBlueprint()) continue;

        auto script = entity->getComponent<Script>();
        if (!script) {
            continue;
        }

        if (!script->started) {
            runEvent(scene, entity, *script, ScriptEvent::OnStart, dt);
            script->started = true;
        }
        runEvent(scene, entity, *script, ScriptEvent::OnUpdate, dt);
    }

}

void ScriptSystem::OnTimeout(Scene& scene, float dt, std::shared_ptr<Entity> entity) {
    auto script = entity->getComponent<Script>();
    if (script)
    runEvent(scene, entity, *script, ScriptEvent::OnTimeout, dt);
}

void ScriptSystem::onCollision(Scene &scene, float dt, std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB)
{
    auto script = entityA->getComponent<Script>();
    if (script) {
        runEvent(scene, entityA, *script, ScriptEvent::OnCollision, dt);
        if (entityB == scene.getPlayerEntity()) {
            runEvent(scene, entityA, *script, ScriptEvent::OnCollisionWithPlayer, dt);
        }
    }

}

void ScriptSystem::onDestroyed(Scene &scene, float dt, std::shared_ptr<Entity> entity)
{
    auto script = entity->getComponent<Script>();
    if (script)
    runEvent(scene, entity, *script, ScriptEvent::OnDestroyed, dt);

}

void ScriptSystem::onSwitchActivate(Scene &scene, float dt, std::shared_ptr<Entity> entity)
{
    auto script = entity->getComponent<Script>();
    if (script)
    runEvent(scene, entity, *script, ScriptEvent::OnSwitchActivate, dt);
}

void ScriptSystem::onSwitchDeactivate(Scene &scene, float dt, std::shared_ptr<Entity> entity)
{
    auto script = entity->getComponent<Script>();
    if (script)
    runEvent(scene, entity, *script, ScriptEvent::OnSwitchDeactivate, dt);
}

void ScriptSystem::OnPowerUpCollected(Scene &scene, float dt, std::shared_ptr<Entity> entity)
{
    auto script = entity->getComponent<Script>();
    if (script)
    runEvent(scene, entity, *script, ScriptEvent::OnPowerUpCollected, dt);
}

void ScriptSystem::OnPlayerDeath(Scene &scene, float dt)
{
    for (auto& entity : scene.getEntities()) {
        if (!entity || !entity->isAlive() || entity->isBlueprint()) {
            continue;
        }

        auto script = entity->getComponent<Script>();
        if (script) {
            runEvent(scene, entity, *script, ScriptEvent::OnPlayerDeath, dt);
        }
    }
}

void ScriptSystem::dispatchDestroyedEvents(Scene& scene, float dt)
{
    for (auto& entity : scene.getEntities()) {
        if (!entity || entity->isAlive() || entity->isBlueprint()) {
            continue;
        }

        onDestroyed(scene, dt, entity);
    }
}
