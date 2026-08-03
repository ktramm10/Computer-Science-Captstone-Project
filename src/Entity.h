#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <cstdint>
#include <string>
#include "Component.h"

class Entity : public std::enable_shared_from_this<Entity>
{
    private:
    unsigned int id;
    std::string name;
    bool alive = true;
    bool isPlayer = false;
    bool blueprint = false;
    std::shared_ptr<Entity> parentEntity = nullptr;
    std::vector<std::shared_ptr<Entity>> childEntities;


    // bitmask to determine component existence
    uint32_t componentMask = 0u;  

    // storage for component data pool
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

    public:
    Entity(unsigned int id, const std::string& name);

    unsigned int getID() const;
    const std::string& getName() const;
    void setName(const std::string& newName) { name = newName; }

    template <typename T>
    void addComponent(std::shared_ptr<T> component, uint32_t flag) {
        std::static_pointer_cast<Component>(component)->setParent(shared_from_this());
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
    
    std::shared_ptr<Component> getComponentByType(ComponentTypes::Value type);


    template <typename T>
    void removeComponent(uint32_t flag) {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            it->second->setParent(nullptr);
            components.erase(it);
        }
        componentMask &= ~flag;
    }

    template<typename T>
    bool hasComponent(uint32_t flag) const {
        return (componentMask & flag) != 0;
    }

    uint32_t getComponentMask() const;

    bool isAlive() const { return alive; }
    bool isPlayerEntity() const { return isPlayer; }
    bool isBlueprint() const { return blueprint; }
    void setBlueprint(bool isBlueprint) { blueprint = isBlueprint; }
    void destroy() { 
        
        alive = false; }
};

