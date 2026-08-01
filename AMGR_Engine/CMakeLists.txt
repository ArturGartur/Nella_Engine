cmake_minimum_required(VERSION 3.20)
project(AMGR_Engine)

# Требуем C++20 для EnTT и современных фич
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Подключение зависимостей через FetchContent ---
include(FetchContent)

# 1. GLFW (Создание окон и ввод)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)
# Отключаем сборку тестов и примеров GLFW для ускорения компиляции
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# 2. EnTT (ECS фреймворк)
FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG        v3.13.2
)
FetchContent_MakeAvailable(entt)

# 3. Dear ImGui (Ветка docking для открепляемых окон)
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)

# 4. GLM (Математика для Vulkan)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# 5. Vulkan (Ищем локальный Vulkan SDK)
find_package(Vulkan REQUIRED)

# --- Создание библиотеки движка ---
add_library(AMGR_Engine STATIC
        # Твои исходники
        src/Engine.cpp
        src/Window.cpp

        # Исходники ImGui
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp

        # Бэкенды ImGui для Vulkan и GLFW
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

# --- Указываем директории с заголовочными файлами ---
target_include_directories(AMGR_Engine PUBLIC
        include
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${glfw_SOURCE_DIR}/include
        ${entt_SOURCE_DIR}/src
)

# --- Линкуем библиотеки ---
target_link_libraries(AMGR_Engine PUBLIC
        Vulkan::Vulkan
        glfw
        glm::glm
)