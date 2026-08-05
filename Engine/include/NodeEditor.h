#pragma once

#include <vector>
#include <string>
#include <set>
#include <functional>
#include <variant>
#include <map>
#include <imgui.h>
#include <imgui_node_editor.h>
#include <nlohmann/json.hpp>

namespace ed = ax::NodeEditor;

enum class NodeType {
    Start,
    Action,
    Branch,
    Data
};

enum class PinType {
    Flow,
    Int,
    Float,
    String,
    Entity
};

struct NodePin {
    ed::PinId ID;
    ed::NodeId NodeID;
    std::string Name;
    PinType Type;
    ed::PinKind Kind;

    std::variant<int, float, std::string, uint32_t> Value;

    NodePin(int id, int nodeId, const std::string& name, PinType type, ed::PinKind kind)
        : ID(id), NodeID(nodeId), Name(name), Type(type), Kind(kind) {

        if (type == PinType::Int) Value = 0;
        else if (type == PinType::Float) Value = 0.0f;
        else if (type == PinType::String) Value = std::string("");
        else if (type == PinType::Entity) Value = (uint32_t)0;
        else Value = 0;
    }
};

struct EditorNode {
    ed::NodeId ID;
    std::string Name;
    NodeType Type;

    std::vector<NodePin> Inputs;
    std::vector<NodePin> Outputs;

    std::function<void(class NodeEditor*, EditorNode*)> ActionCallback = nullptr;
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

    void RegisterCppNode(const std::string& name, std::function<void(NodeEditor*, EditorNode*)> callback);

    NodePin* FindPin(ed::PinId id);
    EditorNode* FindNode(ed::NodeId id);

private:
    void ExecuteNodeInternal(ed::NodeId nodeId, std::set<int>& currentPath);
    ImColor GetIconColor(PinType type);

    std::map<std::string, std::function<void(NodeEditor*, EditorNode*)>> m_NodeRegistry;

    ed::EditorContext* m_Context = nullptr;
    std::vector<EditorNode> m_Nodes;
    std::vector<EditorLink> m_Links;
    std::vector<std::string> m_LogMessages;

    int m_NextId = 1;
    char m_SaveFilepath[256] = "Nella_graph_save.json";
};