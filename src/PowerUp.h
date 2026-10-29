#pragma once

#include "Component.h"
#include "Random.h"

/**
 * @enum class PowerupType
 * @brief Enum representing different types of power-ups that can be applied to entities in the game.
 */
enum class PowerUpType {
    Shotgun,
    VelocityBoost,
    MoveSpeed
};

/**
 * @enum class SelectionMethod
 * @brief Enum representing different methods for selecting power-ups.
 */
enum class SelectionMethod {
    Random,
    Fixed
};

/**
 * @class PowerUp
 * @brief PowerUp component class that inherits from Component.
 * @details This class represents the power-up properties of an entity in a game. It contains various properties related to power-ups.
 */
class PowerUp : public Component {
    public:
    float velocityBoost = 100.f;
    float moveSpeedBoost = 100.f;
    float powerUpDuration = 5.f;
    float powerUpTimeLeft = 0.0f;
    bool activated = false;
    PowerUpType powerUpType = PowerUpType::Shotgun;
    SelectionMethod selectionMethod = SelectionMethod::Random;
    /**
     * @brief Rolls a random power-up type from the available options.
     * @return A randomly selected PowerUpType.
     */
    PowerUpType rollPowerUpType() {
        return static_cast<PowerUpType>(Random::range(0, 2));
    }
    /**
     * @brief Returns the name of the power-up type as a string.
     * @return A string representing the name of the power-up type.
     */
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
    /**
     * @brief Removes the effects of the power-up from the specified entity and destroys the parent entity.
     * @param entity The entity from which the power-up effects will be removed.
     */
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