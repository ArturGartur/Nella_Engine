cmake_minimum_required(VERSION 3.20)
project(AMGR_Workspace)

# Требуем C++20 для всего проекта
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем вложенные проекты
add_subdirectory(AMGR_Engine)
add_subdirectory(Nella)