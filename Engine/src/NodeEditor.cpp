#include "NodeEditor.h"
#include <fstream>
#include <algorithm>
#include <cstring>

using json = nlohmann::json;

void NodeEditor::Initialize() {
    ed::Config config;
    config.SettingsFile = "NellaNodeEditor.json";
    m_Context = ed::CreateEditor(&config);
}

void NodeEditor::Shutdown() {
    ed::DestroyEditor(m_Context);
}

void NodeEditor::AddLog(const std::string& message) {
    m_LogMessages.push_back(message);
}

void NodeEditor::Save(const std::string& filename) {
    json j;

    for (const auto& node : m_Nodes) {
        j["nodes"].push_back(json::object({
            {"id", node.ID},
            {"name", node.Name},
            {"type", static_cast<int>(node.Type)},
            {"in_pin", node.InputPinID},
            {"out_pin", node.OutputPinID},
            {"out_false_pin", node.OutputFalsePinID},
            {"text", std::string(node.Text)}
        }));
    }

    for (const auto& link : m_Links) {
        j["links"].push_back(json::object({
            {"id", (int)link.ID.Get()},
            {"in_id", (int)link.InputId.Get()},
            {"out_id", (int)link.OutputId.Get()}
        }));
    }

    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    }
}

void NodeEditor::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    json j;
    try {
        file >> j;
    } catch (const std::exception& e) {

        return;
    }

    m_Nodes.clear();
    m_Links.clear();
    m_NextId = 1;

    if (j.contains("nodes")) {
        for (const auto& item : j["nodes"]) {
            EditorNode newNode;
            newNode.ID = item["id"].get<int>();
            newNode.Name = item["name"].get<std::string>();
            newNode.Type = static_cast<NodeType>(item.value("type", 1));
            newNode.InputPinID = item.value("in_pin", -1);
            newNode.OutputPinID = item.value("out_pin", -1);
            newNode.OutputFalsePinID = item.value("out_false_pin", -1);

            std::string loadedText = item.value("text", "");
            strncpy(newNode.Text, loadedText.c_str(), sizeof(newNode.Text) - 1);

            m_Nodes.push_back(newNode);
            m_NextId = std::max(m_NextId, newNode.ID + 4);
        }
    }

    if (j.contains("links")) {
        for (const auto& item : j["links"]) {
            EditorLink newLink;
            newLink.ID = ed::LinkId(item["id"].get<int>());
            newLink.InputId = ed::PinId(item["in_id"].get<int>());
            newLink.OutputId = ed::PinId(item["out_id"].get<int>());
            m_Links.push_back(newLink);
            m_NextId = std::max(m_NextId, (int)newLink.ID.Get() + 1);
        }
    }
}

void NodeEditor::ExecuteGraph() {
    AddLog("=== GRAPH EXECUTION STARTED ===");

    if (m_Nodes.empty()) {
        AddLog("Graph is empty!");
        return;
    }

    auto startNodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(),
        [](const EditorNode& n) { return n.Type == NodeType::Start; });

    if (startNodeIt == m_Nodes.end()) {
        AddLog("Error: No 'Start' node found in the graph!");
        return;
    }

    std::set<int> currentPath;
    ExecuteNodeInternal(startNodeIt->ID, currentPath);

    AddLog("=== END OF GRAPH ===");
}

void NodeEditor::ExecuteNodeInternal(int nodeId, std::set<int>& currentPath) {
    if (currentPath.contains(nodeId)) {
        AddLog("-> Loop detected, stopping branch execution.");
        return;
    }

    currentPath.insert(nodeId);

    auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(),
        [nodeId](const EditorNode& n) { return n.ID == nodeId; });

    if (nodeIt == m_Nodes.end()) {
        currentPath.erase(nodeId);
        return;
    }

    std::string logMsg = "Action: " + nodeIt->Name + " | Data: " + std::string(nodeIt->Text);
    AddLog(logMsg);

    std::vector<int> outPins;
    if (nodeIt->OutputPinID != -1) outPins.push_back(nodeIt->OutputPinID);
    if (nodeIt->OutputFalsePinID != -1) outPins.push_back(nodeIt->OutputFalsePinID);

    for (int outPinId : outPins) {
        for (const auto& link : m_Links) {
            int nextTargetPinId = -1;

            if ((int)link.InputId.Get() == outPinId) {
                nextTargetPinId = (int)link.OutputId.Get();
            } else if ((int)link.OutputId.Get() == outPinId) {
                nextTargetPinId = (int)link.InputId.Get();
            }

            if (nextTargetPinId != -1) {
                auto nextNodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(),
                    [nextTargetPinId](const EditorNode& n) { return n.InputPinID == nextTargetPinId; });

                if (nextNodeIt != m_Nodes.end()) {
                    ExecuteNodeInternal(nextNodeIt->ID, currentPath);
                }
            }
        }
    }

    currentPath.erase(nodeId);
}

void NodeEditor::draw() {

    if (!m_Context) {
        return;
    }

    ImGui::Begin("Nella Visual Scripting");

    if (ImGui::Button("Save Graph")) {
        Save("Nella_graph_save.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Graph")) {
        Load("Nella_graph_save.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("Run Graph")) {
        ExecuteGraph();
    }

    ed::SetCurrentEditor(m_Context);
    ed::Begin("Nella Node Editor");

    for (auto& node : m_Nodes) {
        ed::BeginNode(node.ID);
        ImGui::Text(node.Name.c_str());

        if (node.Type != NodeType::Start) {
            ed::BeginPin(node.InputPinID, ed::PinKind::Input);
            ImGui::Text("-> In");
            ed::EndPin();
        }

        ImGui::SameLine(100);

        if (node.Type == NodeType::Branch) {
            ImGui::BeginGroup();
            ed::BeginPin(node.OutputPinID, ed::PinKind::Output);
            ImGui::Text("True ->");
            ed::EndPin();
            ed::BeginPin(node.OutputFalsePinID, ed::PinKind::Output);
            ImGui::Text("False ->");
            ed::EndPin();
            ImGui::EndGroup();
        } else {
            ed::BeginPin(node.OutputPinID, ed::PinKind::Output);
            ImGui::Text("Out ->");
            ed::EndPin();
        }

        ImGui::Dummy(ImVec2(0, 5));
        ImGui::PushItemWidth(200.0f);
        std::string label = "##text_" + std::to_string(node.ID);
        ImGui::InputText(label.c_str(), node.Text, IM_ARRAYSIZE(node.Text));
        ImGui::PopItemWidth();

        ed::EndNode();
    }

    for (auto& link : m_Links) {
        ed::Link(link.ID, link.InputId, link.OutputId);
    }

    if (ed::BeginCreate()) {
        ed::PinId inputPinId, outputPinId;
        if (ed::QueryNewLink(&inputPinId, &outputPinId)) {
            if (inputPinId && outputPinId) {
                if (ed::AcceptNewItem()) {
                    m_Links.push_back({ ed::LinkId(m_NextId++), inputPinId, outputPinId });
                }
            }
        }
    }
    ed::EndCreate();

    if (ed::BeginDelete()) {
        ed::LinkId deletedLinkId;
        while (ed::QueryDeletedLink(&deletedLinkId)) {
            if (ed::AcceptDeletedItem()) {
                m_Links.erase(std::remove_if(m_Links.begin(), m_Links.end(),
                    [deletedLinkId](const EditorLink& link) { return link.ID == deletedLinkId; }),
                    m_Links.end());
            }
        }

        ed::NodeId deletedNodeId;
        while (ed::QueryDeletedNode(&deletedNodeId)) {
            if (ed::AcceptDeletedItem()) {
                m_Nodes.erase(std::remove_if(m_Nodes.begin(), m_Nodes.end(),
                    [deletedNodeId](const EditorNode& node) { return node.ID == (int)deletedNodeId.Get(); }),
                    m_Nodes.end());
            }
        }
    }
    ed::EndDelete();

    ed::Suspend();
    if (ed::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup("NodeContextMenu");
    }

    if (ImGui::BeginPopup("NodeContextMenu")) {
        if (ImGui::MenuItem("Add Start Node")) {
            EditorNode node;
            node.ID = m_NextId++;
            node.Name = "Start";
            node.Type = NodeType::Start;
            node.OutputPinID = m_NextId++;
            m_Nodes.push_back(node);
        }
        if (ImGui::MenuItem("Add Action Node")) {
            EditorNode node;
            node.ID = m_NextId++;
            node.Name = "Action";
            node.Type = NodeType::Action;
            node.InputPinID = m_NextId++;
            node.OutputPinID = m_NextId++;
            m_Nodes.push_back(node);
        }
        if (ImGui::MenuItem("Add Branch (If)")) {
            EditorNode node;
            node.ID = m_NextId++;
            node.Name = "Branch";
            node.Type = NodeType::Branch;
            node.InputPinID = m_NextId++;
            node.OutputPinID = m_NextId++;
            node.OutputFalsePinID = m_NextId++;
            m_Nodes.push_back(node);
        }
        ImGui::EndPopup();
    }
    ed::Resume();

    ed::End();
    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(800.0f, 200.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Nella Console");

    if (ImGui::Button("Clear Log")) {
        m_LogMessages.clear();
    }

    ImGui::Separator();

    ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& msg : m_LogMessages) {
        ImGui::TextUnformatted(msg.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();

    ImGui::End();
}