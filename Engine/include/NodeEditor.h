#pragma once
#include <imgui.h>
#include <imgui_node_editor.h>
#include <memory>

namespace ed = ax::NodeEditor;

class NodeEditor {
public:
    NodeEditor();
    ~NodeEditor();

    void draw();

private:
    ed::EditorContext* m_Context = nullptr;
};