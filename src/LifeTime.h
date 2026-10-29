#pragma once
#include "Component.h"
#include "Entity.h"
#include <memory>

/**
 * @class LifeTime
 * @brief LifeTime component class that inherits from Component.
 * @details This class represents the lifetime properties of an entity in a game. It contains various properties related to the lifetime of an entity,
 *  including the duration of the lifetime and a timer to track the remaining time. When the
 */
class LifeTime : public Component {
    public:
    float lifeTimeDuration = 0.0f;
    float lifeTimeTimer = 0.0f;
    LifeTime() = default;
    LifeTime(float lifeTimeDuration) : lifeTimeDuration(lifeTimeDuration) {} 
    
    /**
     * @brief Starts the lifetime timer for the entity, initializing it to the specified duration.
     */
    void startTimer() {
        lifeTimeTimer = lifeTimeDuration;
    }
    /**
     * @brief Handles the elapse of the lifetime timer, destroying the parent entity when the timer reaches zero.
     */
    void timerElapse() {
        if (auto entity = this->getParent()) {
            entity->destroy();
        }
    }
};