#pragma once

#include "Component.h"
#include "Random.h"


enum class PowerUpType {
    Shotgun,
    VelocityBoost,
    MoveSpeed
};

enum class SelectionMethod {
    Random,
    Fixed
};

class PowerUp : public Component {
    public:
    float velocityBoost = 100.f;
    float moveSpeedBoost = 100.f;
    float powerUpDuration = 5.f;
    float powerUpTimeLeft = 0.0f;
    bool activated = false;
    PowerUpType powerUpType = PowerUpType::Shotgun;
    SelectionMethod selectionMethod = SelectionMethod::Random;
    PowerUpType rollPowerUpType() {
        return static_cast<PowerUpType>(Random::range(0, 2));
    }
    std::string getPowerUpName() const {
        switch (powerUpType) {
            case PowerUpType::Shotgun:
                return "Shotgun";
            case PowerUpType::VelocityBoost:
                return "Velocity Boost";
            case PowerUpType::MoveSpeed:
                return "Move Speed";
            default:
                return "Unknown";
        }
    }
    void RemovePowerUp(std::shared_ptr<Entity> entity) {
        auto input = entity ? entity->getComponent<Input>() : nullptr;
        auto shooter = entity ? entity->getComponent<Shooter>() : nullptr;
        if (input && shooter) {
            switch(powerUpType) {
                case PowerUpType::MoveSpeed:
                    input->moveSpeed -= moveSpeedBoost;
                    input->acceleration -= moveSpeedBoost;
                    input->deceleration -= moveSpeedBoost;
                    break;
                case PowerUpType::VelocityBoost:
                    shooter->projectileSpeed -= velocityBoost;
                    break;
                case PowerUpType::Shotgun:
                    shooter->gunMode = GunMode::default;
                    break;
            }
        }
        if (auto parent = this->getParent()) {
            parent->destroy();
        }
    }
    PowerUp() = default;
};