# JSON Scripting Guide

This guide explains how to use JSON scripts in the engine. Scripts are event driven: an entity owns a `script` component, the script listens for events such as `OnStart` or `OnCollisionWithPlayer`, and each matching rule runs one or more actions.

## Where Scripts Live

Reusable script files are stored in:

```text
src\json\scripts
```

Saved scenes are stored in:

```text
src\json\saved_scenes
```

The editor can create, edit, and link reusable script files from `src\json\scripts`. When a scene is saved, the entity's script rules are also written into the scene JSON under that entity's `components.script` object.

## Basic Script Shape

A reusable script file should normally use this structure:

```json
{
    "started": false,
    "rules": [
        {
            "event": 0,
            "actions": [
                {
                    "type": 3,
                    "x": 100.0,
                    "y": 0.0
                }
            ]
        }
    ]
}
```

The same structure appears inside a saved scene like this:

```json
{
    "name": "Moving Platform",
    "isPlayer": false,
    "isBlueprint": false,
    "id": 12,
    "parentID": -1,
    "components": {
        "transform": {
            "position": [100.0, 200.0],
            "localPosition": [0.0, 0.0],
            "scale": [1.0, 1.0],
            "rotation": 0.0
        },
        "physics": {
            "velocity": [0.0, 0.0],
            "acceleration": [0.0, 0.0],
            "gravity": 0.0,
            "isGrounded": false,
            "useGravity": false,
            "useAcceleration": false
        },
        "script": {
            "started": false,
            "filePath": "src/json/scripts/move_right.json",
            "fileName": "move_right",
            "rules": [
                {
                    "event": 0,
                    "actions": [
                        {
                            "type": 3,
                            "x": 100.0,
                            "y": 0.0,
                            "value": 0.0,
                            "target": ""
                        }
                    ]
                }
            ]
        }
    }
}
```

## Script Fields

| Field | Type | Meaning |
| --- | --- | --- |
| `started` | Boolean | Tracks whether `OnStart` has already fired for this component. Set this to `false` in script files and hand-written scenes unless you intentionally want to skip `OnStart`. |
| `filePath` | String | Optional path to the source script file. Scene saving writes this for linked scripts. |
| `fileName` | String | Optional script display name. Scene saving writes this for linked scripts. |
| `rules` | Array | List of event rules. Each rule has one event ID and an array of actions. |

The editor's script importer also accepts a raw array of rules, but saved scenes use the object form shown above.

## Rule Fields

| Field | Type | Meaning |
| --- | --- | --- |
| `event` | Number | Event ID that triggers the rule. See the event table below. |
| `actions` | Array | Actions to run, in order, when the event is dispatched. |

## Action Fields

| Field | Type | Meaning |
| --- | --- | --- |
| `type` | Number | Action ID to execute. See the action table below. |
| `x` | Number | Optional x value. Used by velocity, sprite origin, and local sprite position actions. Defaults to `0.0`. |
| `y` | Number | Optional y value. Used by velocity, sprite origin, and local sprite position actions. Defaults to `0.0`. |
| `value` | Number | Optional scalar value. Used by rotation, random movement, timers, and player-targeted movement. Defaults to `0.0`. |
| `target` | String | Optional target string. Used for scene paths, door/key IDs, and entity names. Defaults to an empty string. |

Unused fields can be omitted. The loader fills missing numeric fields with `0.0` and missing target strings with `""`.

## Event IDs

| ID | Event | When it runs |
| --- | --- | --- |
| `0` | `OnStart` | Once, the first time the script system updates an active entity with this script. |
| `1` | `OnUpdate` | Every script system update while the entity is alive and not a blueprint. |
| `2` | `OnCollision` | When the entity collides with another entity. |
| `3` | `OnTimeout` | When a timer started for this entity expires. |
| `4` | `OnDestroyed` | When the entity is found destroyed by the script system. |
| `5` | `OnCollisionWithPlayer` | When the entity collides with the current player entity. |
| `6` | `OnSwitchActivate` | When this entity's switch is activated. |
| `7` | `OnSwitchDeactivate` | When this entity's switch is deactivated. |
| `8` | `OnPowerUpCollected` | When this power-up entity is collected. |
| `9` | `OnPlayerDeath` | Broadcast to active scripted entities when the player dies. |

## Action IDs

| ID | Action | Required or useful fields | Effect |
| --- | --- | --- | --- |
| `0` | `RequestSceneLoad` | `target` | Requests a scene load. The target should be a scene JSON path, such as `src/json/saved_scenes/You Win.json`. |
| `1` | `SetRotation` | `value` | Sets the entity transform rotation to `value`. Requires a `transform` component. |
| `2` | `AddRotation` | `value` | Adds `value * dt` to the entity transform rotation each time it runs. Requires a `transform` component. |
| `3` | `SetVelocity` | `x`, `y` | Sets physics velocity to `[x, y]`. Requires a `physics` component. |
| `4` | `AddVelocityInRandomDirection` | `value` | Adds random velocity scaled by `value * dt`. Requires a `physics` component. |
| `5` | `SetVelocityInRandomDirection` | `value` | Sets velocity to a random direction scaled by `value`. Requires a `physics` component. |
| `6` | `AddVelocity` | `x`, `y`, `value` | Adds `[x, y] * value * dt` to velocity. Requires a `physics` component. |
| `7` | `SetSpriteOrigin` | `x`, `y` | Sets the SFML sprite origin. Requires a `sprite` component. |
| `8` | `SetLocalSpritePosition` | `x`, `y` | Sets the transform local position to `[x, y]`. Requires a `transform` component. |
| `9` | `StartTimer` | `value` | Starts a timer for this entity that lasts `value` seconds. When it expires, `OnTimeout` runs. |
| `10` | `SetVelocityTowardsPlayer` | `value` | Sets velocity toward the player, scaled by `value`. Requires `physics` and `transform`; the scene must have a player. |
| `11` | `AddVelocityTowardsPlayer` | `value` | Declared in the enum, but not currently handled by the runtime switch. |
| `12` | `UnlockDoor` | `target` | Adds `target` to the game state's key IDs and refreshes door lock states. Use the door/key ID as the target. |
| `13` | `SpawnEntity` | `target` | Declared in the enum, but not currently handled by the runtime switch. |
| `14` | `DisableEntity` | `target` | Finds entities whose name equals `target` and marks them as blueprints, making them inactive. |
| `15` | `EnableEntity` | `target` | Finds entities whose name equals `target` and clears blueprint mode, making them active. |
| `16` | `DestroyEntity` | `target` | Finds entities whose name equals `target` and destroys them. |
| `17` | `SpawnPopText` | none | Creates a temporary pop text entity using this entity's `popText` component. Loot and power-up data can change the text. |
| `18` | `CollectLoot` | none | Adds this entity's `loot` component values to the game state. |
| `19` | `PlaySound` | none | Plays this entity's `sound` component. |
| `20` | `StopSound` | none | Stops this entity's `sound` component. |
| `21` | `ReloadCurrentScene` | none | Requests a reload of the current scene. |

## Common Examples

### Rotate an Entity Continuously

The entity needs a `transform` component.

```json
{
    "started": false,
    "rules": [
        {
            "event": 1,
            "actions": [
                {
                    "type": 2,
                    "value": 90.0
                }
            ]
        }
    ]
}
```

Because `AddRotation` multiplies by `dt`, this rotates at about 90 degrees per second.

### Load a Win Scene on Player Collision

The entity needs a `script` component and a collision setup that can collide with the player.

```json
{
    "started": false,
    "rules": [
        {
            "event": 5,
            "actions": [
                {
                    "type": 0,
                    "target": "src/json/saved_scenes/You Win.json"
                }
            ]
        }
    ]
}
```

### Toggle a Wall With a Switch

The switch entity needs a `switch` component and this script. The wall entity must be named `temp_wall`.

```json
{
    "started": false,
    "rules": [
        {
            "event": 6,
            "actions": [
                {
                    "type": 14,
                    "target": "temp_wall"
                }
            ]
        },
        {
            "event": 7,
            "actions": [
                {
                    "type": 15,
                    "target": "temp_wall"
                }
            ]
        }
    ]
}
```

### Run an Action on a Timer Loop

This starts a timer on `OnStart`, then restarts it every time it expires. The entity needs the components required by the actions you run on timeout.

```json
{
    "started": false,
    "rules": [
        {
            "event": 0,
            "actions": [
                {
                    "type": 9,
                    "value": 3.0
                }
            ]
        },
        {
            "event": 3,
            "actions": [
                {
                    "type": 5,
                    "value": 180.0
                },
                {
                    "type": 9,
                    "value": 3.0
                }
            ]
        }
    ]
}
```

## Using Scripts in the Editor

1. Select an entity.
2. Add a `Script` component if the entity does not already have one.
3. Create a new script or select an existing script from `src\json\scripts`.
4. Edit the JSON in the script editor.
5. Click `Save & Apply`.
6. Click `Play` to test the behavior.
7. Save the scene when the behavior works.

When you link an existing script, the editor reads the JSON rules into the selected entity's script component. When you use `Save & Apply`, the editor parses the JSON, applies it to the selected entity, and writes it back to the script file.

