#include "TimerManager.h"
#include <algorithm>
#include "ScriptSystem.h"

std::vector<Timer> TimerManager::timers;

void TimerManager::addTimer(float timerDuration, std::shared_ptr<Entity> entity)
{
    Timer timer(timerDuration, entity);
    timers.push_back(timer);
}

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