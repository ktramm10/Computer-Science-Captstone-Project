#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cstdint>
#include <string>

#include "Component.h"

class Entity
{
    private:
    unsigned int id;
    std::string name;
    bool alive = true;
    bool isPlayer = false;

    // bitmask to determine component existence
    uint32_t componentMask = 0u;  

    // storage for component data pool
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

    public:
    Entity(unsigned int id, const std::string& name);

    unsigned int getID() const;
    const std::string& getName() const;

    template <typename T>
    void addComponent(std::shared_ptr<T> component, uint32_t flag) {
        components[typeid(T)] = component;
        componentMask |= flag;
    }

    template <typename T>
    std::shared_ptr<T> getComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    template<typename T>
    bool hasComponent(uint32_t flag) const {
        return (componentMask & flag) != 0;
    }

    uint32_t getComponentMask() const;

    bool isAlive() const { return alive; }
    bool isPlayerEntity() const { return isPlayer; }
    void destroy() { alive = false; }
};

