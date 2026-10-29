#pragma once
#include <memory>
#include "Script.h"

class Entity;
class Scene;

/**
 * @class ScriptSystem
 * @brief System that dispatches script events and executes script actions for entities.
 */
class ScriptSystem {
    private:
    void runEvent(Scene& scene, std::shared_ptr<Entity> entity, Script& script, ScriptEvent event, float dt);

    public:
    /**
     * @brief Updates script components and dispatches start and update events.
     * @param scene The scene containing scripted entities.
     * @param dt The delta time since the previous update.
     */
    void update(Scene& scene, float dt);

    /**
     * @brief Dispatches a timeout event for an entity.
     * @param scene The scene containing the entity.
     * @param dt The delta time for the current frame.
     * @param entity The entity whose timer expired.
     */
    void OnTimeout(Scene& scene, float dt, std::shared_ptr<Entity> entity);

    /**
     * @brief Dispatches collision-related script events.
     * @param scene The scene where the collision occurred.
     * @param dt The delta time for the current frame.
     * @param EntityA The entity whose script should receive the collision event.
     * @param EntityB The other entity involved in the collision.
     */
    void onCollision(Scene &scene, float dt, std::shared_ptr<Entity> EntityA, std::shared_ptr<Entity> EntityB);

    /**
     * @brief Dispatches an entity destruction script event.
     * @param scene The scene containing the destroyed entity.
     * @param dt The delta time for the current frame.
     * @param entity The entity being destroyed.
     */
    void onDestroyed(Scene& scene, float dt, std::shared_ptr<Entity> entity);

    /**
     * @brief Dispatches a switch activation script event.
     * @param scene The scene containing the switch.
     * @param dt The delta time for the current frame.
     * @param entity The entity whose switch was activated.
     */
    void onSwitchActivate(Scene& scene, float dt, std::shared_ptr<Entity> entity);

    /**
     * @brief Dispatches a switch deactivation script event.
     * @param scene The scene containing the switch.
     * @param dt The delta time for the current frame.
     * @param entity The entity whose switch was deactivated.
     */
    void onSwitchDeactivate(Scene& scene, float dt, std::shared_ptr<Entity> entity);

    /**
     * @brief Dispatches a power-up collected script event.
     * @param scene The scene containing the power-up entity.
     * @param dt The delta time for the current frame.
     * @param entity The power-up entity that was collected.
     */
    void OnPowerUpCollected(Scene& scene, float dt, std::shared_ptr<Entity> entity);

    /**
     * @brief Dispatches a player death event to all active scripted entities.
     * @param scene The scene containing the scripted entities.
     * @param dt The delta time for the current frame.
     */
    void OnPlayerDeath(Scene& scene, float dt);

    /**
     * @brief Dispatches destruction events for entities that are no longer alive.
     * @param scene The scene containing the entities to inspect.
     * @param dt The delta time for the current frame.
     */
    void dispatchDestroyedEvents(Scene& scene, float dt);
};