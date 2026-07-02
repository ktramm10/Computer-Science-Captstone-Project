#pragma once

#include <cstdint>
#include <initializer_list>

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
        Interactable = 1u << 6
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
