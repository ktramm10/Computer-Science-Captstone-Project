#include "TimerManager.h"
#include <algorithm>
#include "ScriptSystem.h"

/**
 * @brief Static collection of active timers managed by TimerManager.
 */
std::vector<Timer> TimerManager::timers;

/**
 * @brief Adds a timer that will trigger a timeout event for an entity after a duration.
 * @param timerDuration The amount of time, in seconds, before the timer expires.
 * @param entity The entity associated with the timer.
 */
void TimerManager::addTimer(float timerDuration, std::shared_ptr<Entity> entity)
{
    Timer timer(timerDuration, entity);
    timers.push_back(timer);
}

/**
 * @brief Updates all active timers and dispatches timeout events for expired timers.
 * @param scene The scene containing the timed entities.
 * @param dt The delta time since the previous update.
 * @param scriptSystem The script system used to dispatch timeout events.
 */
void TimerManager::update(Scene& scene, float dt, ScriptSystem& scriptSystem)
{
    std::vector<std::shared_ptr<Entity>> expiredEntities;

    for (auto it = timers.begin(); it != timers.end();) {
        it->timeRemaining -= dt;
        if (it->timeRemaining <= 0.0f) {
            if (auto entity = it->entity.lock()) {
                expiredEntities.push_back(entity);
            }
            it = timers.erase(it);
        } else {
            ++it;
        }
    }

    for (const auto& entity : expiredEntities) {
        scriptSystem.OnTimeout(scene, dt, entity);
    }
}