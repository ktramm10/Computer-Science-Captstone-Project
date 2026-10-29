# User Guide: AI Game Engine

This guide explains how to set up, build, run, and use the AI Game Engine on Windows from a clean source-only copy of the project. A user does not need the original VS Code workspace, Visual Studio solution, generated project files, or existing build folder.

## 1. Requirements

Install these before opening the project:

- Visual Studio 2022 with the Desktop development with C++ workload
- CMake 3.24 or newer, available on your PATH
- Git
- Python 3.10 or newer, available through the Windows `py` launcher or on your PATH
- VS Code
- VS Code CMake Tools extension

CMake downloads the C++ libraries used by the project, including SFML, ImGui-SFML, Dear ImGui, and nlohmann/json. The first configure/build may take several minutes and requires internet access.

## 2. Source-Only Project Files

If you are sharing or receiving the project without generated editor/build files, keep these files and folders:

- `CMakeLists.txt`
- `requirements.txt`
- `src`
- `external`
- `docs`, if you want the documentation included
- Any top-level runtime configuration files you intentionally use, such as `imgui.ini`

The `src` folder must include the C++ files, headers, AI files, JSON scene files, asset manifests, fonts, images, music, and sound effects. In particular, keep these resource folders and files:

- `src\AI\train_model.py`
- `src\AI\training.csv`
- `src\assets`
- `src\fonts`
- `src\images`
- `src\json`
- `src\music`
- `src\sound_effects`

CMake recreates the build folder and the build-system files needed for the selected generator. With a Visual Studio generator, that includes the Visual Studio solution and project files. Python recreates the virtual environment and AI model when the user follows the AI setup steps.

## 3. Open the Project

1. Open VS Code.
2. Select File > Open Folder.
3. Open the folder that contains `CMakeLists.txt`.
4. If VS Code asks for a CMake kit, choose a Visual Studio 2022 kit.

You can also build without VS Code by opening PowerShell in the folder that contains `CMakeLists.txt` and using the commands in Section 5.

## 4. Set Up the AI Model

The AI prompt feature uses Python and scikit-learn. Run these commands from the project root before using AI Prompt:

```powershell
py -m venv .venv
.\.venv\Scripts\python -m pip install --upgrade pip
.\.venv\Scripts\python -m pip install -r requirements.txt
.\.venv\Scripts\python src\AI\train_model.py
```

The training command reads `src\AI\training.csv` and creates:

```text
src\AI\game_prompt_model.joblib
```

You only need to repeat this step if the virtual environment, requirements, training data, or model file changes. If you prefer to run training through CMake after dependencies are installed, build the `train_ai_model` target.

## 5. Build the Engine

In VS Code:

1. Open the Command Palette with Ctrl+Shift+P.
2. Run CMake: Configure.
3. Run CMake: Set Build Target and choose `MyGameEngine`.
4. Run CMake: Build.

Or build from PowerShell at the project root:

```powershell
cmake -S . -B build
cmake --build build --config Debug --target MyGameEngine
```

The executable is created at:

```text
build\Debug\MyGameEngine.exe
```

For a Release build, replace `Debug` with `Release` in the build command and executable path.

## 6. Run the Engine

In VS Code, use the CMake Tools launch button or run the `MyGameEngine` target.

From PowerShell at the project root, run:

```powershell
.\build\Debug\MyGameEngine.exe
```

The engine resolves assets from the compiled project root, so it can be launched from the project root or from VS Code/CMake Tools.

## 7. Load or Create a Scene

When the window opens, use the scene selection panel:

- Select a scene to load an existing saved scene.
- Click Create New Scene to start with an empty scene.
- Click Create New Scene with AI Prompt to generate a starter scene from a prompt.

Saved scenes are stored in:

```text
src\json\saved_scenes
```

AI Prompt scene outlines are stored in `src\json\scene_outlines` and are copied into the saved scenes folder when a generated scene is saved.

## 8. Main Toolbar

After a scene is loaded, use the toolbar for the main workflow:

- Play saves the scene and starts the simulation.
- Stop ends the simulation and reloads the scene from disk.
- Save Scene saves the current scene.
- Save Scene As saves the scene with a new name.
- Reload Current Scene reloads the current file.
- Load New Scene returns to the scene selection panel.
- New Scene creates another blank scene.
- AI Prompt opens the AI prompt panel.
- Hide Editor hides the editor panels.

## 9. Use the AI Prompt

1. Click AI Prompt or Create New Scene with AI Prompt.
2. Enter a prompt, such as `make a platformer with coins`.
3. Enter a scene name.
4. Click Generate Scene.

The engine classifies the prompt, loads a matching scene outline, saves the generated scene, and shows AI results such as genre, template, confidence, entities, components, and model diagnostics. If the model file is missing, the predictor script will train it automatically, but first-time AI use is faster and more reliable if you complete Section 4 first.

## 10. Basic Controls

Editor mode:

- Arrow keys move the editor camera.
- Left mouse drag moves selected entities when drag movement is enabled.

Play mode:

- A/D or Left/Right move horizontally.
- W/S or Up/Down move vertically in top-down scenes.
- Space or Up jumps in platformer scenes.
- Left mouse button fires projectiles toward the mouse position if the player has the required input, physics, shooter, transform, and collision components.

## Quick Start

For normal use after setup:

1. Open the project in VS Code.
2. Configure CMake with a Visual Studio 2022 kit if VS Code asks.
3. Build `MyGameEngine` if code changed.
4. Run `.\build\Debug\MyGameEngine.exe` from the project root, or launch `MyGameEngine` with CMake Tools.
5. Load or create a scene.
6. Click Play to test it.

## Troubleshooting

- CMake cannot configure.
    Confirm Visual Studio 2022, the Desktop development with C++ workload, CMake 3.24 or newer, Git, and internet access are available. Then run CMake: Configure again.
- Build succeeds but the executable path is different.
    Check the active CMake build configuration. Debug builds use `build\Debug\MyGameEngine.exe`; Release builds use `build\Release\MyGameEngine.exe`.
- AI Prompt fails or reports Python errors.
    Run the commands in Section 4 again from the project root. Confirm `.venv\Scripts\python.exe`, `src\AI\train_model.py`, `src\AI\training.csv`, and `src\AI\game_prompt_model.joblib` exist.
- The user received only source files and no build folder.
    This is expected. Run CMake: Configure and CMake: Build in VS Code, or run the PowerShell build commands in Section 5. CMake will create a new `build` folder.
- Assets, fonts, sounds, or music do not load.
    Check the JSON manifests in `src\assets`. Paths in those manifests should point to existing files under the project folder.
- No scenes appear in the scene selection panel.
    Confirm saved scene JSON files exist in `src\json\saved_scenes`.