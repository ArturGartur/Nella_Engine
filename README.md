# Nella Engine

A custom, data-driven 3D game engine built from scratch with C++20 and Vulkan. 

Designed with a modular architecture (Engine as a dynamic library, Game as an executable), AMGR Engine focuses on high performance and a smooth, visual developer experience.

## 🛠 Tech Stack

* **Core:** C++20
* **Graphics API:** Vulkan
* **Windowing & Input:** GLFW
* **Entity Component System (ECS):** EnTT
* **Build System:** CMake
* **UI (Editor):** Dear ImGui (Integration in progress)

## ✨ Core Philosophy & Features

* **Modular Architecture:** The engine is compiled as a standalone `.dll` (or `.so`), keeping the engine logic strictly separated from the game logic.
* **Data-Driven Pipeline:** Levels and assets are managed via data formats (JSON/GLTF), allowing for iterative development without constant C++ recompilation.
* **Visual Editing:** Moving towards a robust, in-engine visual editor interface to provide an accessible, immediate level-design experience.
* **High Performance:** Leveraging Vulkan's low-level capabilities for maximum rendering efficiency.

## 🚀 Getting Started

### Prerequisites
* [CMake](https://cmake.org/) (v3.20 or higher)
* [Vulkan SDK](https://vulkan.lunarg.com/) (v1.3+)
* A compiler supporting C++20 (MSVC, GCC, or Clang)

### Building the Project

The project uses CMake to fetch dependencies (like GLFW and EnTT) automatically.

