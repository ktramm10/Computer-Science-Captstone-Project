#pragma once
#include <vector>
#include "Scene.h"
#include "ScriptSystem.h"
#include "Timer.h"

class TimerManager {
    public:
    static std::vector<Timer> timers;

    static void addTimer(float timerDuration, std::shared_ptr<Entity> entity);
    void update(Scene& scene, float dt, ScriptSystem& scriptSystem);
};