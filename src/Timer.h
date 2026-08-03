#pragma once
#include <memory>
#include "Entity.h"

class Timer
{
    public:
    float timeRemaining;
    float timerDuration;
    std::weak_ptr<Entity> entity;
    Timer(float timerDuration, std::shared_ptr<Entity> entity) :  timeRemaining(timerDuration), timerDuration(timerDuration), entity(entity) {}
};


