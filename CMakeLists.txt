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

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b
    SOURCE_SUBDIR do-not-add-imgui-directly
)

FetchContent_MakeAvailable(imgui)

set(IMGUI_DIR ${imgui_SOURCE_DIR} CACHE PATH "Dear ImGui source directory" FORCE)
set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "Use the already-fetched SFML target" FORCE)

FetchContent_Declare(
    ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v3.0
)

set(JSON_BuildTests OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(SFML nlohmann_json ImGui-SFML)

add_executable(MyGameEngine
    src/Engine.cpp
    src/Entity.cpp
    src/Scene.cpp
    src/Renderer.cpp
    src/PhysicsSystem.cpp
    src/CollisionSystem.cpp
    src/Health.cpp
    src/InputSystem.cpp
    src/Interactable.cpp
    src/ResourceManager.cpp
    src/SceneLoader.cpp
    src/SceneSerializer.cpp
    src/EditorUI.cpp
)

target_link_libraries(MyGameEngine PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    ImGui-SFML::ImGui-SFML
    nlohmann_json::nlohmann_json
)