#include "InputSystem.h"

#include <cmath>
#include <iostream>
#include "Random.h"
#include "Engine.h"
#include "StringHelper.h"
#include <iostream>

/** @brief Namespace for input system implementation details. */
namespace {

    int projectileSuffix = 1;
    bool wasFirePressed = false;
    /**
     * @brief Retrieves the string representation of a linked game value for UI text components.
     * @param scene The scene containing the entities and game state.
     * @param gameValue The specific game value to retrieve (e.g., PlayerHealth, Coins, Score, Experience).
     * @return A string representing the current value of the specified game value.
     */
    std::string getLinkedGameValue(Scene& scene, GameValue gameValue) {
        switch (gameValue) {
            case GameValue::PlayerHealth:
                if (auto player = scene.getPlayerEntity()) {
                    if (auto health = player->getComponent<Health>()) {
                        return StringHelper::formatFloat(health->currentHealth, 2);
                    }
                }
                return "";
            case GameValue::Coins:
                return std::to_string(scene.getGameState().coins);
            case GameValue::Score:
                return std::to_string(scene.getGameState().score);
            case GameValue::Experience:
                return StringHelper::formatFloat(scene.getGameState().experience, 2);
            default:
                return "";
        }
    }
    /**
     * @brief Updates the UI text components in the scene based on their linked game values.
     * @param scene The scene containing the entities and UI text components.
     */
    void handleUITextUpdates(Scene& scene) {
        for (const auto& entity : scene.getEntities()) {
            if (!entity || !entity->isAlive() || entity->isBlueprint()) {
                continue;
            }
            auto textComponent = entity->getComponent<Text>();
            if (textComponent && textComponent->linkGameValue) {
                textComponent->concatGameValue(getLinkedGameValue(scene, textComponent->linkedGameValue));
            }
        }
    }
    /**
     * @brief Moves a current value towards a target value by a specified maximum delta, ensuring it does not overshoot the target.
     * @param current The current value to be moved.
     * @param target The target value to move towards.
     * @param maxDelta The maximum amount to change the current value in one step.
     * @return The new value after moving towards the target.
     */
    float moveToward(float current, float target, float maxDelta) {
        if (current < target) {
            return std::min(current + maxDelta, target);
        }

        if (current > target) {
            return std::max(current - maxDelta, target);
        }

        return target;
    }
    /**
     * @brief Creates and spawns a new entity in the scene based on a source entity and a spawner component.
     * @param scene The scene in which the new entity will be created.
     * @param sourceEntity The source entity to copy components from.
     * @param spawner The spawner component that defines the spawning behavior and properties.
     * @param resourceManager The resource manager used to retrieve textures and fonts for the new entity
     * @return A shared pointer to the newly created and spawned entity.
     */
    std::shared_ptr<Entity> createAndSpawnEntity(Scene &scene, std::shared_ptr<Entity> sourceEntity, std::shared_ptr<Spawner>& spawner, ResourceManager& resourceManager)
    {
        auto spawnedEntity = scene.createEntity("spawned " + spawner->baseName + std::to_string(idIndex++));
        if (!sourceEntity) {
            return spawnedEntity;
        }

        auto componentMask = sourceEntity->getComponentMask();

        if ((componentMask & ComponentTypes::Transform) != 0) {
            auto transform = sourceEntity->getComponent<Transform>();
            if (transform) {
                spawnedEntity->addComponent(std::make_shared<Transform>(*transform), ComponentTypes::Transform);
            }
        }
        if ((componentMask & ComponentTypes::Sprite) != 0) {
            auto sprite = sourceEntity->getComponent<Sprite>();
            if (sprite) {
                std::string textureID = resourceManager.hasTexture(sprite->texID) ? sprite->texID : ResourceManager::DefaultTextureID;
                auto spawnedSprite = std::make_shared<Sprite>(resourceManager.getTexture(textureID), textureID);
                if (sprite->sprite) {
                    spawnedSprite->sprite->setTextureRect(sprite->sprite->getTextureRect());
                }
                spawnedEntity->addComponent(spawnedSprite, ComponentTypes::Sprite);
            }
        }
        if ((componentMask & ComponentTypes::Physics) != 0) {
            auto physics = sourceEntity->getComponent<Physics>();
            if (physics) {
                spawnedEntity->addComponent(std::make_shared<Physics>(*physics), ComponentTypes::Physics);
            }
        }
        if ((componentMask & ComponentTypes::Collision) != 0) {
            auto collision = sourceEntity->getComponent<Collision>();
            if (collision) {
                spawnedEntity->addComponent(std::make_shared<Collision>(*collision), ComponentTypes::Collision);
            }
        }
        if ((componentMask & ComponentTypes::Input) != 0) {
            auto input = sourceEntity->getComponent<Input>();
            if (input) {
                spawnedEntity->addComponent(std::make_shared<Input>(*input), ComponentTypes::Input);
            }
        }
        if ((componentMask & ComponentTypes::Health) != 0) {
            auto health = sourceEntity->getComponent<Health>();
            if (health) {
                spawnedEntity->addComponent(std::make_shared<Health>(*health), ComponentTypes::Health);
            }
        }
        if ((componentMask & ComponentTypes::PickUp) != 0) {
            auto pickUp = sourceEntity->getComponent<PickUp>();
            if (pickUp) {
                spawnedEntity->addComponent(std::make_shared<PickUp>(*pickUp), ComponentTypes::PickUp);
            }
        }
        if ((componentMask & ComponentTypes::ContactDamage) != 0) {
            auto contactDamage = sourceEntity->getComponent<ContactDamage>();
            if (contactDamage) {
                spawnedEntity->addComponent(std::make_shared<ContactDamage>(*contactDamage), ComponentTypes::ContactDamage);
            }
        }
        if ((componentMask & ComponentTypes::AI) != 0) {
            auto ai = sourceEntity->getComponent<AI>();
            if (ai) {
                spawnedEntity->addComponent(std::make_shared<AI>(*ai), ComponentTypes::AI);
            }
        }
        if ((componentMask & ComponentTypes::Shooter) != 0) {
            auto shooter = sourceEntity->getComponent<Shooter>();
            if (shooter) {
                spawnedEntity->addComponent(std::make_shared<Shooter>(*shooter), ComponentTypes::Shooter);
            }
        }
        if ((componentMask & ComponentTypes::LifeTime) != 0) {
            auto lifeTime = sourceEntity->getComponent<LifeTime>();
            if (lifeTime) {
                auto spawnedLifeTime = std::make_shared<LifeTime>(*lifeTime);
                spawnedEntity->addComponent(spawnedLifeTime, ComponentTypes::LifeTime);
            }
        }
        if ((componentMask & ComponentTypes::Door) != 0) {
            auto door = sourceEntity->getComponent<Door>();
            if (door) {
                spawnedEntity->addComponent(std::make_shared<Door>(*door), ComponentTypes::Door);
            }
        }
        if ((componentMask & ComponentTypes::Key) != 0) {
            auto key = sourceEntity->getComponent<Key>();
            if (key) {
                spawnedEntity->addComponent(std::make_shared<Key>(*key), ComponentTypes::Key);
            }
        }
        if ((componentMask & ComponentTypes::Script) != 0) {
            auto script = sourceEntity->getComponent<Script>();
            if (script) {
                auto spawnedScript = std::make_shared<Script>(*script);
                spawnedScript->started = false;
                spawnedEntity->addComponent(spawnedScript, ComponentTypes::Script);
            }
        }
        if ((componentMask & ComponentTypes::Text) != 0) {
            auto text = sourceEntity->getComponent<Text>();
            if (text) {
                std::string fontID = resourceManager.hasFont(text->fontID) ? text->fontID : ResourceManager::DefaultFontID;
                auto spawnedText = std::make_shared<Text>(resourceManager.getFont(fontID), fontID);
                spawnedText->style = text->style;
                spawnedText->linkGameValue = text->linkGameValue;
                spawnedText->linkedGameValue = text->linkedGameValue;
                spawnedText->characterSize = text->characterSize;
                spawnedText->textString = text->textString;
                spawnedText->fillColor = text->fillColor;
                spawnedText->applyProperties();
                spawnedEntity->addComponent(spawnedText, ComponentTypes::Text);
            }
        }
        if ((componentMask & ComponentTypes::Switch) != 0) {
            auto switchComponent = sourceEntity->getComponent<Switch>();
            if (switchComponent) {
                spawnedEntity->addComponent(std::make_shared<Switch>(*switchComponent), ComponentTypes::Switch);
            }
        }
        if ((componentMask & ComponentTypes::PowerUp) != 0) {
            auto powerUp = sourceEntity->getComponent<PowerUp>();
            if (powerUp) {
                if (powerUp->selectionMethod == SelectionMethod::Random) {
                    powerUp->powerUpType = powerUp->rollPowerUpType();
                }
                spawnedEntity->addComponent(std::make_shared<PowerUp>(*powerUp), ComponentTypes::PowerUp);
            }
        }
        if ((componentMask & ComponentTypes::PopText) != 0) {
            auto popText = sourceEntity->getComponent<PopText>();
            if (popText) {
                std::string fontID = resourceManager.hasFont(popText->fontID) ? popText->fontID : ResourceManager::DefaultFontID;
                auto spawnedPopText = std::make_shared<PopText>(resourceManager.getFont(fontID), fontID);
                spawnedPopText->offset = popText->offset;
                spawnedPopText->textString = popText->textString;
                spawnedPopText->size = popText->size;
                spawnedPopText->color = popText->color;
                spawnedPopText->style = popText->style;
                spawnedPopText->lifeTimeDuration = popText->lifeTimeDuration;
                spawnedEntity->addComponent(spawnedPopText, ComponentTypes::PopText);
                spawnedPopText->applyProperties(scene.getWorldPosition(spawnedEntity));
            }
        }
        if ((componentMask & ComponentTypes::Loot) != 0) {
            auto loot = sourceEntity->getComponent<Loot>();
            if (loot) {
                spawnedEntity->addComponent(std::make_shared<Loot>(*loot), ComponentTypes::Loot);
            }
        }

        return spawnedEntity;

    }
    /**
     * @brief Generates a new random goal position within the bounds of the game window.
     * @return A 2D vector representing the new goal position.
     */
    sf::Vector2f newGoalPosition()
    {
        float xBound = WINDOW_SIZE.x;
        float yBound = WINDOW_SIZE.y;
        return sf::Vector2f({Random::range(0.f, xBound), Random::range(0.f, yBound)});
    }
    /**
     * @brief Handles the spawning of entities in the scene based on spawner components.
     * @param scene The scene containing the entities and spawner components.
     * @param dt The delta time since the last update, used for timing spawn intervals.
     * @param resourceManager The resource manager used to retrieve textures and fonts for the spawned entities.
     */
    void handleSpawner(Scene& scene, float dt, ResourceManager& resourceManager) {
        auto entities = scene.getEntities();
        for (auto& entity : entities) {
            if (!entity || !entity->isAlive()) {
                continue;
            }
            const auto componentMask = entity->getComponentMask();
            if ((componentMask & ComponentTypes::Spawner) == 0) {
                continue;
            }
            auto spawner = entity->getComponent<Spawner>();
            if (spawner) {
                if (spawner->spawnTimeLeft > 0.f) {
                    spawner->spawnTimeLeft -= dt;
                }
                else {
                    if (spawner->spawnMethod == SpawnMethod::Fixed) {
                        auto spawnedEntity = createAndSpawnEntity(scene, entity, spawner, resourceManager);
                        auto transform = spawnedEntity->getComponent<Transform>();
                        if (transform) {
                            transform->setWorldPosition(spawner->spawnPosition);
                        }
                    }
                    else if (spawner->spawnMethod == SpawnMethod::Random) {
                        auto spawnedEntity = createAndSpawnEntity(scene, entity, spawner, resourceManager);
                        auto transform = spawnedEntity->getComponent<Transform>();
                        if (transform) {
                            transform->setWorldPosition(sf::Vector2f{Random::range(0.f, WINDOW_SIZE.x), Random::range(0.f, WINDOW_SIZE.y)});
                        }
                    }
                    spawner->spawnTimeLeft = spawner->spawnFrequency;
                }
            }
        }
    }
    /**
     * @brief Updates the timers for various components in the scene, such as LifeTime, Switch, and PowerUp components.
     * @param scene The scene containing the entities and their components.
     * @param dt The delta time since the last update, used for timing updates.
     */
    void updateComponentTimers(Scene& scene, float dt) {
        for (auto& entity : scene.getEntities()) {
            if (!entity || !entity->isAlive()) {
                continue;
            }

            // lifeTime component
            const auto componentMask = entity->getComponentMask();
            if ((componentMask & ComponentTypes::LifeTime) != 0) {
                auto lifeTime = entity->getComponent<LifeTime>();
                if (!lifeTime) {
                    continue;
                }

                if (lifeTime->lifeTimeTimer <= 0.0f) {
                    lifeTime->startTimer();
                }

                lifeTime->lifeTimeTimer -= dt;
                if (lifeTime->lifeTimeTimer <= 0.0f) {
                    lifeTime->timerElapse();
                }
            }

            
            // switch component
            if ((componentMask & ComponentTypes::Switch) != 0) {
                auto switchComponent = entity->getComponent<Switch>();
                switchComponent->lockout -= dt;
            }

            // PowerUp Component
            if ((componentMask & ComponentTypes::PowerUp) != 0) {
                auto powerUp = entity->getComponent<PowerUp>();
                if (!powerUp->activated) {
                    continue;
                }
                if (powerUp->powerUpTimeLeft <= 0.f) {
                    powerUp->RemovePowerUp(scene.getPlayerEntity());
                }
                else if (powerUp->powerUpTimeLeft > 0.f) {
                    powerUp->powerUpTimeLeft -= dt;
                }
            }

        }
    }

    /**
     * @brief Processes the input for a top-down player character.
     * @param input The input component containing the player's input state.
     * @param physics The physics component containing the player's physical properties.
     * @param dt The delta time since the last update, used for timing updates.
     */
    void processTopDownPlayerInput(std::shared_ptr<Input> input, std::shared_ptr<Physics> physics, float dt) {

        sf::Vector2f moveDirection{0.0f, 0.0f};
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)){
            moveDirection.x -= 1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)){
            moveDirection.x += 1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)){
            moveDirection.y -= 1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)){
            moveDirection.y += 1.0f;
        }
        if (moveDirection.lengthSquared() > 0.0f) {
            moveDirection = moveDirection.normalized();
        }
        if (physics->useAcceleration) {
            sf::Vector2f targetVelocity = moveDirection * input->moveSpeed;
            float maxDelta = input->acceleration * dt;
            if (moveDirection.lengthSquared() == 0.0f) {
                maxDelta = input->deceleration * dt;
            }
            physics->velocity.x = moveToward(physics->velocity.x, targetVelocity.x, maxDelta);
            physics->velocity.y = moveToward(physics->velocity.y, targetVelocity.y, maxDelta);
        }
        else {
        physics->velocity = moveDirection * input->moveSpeed;
        }
    }

    /**
     * @brief Processes the input for a platformer player character.
     * @param input The input component containing the player's input state.
     * @param physics The physics component containing the player's physical properties.
     * @param scene The scene containing the game entities and their interactions.
     */
    void processPlatformerPlayerInput(std::shared_ptr<Input> input, std::shared_ptr<Physics> physics, Scene& scene, float dt) {
        float moveDirectionX = 0.0f;
        if (input->canMove) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)){
                moveDirectionX -= 1.0f;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)){
                moveDirectionX += 1.0f;
            }
        }

        if (physics->useAcceleration) {
            float targetVelocityX = moveDirectionX * input->moveSpeed;
            float maxDelta = moveDirectionX == 0.0f ? input->deceleration * dt : input->acceleration * dt;
            physics->velocity.x = moveToward(physics->velocity.x, targetVelocityX, maxDelta);
        }
        else {
            physics->velocity.x = moveDirectionX * input->moveSpeed;
        }

        if (input->canMove && input->canJump && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))){
            if (!physics->isGrounded) {
                return;
            }
            auto entity = input->getParent();
            auto parent = entity->getParentEntity().lock();
            if (entity && parent) {
                scene.clearParent(entity->getID());
                physics->useGravity = true;
            }
            physics->velocity.y = -input->jumpForce;
            physics->isGrounded = false;
        }
    }
    /**
     * @brief Handles the movement lock for an input component, updating the canMove flag based on the movement lock timer.
     * @param input The input component to handle movement lock for.
     * @param dt The delta time since the last update, used for timing updates.
     */
    void handleMovementLock(std::shared_ptr<Input> input, float dt) {
        // Handle movement lock and update canMove flag.
        if (input->movementLockTimer > 0.0f) {
            input->movementLockTimer -= dt;
            input->canMove = input->movementLockTimer <= 0.0f;
        }
    }
    /**
     * @brief Calculates the direction in which a player entity should shoot a projectile based on the mouse position and the entity's position in the scene.
     * @param entity The player entity that is shooting the projectile.
     * @param scene The scene containing the player entity and other game entities.
     * @param window The render window used to map pixel coordinates to world coordinates.
     * @return A normalized 2D vector representing the direction in which the projectile should be.
     */
    sf::Vector2f CalculatePlayerShootDirection(std::shared_ptr<Entity> entity, const Scene& scene, sf::RenderWindow& window ) {
        auto shootertransform = entity->getComponent<Transform>();
        if (shootertransform) {
            sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            sf::Vector2f shooterToMouse = mousePosition - scene.getWorldPosition(entity);
            sf::Vector2f projectileDirection;
            if (shooterToMouse.lengthSquared() > 0.f) {
                projectileDirection = shooterToMouse.normalized();
                return projectileDirection;
            }
            return sf::Vector2f{0.f, 0.f};
        }
        return sf::Vector2f{0.f, 0.f};
    }
    /**
     * @brief Creates a projectile entity in the scene with specified properties and components.
     * @param projectileDirection The direction in which the projectile will move.
     * @param scene The scene in which the projectile will be created.
     * @param resourceManager The resource manager used to retrieve textures for the projectile.
     * @param shooterEntity The entity that is shooting the projectile, used to determine properties like damage and speed.
     * @param window The render window used for coordinate mapping (not used in this function but included for consistency).
     */
    void createProjectile(sf::Vector2f projectileDirection, Scene& scene, ResourceManager& resourceManager,
        std::shared_ptr<Entity> shooterEntity, sf::RenderWindow& window) {
        auto projectile = scene.createEntity("projectile" + std::to_string(projectileSuffix));
        ++projectileSuffix;
        // initialize components
        auto projectileTransform = std::make_shared<Transform>();
        auto contactDamage = std::make_shared<ContactDamage>();
        auto collision = std::make_shared<Collision>();
        auto projectilePhysics = std::make_shared<Physics>();
        auto projectileLifeTime = std::make_shared<LifeTime>(4.f);
        auto shooter = shooterEntity->getComponent<Shooter>();
        std::string projectileTextureID = resourceManager.hasTexture(shooter->textureID) ? shooter->textureID : ResourceManager::DefaultTextureID;
        auto projectileSprite = std::make_shared<Sprite>(resourceManager.getTexture(projectileTextureID), projectileTextureID);
        // edit components
        shooter->collisionScaling = {32.f * shooter->projectileScale.x, 32.f * shooter->projectileScale.y};
        auto shooterTransform = shooterEntity->getComponent<Transform>();
        auto shooterCollider = shooterEntity->getComponent<Collision>();
        projectileTransform->setWorldPosition(scene.getWorldPosition(shooterEntity) + projectileDirection * 40.0f);
        projectileTransform->setScale(shooter->projectileScale);
        collision->size = shooter->collisionScaling;
        collision->offset = shooter->offset;
        collision->isPhysicalCollider = false;
        collision->faction = shooterCollider->faction;
        contactDamage->damage = shooter->projectileDamage;
        contactDamage->destroyEntityOnContact = true;
        projectilePhysics->useGravity = false;
        projectilePhysics->velocity = projectileDirection * shooter->projectileSpeed;
        
        // link components to entity
        projectile->addComponent(projectileTransform, ComponentTypes::Transform);
        projectile->addComponent(projectileSprite, ComponentTypes::Sprite);
        projectile->addComponent(projectilePhysics, ComponentTypes::Physics);
        projectile->addComponent(collision, ComponentTypes::Collision);
        projectile->addComponent(contactDamage, ComponentTypes::ContactDamage);
        projectile->addComponent(projectileLifeTime, ComponentTypes::LifeTime);
    }
    /**
     * @brief Handles the shooting action for a player entity, creating projectiles based on the gun mode and input state.
     * @param entity The player entity that is shooting.
     * @param scene The scene in which the player entity exists and where projectiles will be created.
     * @param resourceManager The resource manager used to retrieve textures for the projectiles.
     * @param window The render window used for coordinate mapping (not used in this function but included for consistency).
     * @param firePressed A boolean indicating whether the fire button is currently pressed.
     */
    void HandlePlayerShoot(std::shared_ptr<Entity> entity, Scene& scene, ResourceManager& resourceManager, sf::RenderWindow& window, const bool firePressed, AudioSystem& audioSystem) {
        
        const bool fireRequested = firePressed && !wasFirePressed; 
        if (fireRequested) {
            audioSystem.playSound(resourceManager.getSoundBuffer("shoot_sfx"));
            auto shooter = entity->getComponent<Shooter>();
            auto shooterTransform = entity->getComponent<Transform>();
            auto shooterCollider = entity->getComponent<Collision>();
            bool requiredComponentsValid = shooter && shooterTransform && shooterCollider;
            if (requiredComponentsValid) {
                switch(shooter->gunMode) {
                    case GunMode::default: {
                        createProjectile(CalculatePlayerShootDirection(entity, scene, window), scene, resourceManager, entity, window);
                        break;
                    }
                    case GunMode::Shotgun: {
                        // create multiple projectiles for shotgun spread
                        for (int i = 0; i < shooter->shotGunPelletCount; ++i) {
                            createProjectile(CalculatePlayerShootDirection(entity, scene, window).rotatedBy(sf::degrees(10.f * (i - (shooter->shotGunPelletCount - 1) / 2.f))), scene, resourceManager, entity, window);
                        }
                        break;
                    }
                }
            }
        }
    }
    /**
     * @brief Parses player input for all entities in the scene, handling movement and shooting actions based on input components.
     * @param scene The scene containing the entities and their components.
     * @param dt The delta time since the last update, used for timing updates.
     * @param window The render window used for coordinate mapping
     * @param resourceManager The resource manager used to retrieve textures for projectiles and other resources.
     * @param audioSystem The system that handles playing sound effects.
     */
    void parsePlayerInput(Scene &scene, float dt, sf::RenderWindow& window, ResourceManager& resourceManager, AudioSystem& audioSystem)
    {
        const bool firePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        auto entities = scene.getEntities();
        for (auto& entity : entities) {
            if (!entity || !entity->isAlive() || entity->isBlueprint()) {
                    continue;
                }

            const auto componentMask = entity->getComponentMask();
            if ((componentMask & ComponentTypes::Input) != 0 && (componentMask & ComponentTypes::Physics) != 0) {
                auto input = entity->getComponent<Input>();
                auto physics = entity->getComponent<Physics>();

                
                switch(input->movementType) {
                    case MovementType::TopDown:
                        if (!physics->useAcceleration) {
                            physics->velocity *= 0.f;
                        }
                        processTopDownPlayerInput(input, physics, dt);
                        break;
                    case MovementType::Platformer:
                    default:
                        handleMovementLock(input, dt);
                        processPlatformerPlayerInput(input, physics, scene, dt);
                        break;
                    }

                HandlePlayerShoot(entity, scene, resourceManager, window, firePressed, audioSystem);
            }
            wasFirePressed = firePressed;
        }
    }      

        
    /**
     * @brief Calculates the direction in which an AI entity should shoot a projectile towards the player entity, with an optional error margin for inaccuracy.
     * @param scene The scene containing the AI and player entities.
     * @param playerEntity The player entity that the AI is targeting.
     * @param self The AI entity that is shooting the projectile.
     * @param errorMargin The maximum angle in degrees by which the AI's shot can deviate from the direct line to the player, simulating inaccuracy.
     * @return A normalized 2D vector representing the direction in which the AI should shoot
     */
    sf::Vector2f calculateAIShootDirection(const Scene& scene, std::shared_ptr<Entity> playerEntity, std::shared_ptr<Entity> self, float errorMargin) {
        auto playerTransform = playerEntity->getComponent<Transform>();
        auto selfTransform = self->getComponent<Transform>();
        bool validTransforms = selfTransform && playerTransform;
        if (validTransforms) {
            sf::Vector2f AItoPlayer = scene.getWorldPosition(playerEntity) - scene.getWorldPosition(self);
            sf::Vector2f AItoPlayerDirection = AItoPlayer.normalized();
            // offset by random float within error margin
            float offsetDegrees = Random::range(0.f, 15.f);
            int posOrNeg = Random::range(-1, 1);
            if (posOrNeg <= 0) {
                offsetDegrees *= -1;
            }
            float radians = offsetDegrees * (3.14159/180);
            float xPrime = AItoPlayerDirection.x * cosf(radians) - AItoPlayerDirection.y * sinf(radians);
            float yPrime = AItoPlayerDirection.x * sinf(radians) + AItoPlayerDirection.y * cosf(radians);
            return {xPrime, yPrime};
        }
        return {0.f,0.f};

    }
    /**
     * @brief Handles the shooting action for an AI entity, creating projectiles towards the player entity based on the AI's shooting frequency and error margin.
     * @param entity The AI entity that is shooting.
     * @param scene The scene in which the AI entity exists and where projectiles will be created.
     * @param ai The AI component containing the shooting frequency and error margin for the AI's shots.
     * @param resourceManager The resource manager used to retrieve textures for the projectiles.
     * @param dt The delta time since the last update, used for timing updates.
     */
    void HandleAIShoot(std::shared_ptr<Entity> entity, Scene& scene, std::shared_ptr<AI> ai, ResourceManager& resourceManager, float dt, AudioSystem& audioSystem) {
        auto AIshooter = entity->getComponent<Shooter>();
        auto playerEntity = scene.getPlayerEntity();
        if (AIshooter && playerEntity){
            if (ai->shootTimeLeft <= 0.0f) {
                audioSystem.playSound(resourceManager.getSoundBuffer("shoot_sfx"));
                // shoot implementation
                auto shooter = entity->getComponent<Shooter>();
                auto shooterTransform = entity->getComponent<Transform>();
                auto shooterCollider = entity->getComponent<Collision>();
                if (shooter && shooterTransform && shooterCollider) {
                    auto projectile = scene.createEntity("projectile" + std::to_string(projectileSuffix));
                    projectileSuffix += 1;
                    auto projectileTransform = std::make_shared<Transform>();
                    auto contactDamage = std::make_shared<ContactDamage>();
                    auto collision = std::make_shared<Collision>();
                    auto projectilePhysics = std::make_shared<Physics>();
                    auto projectileLifeTime = std::make_shared<LifeTime>(4.f);

                    std::string projectileTextureID = resourceManager.hasTexture(shooter->textureID) ? shooter->textureID : ResourceManager::DefaultTextureID;
                    auto projectileSprite = std::make_shared<Sprite>(resourceManager.getTexture(projectileTextureID), projectileTextureID);
                    sf::Vector2f projectileDirection = calculateAIShootDirection(scene, playerEntity, entity, ai->errorMargin);
                    shooter->collisionScaling = {32.f * shooter->projectileScale.x, 32.f * shooter->projectileScale.y};
                    projectileTransform->setWorldPosition(scene.getWorldPosition(entity) + projectileDirection * 40.0f);
                    projectileTransform->setScale(shooter->projectileScale);
                    collision->size = shooter->collisionScaling;
                    collision->offset = shooter->offset;
                    collision->isPhysicalCollider = false;
                    collision->faction = shooterCollider->faction;
                    contactDamage->damage = shooter->projectileDamage;
                    contactDamage->destroyEntityOnContact = true;

                    projectilePhysics->useGravity = false;
                    projectilePhysics->velocity = projectileDirection * shooter->projectileSpeed;

                    projectile->addComponent(projectileTransform, ComponentTypes::Transform);
                    projectile->addComponent(projectileSprite, ComponentTypes::Sprite);
                    projectile->addComponent(projectilePhysics, ComponentTypes::Physics);
                    projectile->addComponent(collision, ComponentTypes::Collision);
                    projectile->addComponent(contactDamage, ComponentTypes::ContactDamage);
                    projectile->addComponent(projectileLifeTime, ComponentTypes::LifeTime);
                }

                // reset timer
                ai->shootTimeLeft = ai->shootFrequency;
            }
            else {
                ai->shootTimeLeft -= dt;
            }
        }
    }
    /**
     * @brief Updates the behavior of an AI entity in a top-down game, including movement towards a goal position and shooting at the player.
     * @param ai The AI component containing the AI's properties and state.
     * @param transform The transform component of the AI entity, used for position and scale.
     * @param physics The physics component of the AI entity, used for velocity and movement.
     * @param entity The AI entity being updated.
     * @param scene The scene containing the AI entity and other game entities.
     * @param dt The delta time since the last update, used for timing updates.
     * @param resourceManager The resource manager used to retrieve textures for projectiles and other resources
     */
    void topDownAI(std::shared_ptr<AI> ai, std::shared_ptr<Transform> transform, std::shared_ptr<Physics> physics, std::shared_ptr<Entity> entity, Scene& scene, float dt, ResourceManager& resourceManager, AudioSystem& audioSystem) {
        sf::Vector2f aiPosition = scene.getWorldPosition(entity);
        
        HandleAIShoot(entity, scene, ai, resourceManager, dt, audioSystem);
        // if goal_position is out of bound that means that its the 
        if (ai->goalPosition.x < 0 || ai->goalPosition.y < 0) {
            ai->goalPosition = newGoalPosition();
        }
        // check if goal location has been reach (roughly)
        float allowance = 160.f;
        sf::Vector2f toGoal = ai->goalPosition - aiPosition;
        if (toGoal.lengthSquared() <= allowance * allowance || ai->decelerating) {
            if (physics->velocity == sf::Vector2f{0.0f, 0.0f}){
                // goal is reached so generate a new goal position
                ai->decelerating = false;
                ai->goalPosition = newGoalPosition();
                toGoal = ai->goalPosition - aiPosition;
            }
            else {
                ai->decelerating = true;
                toGoal = {0.0f, 0.0f};
            }
        }
        if (physics->useAcceleration) {
            sf::Vector2f targetVelocity{0.0f, 0.0f};
            if (toGoal.lengthSquared() > 0.0f) {
                targetVelocity = toGoal.normalized() * ai->moveSpeed;
            }
            float maxDelta = ai->acceleration * dt;
            if (toGoal.lengthSquared() == 0.0f) {
                maxDelta = ai->deceleration * dt;
            }
            physics->velocity.x = moveToward(physics->velocity.x, targetVelocity.x, maxDelta);
            physics->velocity.y = moveToward(physics->velocity.y, targetVelocity.y, maxDelta);
        }
        else if (toGoal.lengthSquared() > 0.0f) {
            physics->velocity = toGoal.normalized() * ai->moveSpeed;
        }
    }
    /**
     * @brief Updates the behavior of an AI entity in a platformer game, including movement states such as strafing and following the player.
     * @param ai The AI component containing the AI's properties and state.
     * @param transform The transform component of the AI entity, used for position and scale.
     * @param physics The physics component of the AI entity, used for velocity and movement.
     * @param entity The AI entity being updated.
     * @param scene The scene containing the AI entity and other game entities.
     * @param dt The delta time since the last update, used for timing updates.
     * @param resourceManager The resource manager used to retrieve textures for projectiles and other resources.
     */
    void PlatformerAI(std::shared_ptr<AI> ai, std::shared_ptr<Transform> transform, std::shared_ptr<Physics> physics, std::shared_ptr<Entity> entity,
        Scene& scene, float dt, ResourceManager& resourceManager) {
        auto playerEntity = scene.getPlayerEntity();
        if (!playerEntity) {
            return;
        }
        auto playerTransform = playerEntity->getComponent<Transform>();
        if (!playerTransform) {
            return;
        }
        sf::Vector2f aiPosition = scene.getWorldPosition(entity);
        sf::Vector2f strafePosition = entity->getParentEntity().lock() ? scene.getWorldPosition(entity->getParentEntity().lock()) : aiPosition;
        sf::Vector2f aiToPlayerVec =  aiPosition - scene.getWorldPosition(playerEntity);
        float distanceFromStartX = strafePosition.x - ai->strafeStartX;
        float distanceFromStartY = strafePosition.y - ai->strafeStartY;
        float strafeLimit = std::abs(ai->strafeDistance);
        float targetVelocityX = 0.0f;
        float targetVelocityY = physics->velocity.y;
        bool controlsVerticalVelocity = false;
        switch (ai->currentMovementState) {
            case PlatformerMovementStates::Strafe:
                if (ai->verticalStrafe) {
                    if (!ai->hasStrafeStart) {
                        ai->strafeStartY = strafePosition.y;
                        ai->hasStrafeStart = true;
                    }
                    if (ai->strafeDirection == 0) {
                        ai->strafeDirection = 1;
                    }
                    if ((ai->strafeDirection > 0 && distanceFromStartY >= strafeLimit) ||
                        (ai->strafeDirection < 0 && distanceFromStartY <= -strafeLimit)) {
                        ai->strafeDirection *= -1;
                    }
                    targetVelocityY = ai->moveSpeed * ai->strafeDirection;
                    controlsVerticalVelocity = true;
                    break;
                }
                else {
                    if (!ai->hasStrafeStart) {
                        ai->strafeStartX = strafePosition.x;
                        ai->hasStrafeStart = true;
                    }
                    if (ai->strafeDirection == 0) {
                        ai->strafeDirection = 1;
                    }
                    if ((ai->strafeDirection > 0 && distanceFromStartX >= strafeLimit) ||
                        (ai->strafeDirection < 0 && distanceFromStartX <= -strafeLimit)) {
                        ai->strafeDirection *= -1;
                    }
                    targetVelocityX = ai->moveSpeed * ai->strafeDirection;
                    break;    
                }
            case PlatformerMovementStates::Follow:
                if (aiToPlayerVec.x > 0.0f) {
                    targetVelocityX = -ai->moveSpeed;
                }
                else if (aiToPlayerVec.x < 0.0f) {
                    targetVelocityX = ai->moveSpeed;
                }
                break;
            case PlatformerMovementStates::Idle:
            default:
                // do a little jig for em bro
                break;
        }
        if (physics->useAcceleration) {
            float maxDeltaX = targetVelocityX == 0.0f ? ai->deceleration * dt : ai->acceleration * dt;
            physics->velocity.x = moveToward(physics->velocity.x, targetVelocityX, maxDeltaX);
            if (controlsVerticalVelocity) {
                float maxDeltaY = targetVelocityY == 0.0f ? ai->deceleration * dt : ai->acceleration * dt;
                physics->velocity.y = moveToward(physics->velocity.y, targetVelocityY, maxDeltaY);
            }
        }
        else {
            physics->velocity.x = targetVelocityX;
            if (controlsVerticalVelocity) {
                physics->velocity.y = targetVelocityY;
            }
        }
        float distanceBetween = aiToPlayerVec.length();
        if (ai->currentMovementState == ai->defaultMovementState) {
            if (ai->canFollow && distanceBetween < ai->aggroRange) {
                ai->currentMovementState = PlatformerMovementStates::Follow;
            }
        }
        else if (ai->currentMovementState == PlatformerMovementStates::Follow) {
            if (distanceBetween > ai->aggroRange) {
                ai->currentMovementState = ai->defaultMovementState;
            }
        }
    }
    /**
     * @brief Parses AI input for all entities in the scene, updating their behavior based on their AI components and movement types.
     * @param scene The scene containing the entities and their components.
     * @param dt The delta time since the last update, used for timing updates.
     * @param resourceManager The resource manager used to retrieve textures for projectiles and other resources
     */
    void parseAIInput(Scene &scene, float dt, ResourceManager& resourceManager, AudioSystem& audioSystem)
    {
        auto entities = scene.getEntities();
        for (auto& entity : entities) {
            if (!entity || !entity->isAlive() || entity->isBlueprint()) {
                continue;
            }
            const auto componentMask = entity->getComponentMask();
            if ((componentMask & ComponentTypes::AI) == 0 ||
                (componentMask & ComponentTypes::Physics) == 0 ||
                (componentMask & ComponentTypes::Transform) == 0 ||
                !scene.getPlayerEntity()) {
                continue;
            }

            auto ai = entity->getComponent<AI>();
            auto physics = entity->getComponent<Physics>();
            auto transform = entity->getComponent<Transform>();
            auto playerEntity = scene.getPlayerEntity();


            switch (ai->movementType) {
                case MovementType::TopDown:
                if (!physics->useAcceleration) {
                    physics->velocity *= 0.f;
                }
                topDownAI(ai, transform, physics, entity, scene, dt, resourceManager, audioSystem);
                break;
                case MovementType::Platformer:
                default:
                PlatformerAI(ai, transform, physics, entity, scene, dt, resourceManager);
            }
        }
    }
}
/**
 * @brief Updates the input system for the scene, processing player and AI input, updating UI text, component timers, and handling entity spawning.
 * @param scene The scene containing the entities and their components.
 * @param dt The delta time since the last update, used for timing updates.
 * @param window The render window used for coordinate mapping and input handling.
 * @param resourceManager The resource manager used to retrieve textures and fonts for projectiles and other resources.
 */
void InputSystem::update(Scene& scene, float dt, sf::RenderWindow& window, ResourceManager& resourceManager, AudioSystem& audioSystem) {
    parsePlayerInput(scene, dt, window, resourceManager, audioSystem);
    parseAIInput(scene, dt, resourceManager, audioSystem);
    handleUITextUpdates(scene);
    updateComponentTimers(scene, dt);
    handleSpawner(scene, dt, resourceManager);
    
}
