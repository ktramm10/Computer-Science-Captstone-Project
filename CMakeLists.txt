cmake_minimum_required(VERSION 3.24)

project(MyGameEngine)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

set(JSON_BuildTests OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(SFML nlohmann_json)

add_executable(MyGameEngine
    src/Engine.cpp
    src/Entity.cpp
    src/Scene.cpp
    src/Renderer.cpp
    src/PhysicsSystem.cpp
    src/CollisionSystem.cpp
    src/InputSystem.cpp
    src/ResourceManager.cpp
    src/SceneLoader.cpp
    src/SceneSerializer.cpp
)

target_link_libraries(MyGameEngine PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    nlohmann_json::nlohmann_json
)