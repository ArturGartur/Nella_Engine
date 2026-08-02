cmake_minimum_required(VERSION 3.20)
project(AMGR_Engine_Project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. GLFW
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
FetchContent_MakeAvailable(glfw)

# 2. GLM
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1)
FetchContent_MakeAvailable(glm)

# 3. EnTT
FetchContent_Declare(entt GIT_REPOSITORY https://github.com/skypjack/entt.git GIT_TAG v3.13.0)
FetchContent_MakeAvailable(entt)

# 4. ImGui (Стабильная версия с поддержкой Docking)
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG v1.89.9-docking)
FetchContent_MakeAvailable(imgui)

# 5. ImGui Node Editor (Стабильная версия, без конфликтов математики)
FetchContent_Declare(imgui_node_editor GIT_REPOSITORY https://github.com/thedmd/imgui-node-editor.git GIT_TAG v0.9.2)
FetchContent_MakeAvailable(imgui_node_editor)

# Vulkan
find_package(Vulkan REQUIRED)

# Подключаем папки движка и игры
add_subdirectory(Engine)
add_subdirectory(Nella)