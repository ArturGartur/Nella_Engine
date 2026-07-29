cmake_minimum_required(VERSION 4.3)
project(AMGR_Engine)

# Устанавливаем стандарт C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Ищем Vulkan (он найдется автоматически благодаря SDK)
find_package(Vulkan REQUIRED)

# Подключаем встроенный модуль CMake для скачивания пакетов
include(FetchContent)

# 2. Скачиваем GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4 # Последняя стабильная версия
)
# Отключаем сборку тестов и примеров GLFW, чтобы не тратить время
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# 3. Скачиваем EnTT
FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG        v3.13.2
)
FetchContent_MakeAvailable(entt)

# Добавляем наш главный файл
add_executable(AMGR_Engine main.cpp
        Engine.cpp
        Engine.h)

# 4. Линкуем (связываем) все библиотеки с нашим движком
target_link_libraries(AMGR_Engine PRIVATE
        Vulkan::Vulkan
        glfw
        EnTT::EnTT
)