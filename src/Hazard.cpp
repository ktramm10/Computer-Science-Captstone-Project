#include "Hazard.h"
#include "Scene.h"
#include "ScriptSystem.h"
#include <iostream>
/**
 * @brief Constant representing the duration of control loss when an entity takes damage.
 */
const static float ControlLossOnDamage = .1f;
/**
 * @brief Constant representing the duration of the hit flash effect when an entity takes damage.
 */
const static float HitFlashDuration = .15f;

/**
 * @brief Applies the effects of a hazard to another entity, including damage, knockback, and movement lock.
 * @param otherEntity The entity that is affected by the hazard.
 */
void Hazard::applyHazard(std::shared_ptr<Entity> otherEntity, std::shared_ptr<Entity> hazardEntity, Scene &scene, ScriptSystem &scriptSystem, float dt)
{
    auto& contactDamage = hazardEntity->getComponent<ContactDamage>();
    if (contactDamage) {
        auto& input = otherEntity->getComponent<Input>();
        auto& health = otherEntity->getComponent<Health>();
        if (health) {
            // damage function returns false if entity is killed and true if still alive
            if (!health->damage(contactDamage->damage)) {
                if (otherEntity == scene.getPlayerEntity()) {
                    scriptSystem.OnPlayerDeath(scene, dt);
                }
                otherEntity->destroy();
            } 
        }
        auto& collider = otherEntity->getComponent<Collision>();
        if (collider) {
            collider->knockbackDistance = contactDamage->knockback;
        }
        // when damage is taken lose control of character for a short period of time.
        if (input && input->movementLockTimer <= 0) {
            input->lockMovement(.1);
        }
        
        if (contactDamage->destroyEntityOnContact) {
            hazardEntity->destroy();
        }
    }
}