#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>


/**
 * @class ComponentTypes
 * @brief Class that defines the different component types in the entity-component system.
 * @details This class provides an enumeration of component types, allowing for the combination of multiple component types into a single bitmask.
 */
class ComponentTypes
{
public:
    enum Value : uint32_t
    {
        None = 0u,
        Transform = 1u << 0,
        Sprite = 1u << 1,
        Physics = 1u << 2,
        Collision = 1u << 3,
        Input = 1u << 4,
        Health = 1u << 5,
        PickUp = 1u << 6,
        ContactDamage = 1u << 7,
        AI = 1u << 8,
        Shooter = 1u << 9,
        LifeTime = 1u << 10,
        Spawner = 1u << 11,
        Door = 1u << 12,
        Key = 1u << 13,
        Script = 1u << 14,
        Text = 1u << 15,
        Switch = 1u << 16,
        PowerUp = 1u << 17,
        PopText = 1u << 18,
        Loot = 1u << 19,
        Sound = 1u << 20
    };
    



    static constexpr uint32_t combine(std::initializer_list<Value> values)
    {
        uint32_t mask = 0u;
        for (const auto value : values)
        {
            mask |= static_cast<uint32_t>(value);
        }
        return mask;
    }
};
static const std::string componentNames[] = {
    "Transform",
    "Sprite",
    "Physics",
    "Collision",
    "Input",
    "Health",
    "Pickup",
    "ContactDamage",
    "AI",
    "Shooter",
    "LifeTime",
    "Spawner",
    "Door",
    "Key",
    "Script",
    "Text",
    "Switch",
    "PowerUp",
    "PopText",
    "Loot",
    "Sound"
};

static const ComponentTypes::Value allComponentTypes[] = {
    ComponentTypes::Transform,
    ComponentTypes::Sprite,
    ComponentTypes::Physics,
    ComponentTypes::Collision,
    ComponentTypes::Input,
    ComponentTypes::Health,
    ComponentTypes::PickUp,
    ComponentTypes::ContactDamage,
    ComponentTypes::AI,
    ComponentTypes::Shooter,
    ComponentTypes::LifeTime,
    ComponentTypes::Spawner,
    ComponentTypes::Door,
    ComponentTypes::Key,
    ComponentTypes::Script,
    ComponentTypes::Text,
    ComponentTypes::Switch,
    ComponentTypes::PowerUp,
    ComponentTypes::PopText,
    ComponentTypes::Loot,
    ComponentTypes::Sound
};


