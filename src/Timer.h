#pragma once
#include <memory>
#include "Entity.h"

/**
 * @class Timer
 * @brief Stores countdown data for an entity-bound script timer.
 */
class Timer
{
    public:
    float timeRemaining;
    float timerDuration;
    std::weak_ptr<Entity> entity;

    /**
     * @brief Constructs a timer for an entity.
     * @param timerDuration The duration of the timer in seconds.
     * @param entity The entity associated with the timer.
     */
    Timer(float timerDuration, std::shared_ptr<Entity> entity) :  timeRemaining(timerDuration), timerDuration(timerDuration), entity(entity) {}
};


