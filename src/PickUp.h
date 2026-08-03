#pragma once
#include "Component.h"
#include "Interactable.h"


enum class PickUpType {
    Coin,
    Health,
    Ammo
};

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