#pragma once
#include "Component.h"
#include "Interactable.h"

class Collectable : public Component {
    public:
    float amount;
    CollectableType collectableType;
    Collectable(CollectableType collectableType, float amount) : collectableType(collectableType), amount(amount) {}

};