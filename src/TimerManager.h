#pragma once
#include <vector>
#include "Scene.h"
#include "ScriptSystem.h"
#include "Timer.h"

/**
 * @class TimerManager
 * @brief Manages active timers and dispatches script timeout events when they expire.
 */
class TimerManager {
    public:
    static std::vector<Timer> timers;

    /**
     * @brief Adds a timer associated with an entity.
     * @param timerDuration The duration of the timer in seconds.
     * @param entity The entity associated with the timer.
     */
    static void addTimer(float timerDuration, std::shared_ptr<Entity> entity);

    /**
     * @brief Updates all timers and dispatches timeout events for expired timers.
     * @param scene The scene containing the timed entities.
     * @param dt The delta time since the previous update.
     * @param scriptSystem The script system used to dispatch timeout events.
     */
    void update(Scene& scene, float dt, ScriptSystem& scriptSystem);
};