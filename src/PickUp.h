#pragma once
#include "Component.h"
#include "Interactable.h"

/**
 * @enum class PickUpType
 * @brief Enum representing different types of pickups that can be collected by entities in the game.
 */
enum class PickUpType {
    Coin,
    Health,
    Ammo
};

/**
 * @class PickUp
 * @brief PickUp component class that inherits from Component.
 * @details This class represents the pickup properties of an entity in a game. It contains various properties related to pickups.
 */
class PickUp : public Component {
    public:
    float amount;
    PickUpType pickUpType;
    PickUp() {
        pickUpType = PickUpType::Coin;
        amount = 10;
    }
    PickUp(PickUpType pickUpType, float amount) : pickUpType(pickUpType), amount(amount) {}

};