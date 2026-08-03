#pragma once

#include <vector>
#include <string>
#include <set>
#include <imgui.h>
#include <imgui_node_editor.h>
#include <nlohmann/json.hpp>

namespace ed = ax::NodeEditor;

enum class NodeType {
    Start,
    Action,
    Branch
};

struct EditorNode {
    int ID;
    std::string Name;
    NodeType Type = NodeType::Action;
    int InputPinID = -1;
    int OutputPinID = -1;
    int OutputFalsePinID = -1;
    char Text[256] = "";
};

struct EditorLink {
    ed::LinkId ID;
    ed::PinId InputId;
    ed::PinId OutputId;
};

class NodeEditor {
public:
    void Initialize();
    void Shutdown();
    void draw();

    void Save(const std::string& filename);
    void Load(const std::string& filename);

    void ExecuteGraph();
    void AddLog(const std::string& message);

private:
    void ExecuteNodeInternal(int nodeId, std::set<int>& currentPath);

    ed::EditorContext* m_Context = nullptr;
    std::vector<EditorNode> m_Nodes;
    std::vector<EditorLink> m_Links;
    std::vector<std::string> m_LogMessages;

    int m_NextId = 1;
};