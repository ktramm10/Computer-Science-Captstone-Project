#pragma once
#include "Component.h"
#include "Entity.h"
#include <memory>

class LifeTime : public Component {
    public:
    float lifeTimeDuration = 0.0f;
    float lifeTimeTimer = 0.0f;
    LifeTime() = default;
    LifeTime(float lifeTimeDuration) : lifeTimeDuration(lifeTimeDuration) {} 
    
    void startTimer() {
        lifeTimeTimer = lifeTimeDuration;
    }
    void timerElapse() {
        if (auto entity = this->getParent()) {
            entity->destroy();
        }
    }
};