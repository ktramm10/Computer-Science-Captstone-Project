#include "Entity.h"
#include "Transform.h"
#include "Sprite.h"
#include "Physics.h"
#include "Collision.h"
#include "Input.h"
#include "Health.h"
#include "PickUp.h"
#include "ContactDamage.h"
#include "AI.h"
#include "Shooter.h"
#include "LifeTime.h"
#include "Spawner.h"
#include "Door.h"
#include "Key.h"
#include "Script.h"
#include "Text.h"
#include "Switch.h"
#include "PowerUp.h"
#include "PopText.h"
#include "Loot.h"

std::shared_ptr<Component> Entity::getComponentByType(ComponentTypes::Value type)
{
    switch (type) {
    case ComponentTypes::Transform:
        return getComponent<Transform>();
    case ComponentTypes::Sprite:
        return getComponent<Sprite>();
    case ComponentTypes::Physics:
        return getComponent<Physics>();
    case ComponentTypes::Collision:
        return getComponent<Collision>();
    case ComponentTypes::Input:
        return getComponent<Input>();
    case ComponentTypes::Health:
        return getComponent<Health>();
    case ComponentTypes::PickUp:
        return getComponent<PickUp>();
    case ComponentTypes::ContactDamage:
        return getComponent<ContactDamage>();
    case ComponentTypes::AI:
        return getComponent<AI>();
    case ComponentTypes::Shooter:
        return getComponent<Shooter>();
    case ComponentTypes::LifeTime:
        return getComponent<LifeTime>();
    case ComponentTypes::Spawner:
        return getComponent<Spawner>();
    case ComponentTypes::Door:
        return getComponent<Door>();
    case ComponentTypes::Key:
        return getComponent<Key>();
    case ComponentTypes::Script:
        return getComponent<Script>();
    case ComponentTypes::Text:
        return getComponent<Text>();
    case ComponentTypes::Switch:
        return getComponent<Switch>();
    case ComponentTypes::PowerUp:
        return getComponent<PowerUp>();
    case ComponentTypes::PopText:
        return getComponent<PopText>();
    case ComponentTypes::Loot:
        return getComponent<Loot>();
    default:
        return nullptr;
    }
}
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