add_library(Engine STATIC
        src/Engine.cpp
        src/Window.cpp
        src/NodeEditor.cpp

        # Исходники ядра ImGui
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp

        # Бэкенды ImGui
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp

        # Исходники ImGui Node Editor
        ${imgui_node_editor_SOURCE_DIR}/imgui_node_editor.cpp
        ${imgui_node_editor_SOURCE_DIR}/imgui_node_editor_api.cpp
        ${imgui_node_editor_SOURCE_DIR}/imgui_canvas.cpp
        ${imgui_node_editor_SOURCE_DIR}/crude_json.cpp
)

target_include_directories(Engine PUBLIC
        include
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${glfw_SOURCE_DIR}/include
        ${glm_SOURCE_DIR}/glm
        ${entt_SOURCE_DIR}/src
        ${imgui_node_editor_SOURCE_DIR}
)

target_link_libraries(Engine PUBLIC
        glfw
        Vulkan::Vulkan
)