#include "Entity.h"

Entity::Entity(unsigned int id, const std::string& name) : id(id), name(name) {}

unsigned int Entity::getID() const {
    return id;
}

const std::string& Entity::getName() const {
    return name;
}

uint32_t Entity::getComponentMask() const {
    return componentMask;
}
