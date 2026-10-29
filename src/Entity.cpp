#include "Entity.h"

/**
 * @brief Constructs an Entity with the specified ID and name.
 * @param id The unique identifier of the entity.
 * @param name The name of the entity.
 */
Entity::Entity(unsigned int id, const std::string& name) : id(id), name(name) {}
/**
 * @brief Retrieves the unique identifier of the entity.
 * @return The unique identifier of the entity as an unsigned integer.
 */
unsigned int Entity::getID() const {
    return id;
}
/**
 * @brief Retrieves the name of the entity.
 * @return The name of the entity as a constant reference to a std::string.
 */
const std::string& Entity::getName() const {
    return name;
}
/**
 * @brief Retrieves the component mask of the entity, indicating which components are present.
 * @return The component mask as a 32-bit unsigned integer.
 */
uint32_t Entity::getComponentMask() const {
    return componentMask;
}
