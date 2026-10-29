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
    src/AudioSystem.cpp
    src/Health.cpp
    src/InputSystem.cpp
    src/Interactable.cpp
    src/ResourceManager.cpp
    src/SceneLoader.cpp
    src/SceneSerializer.cpp
    src/AIPromptBridge.cpp
    src/EditorUI.cpp
    src/Hazard.cpp
    src/Script.cpp
    src/ScriptSystem.cpp
    src/TimerManager.cpp
    src/StringHelper.cpp
    external/TextEditor.cpp
)

target_include_directories(MyGameEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)

target_compile_definitions(MyGameEngine PRIVATE
    PROJECT_ROOT_PATH="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(MyGameEngine PRIVATE
    SFML::Graphics
    SFML::Audio
    SFML::Window
    SFML::System
    ImGui-SFML::ImGui-SFML
    nlohmann_json::nlohmann_json
)

set(PROJECT_VENV_PYTHON ${CMAKE_CURRENT_SOURCE_DIR}/.venv/Scripts/python.exe)

if(EXISTS ${PROJECT_VENV_PYTHON})
    set(PROJECT_PYTHON_EXECUTABLE ${PROJECT_VENV_PYTHON})
else()
    find_package(Python3 COMPONENTS Interpreter)
    if(Python3_Interpreter_FOUND)
        set(PROJECT_PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
    endif()
endif()

if(PROJECT_PYTHON_EXECUTABLE)
    add_custom_target(train_ai_model
        COMMAND ${PROJECT_PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/src/AI/train_model.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Training the scikit-learn game prompt model"
        VERBATIM
    )
endif()