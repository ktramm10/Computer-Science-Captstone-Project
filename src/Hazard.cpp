#include "Hazard.h"
#include "Scene.h"
#include "ScriptSystem.h"
#include <iostream>
const static float ControlLossOnDamage = .1f;
const static float HitFlashDuration = .15f;

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