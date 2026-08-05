#include "NodeEditor.h"
#include <fstream>
#include <algorithm>
#include <cstring>
#include <iostream>

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

ImColor NodeEditor::GetIconColor(PinType type) {
    switch (type) {
        case PinType::Flow:   return ImColor(255, 255, 255);
        case PinType::Int:    return ImColor(68, 201, 156);
        case PinType::Float:  return ImColor(147, 226, 74);
        case PinType::String: return ImColor(224, 108, 169);
        case PinType::Entity: return ImColor(255, 204, 0);
        default:              return ImColor(255, 255, 255);
    }
}

NodePin* NodeEditor::FindPin(ed::PinId id) {
    for (auto& node : m_Nodes) {
        for (auto& pin : node.Inputs)  if (pin.ID == id) return &pin;
        for (auto& pin : node.Outputs) if (pin.ID == id) return &pin;
    }
    return nullptr;
}

EditorNode* NodeEditor::FindNode(ed::NodeId id) {
    for (auto& node : m_Nodes) {
        if (node.ID == id) return &node;
    }
    return nullptr;
}

void NodeEditor::Save(const std::string& filename) {
    ed::SetCurrentEditor(m_Context);

    json j;
    for (const auto& node : m_Nodes) {
        ImVec2 pos = ed::GetNodePosition(node.ID);

        json j_node = {
            {"id", (int)node.ID.Get()},
            {"name", node.Name},
            {"type", static_cast<int>(node.Type)},
            {"x", pos.x},
            {"y", pos.y}
        };

        json j_inputs = json::array();
        for (const auto& pin : node.Inputs) {
            json j_pin = { {"id", (int)pin.ID.Get()}, {"name", pin.Name}, {"type", static_cast<int>(pin.Type)} };
            if (pin.Type == PinType::Int) j_pin["val_int"] = std::get<int>(pin.Value);
            else if (pin.Type == PinType::String) j_pin["val_str"] = std::get<std::string>(pin.Value);
            j_inputs.push_back(j_pin);
        }
        j_node["inputs"] = j_inputs;

        json j_outputs = json::array();
        for (const auto& pin : node.Outputs) {
            j_outputs.push_back({ {"id", (int)pin.ID.Get()}, {"name", pin.Name}, {"type", static_cast<int>(pin.Type)} });
        }
        j_node["outputs"] = j_outputs;

        j["nodes"].push_back(j_node);
    }

    for (const auto& link : m_Links) {
        j["links"].push_back({
            {"id", (int)link.ID.Get()},
            {"in_id", (int)link.InputId.Get()},
            {"out_id", (int)link.OutputId.Get()}
        });
    }

    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
        AddLog("Graph saved to: " + filename);
    }
}

void NodeEditor::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        AddLog("Failed to open save file: " + filename);
        return;
    }

    json j;
    try { file >> j; }
    catch (const std::exception& e) { AddLog("JSON parsing error!"); return; }

    m_Nodes.clear();
    m_Links.clear();
    m_NextId = 1;

    ed::SetCurrentEditor(m_Context);

    if (j.contains("nodes")) {
        for (const auto& item : j["nodes"]) {
            EditorNode newNode;
            newNode.ID = item["id"].get<int>();
            newNode.Name = item["name"].get<std::string>();
            newNode.Type = static_cast<NodeType>(item.value("type", 1));

            if (item.contains("inputs")) {
                for (const auto& pItem : item["inputs"]) {
                    PinType pType = static_cast<PinType>(pItem["type"].get<int>());
                    NodePin pin(pItem["id"].get<int>(), (int)newNode.ID.Get(), pItem["name"].get<std::string>(), pType, ed::PinKind::Input);

                    if (pType == PinType::Int && pItem.contains("val_int")) pin.Value = pItem["val_int"].get<int>();
                    if (pType == PinType::String && pItem.contains("val_str")) pin.Value = pItem["val_str"].get<std::string>();

                    newNode.Inputs.push_back(pin);
                    m_NextId = std::max(m_NextId, (int)pin.ID.Get() + 1);
                }
            }

            if (item.contains("outputs")) {
                for (const auto& pItem : item["outputs"]) {
                    PinType pType = static_cast<PinType>(pItem["type"].get<int>());
                    NodePin pin(pItem["id"].get<int>(), (int)newNode.ID.Get(), pItem["name"].get<std::string>(), pType, ed::PinKind::Output);
                    newNode.Outputs.push_back(pin);
                    m_NextId = std::max(m_NextId, (int)pin.ID.Get() + 1);
                }
            }

            if (m_NodeRegistry.find(newNode.Name) != m_NodeRegistry.end()) {
                newNode.ActionCallback = m_NodeRegistry[newNode.Name];
            }

            m_Nodes.push_back(newNode);
            m_NextId = std::max(m_NextId, (int)newNode.ID.Get() + 1);

            float x = item.value("x", 0.0f);
            float y = item.value("y", 0.0f);
            ed::SetNodePosition(newNode.ID, ImVec2(x, y));
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

    AddLog("Graph loaded from: " + filename);
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

void NodeEditor::ExecuteNodeInternal(ed::NodeId nodeId, std::set<int>& currentPath) {
    if (currentPath.contains((int)nodeId.Get())) {
        AddLog("-> Loop detected, stopping branch execution.");
        return;
    }

    currentPath.insert((int)nodeId.Get());

    auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(),
        [nodeId](const EditorNode& n) { return n.ID == nodeId; });

    if (nodeIt == m_Nodes.end()) {
        currentPath.erase((int)nodeId.Get());
        return;
    }

    std::string logMsg = "Action: " + nodeIt->Name;
    AddLog(logMsg);

    if (nodeIt->ActionCallback) {
        nodeIt->ActionCallback(this, &(*nodeIt));
    }

    std::vector<ed::PinId> outFlowPins;
    for (auto& pin : nodeIt->Outputs) {
        if (pin.Type == PinType::Flow) {
            outFlowPins.push_back(pin.ID);
        }
    }

    for (ed::PinId outPinId : outFlowPins) {
        for (const auto& link : m_Links) {
            ed::PinId nextTargetPinId = 0;

            if (link.InputId == outPinId) {
                nextTargetPinId = link.OutputId;
            } else if (link.OutputId == outPinId) {
                nextTargetPinId = link.InputId;
            }

            if (nextTargetPinId) {
                NodePin* nextPin = FindPin(nextTargetPinId);
                if (nextPin && nextPin->NodeID != nodeId) {
                    ExecuteNodeInternal(nextPin->NodeID, currentPath);
                }
            }
        }
    }

    currentPath.erase((int)nodeId.Get());
}

void NodeEditor::RegisterCppNode(const std::string& name, std::function<void(NodeEditor*, EditorNode*)> callback) {

    m_NodeRegistry[name] = callback;

    EditorNode node;
    node.ID = m_NextId++;
    node.Name = name;
    node.Type = NodeType::Action;
    node.ActionCallback = callback;

    node.Inputs.push_back(NodePin(m_NextId++, (int)node.ID.Get(), "In", PinType::Flow, ed::PinKind::Input));
    node.Outputs.push_back(NodePin(m_NextId++, (int)node.ID.Get(), "Out", PinType::Flow, ed::PinKind::Output));

    node.Inputs.push_back(NodePin(m_NextId++, (int)node.ID.Get(), "Unit Name", PinType::String, ed::PinKind::Input));
    node.Outputs.push_back(NodePin(m_NextId++, (int)node.ID.Get(), "Entity ID", PinType::Entity, ed::PinKind::Output));

    m_Nodes.push_back(node);
}

void NodeEditor::draw() {
    if (!m_Context) return;

    ImGui::Begin("Nella Visual Scripting");

    ImGui::PushItemWidth(200.0f);
    ImGui::InputText("##SaveFilepath", m_SaveFilepath, IM_ARRAYSIZE(m_SaveFilepath));
    ImGui::PopItemWidth();
    ImGui::SameLine();

    if (ImGui::Button("Save Graph")) Save(m_SaveFilepath);
    ImGui::SameLine();
    if (ImGui::Button("Load Graph")) Load(m_SaveFilepath);
    ImGui::SameLine();

    ImGui::Text("|");
    ImGui::SameLine();

    if (ImGui::Button("Run Graph", ImVec2(100, 0))) {
        ExecuteGraph();
    }

    ed::SetCurrentEditor(m_Context);
    ed::Begin("Nella Node Editor");

    for (auto& node : m_Nodes) {
        ed::BeginNode(node.ID);
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::Dummy(ImVec2(0, 4.0f));

        for (auto& pin : node.Inputs) {
            ed::BeginPin(pin.ID, ed::PinKind::Input);

            ImGui::PushStyleColor(ImGuiCol_Text, GetIconColor(pin.Type).Value);
            ImGui::TextUnformatted("->");
            ImGui::PopStyleColor();

            ImGui::SameLine();
            ImGui::TextUnformatted(pin.Name.c_str());

            bool isConnected = false;
            for (const auto& link : m_Links) {
                if (link.OutputId == pin.ID || link.InputId == pin.ID) isConnected = true;
            }

            if (!isConnected && pin.Type != PinType::Flow) {
                ImGui::SameLine();
                ImGui::PushItemWidth(100.0f);
                std::string label = "##" + std::to_string((int)pin.ID.Get());

                if (pin.Type == PinType::Int) {
                    int* v = std::get_if<int>(&pin.Value);
                    ImGui::InputInt(label.c_str(), v, 0);
                }
                else if (pin.Type == PinType::String) {
                    std::string* str = std::get_if<std::string>(&pin.Value);
                    char buffer[256];
                    strncpy(buffer, str->c_str(), sizeof(buffer));
                    if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer))) {
                        *str = buffer;
                    }
                }
                ImGui::PopItemWidth();
            }
            ed::EndPin();
        }

        for (auto& pin : node.Outputs) {
            ImGui::Dummy(ImVec2(0, 2.0f));
            ed::BeginPin(pin.ID, ed::PinKind::Output);

            ImGui::TextUnformatted(pin.Name.c_str());
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Text, GetIconColor(pin.Type).Value);
            ImGui::TextUnformatted("->");
            ImGui::PopStyleColor();

            ed::EndPin();
        }

        ed::EndNode();
    }

    for (auto& link : m_Links) {
        NodePin* outPin = FindPin(link.OutputId);
        ImColor color = outPin ? GetIconColor(outPin->Type) : ImColor(255, 255, 255);
        ed::Link(link.ID, link.InputId, link.OutputId, color, 2.0f);
    }

    if (ed::BeginCreate()) {
        ed::PinId inputPinId, outputPinId;
        if (ed::QueryNewLink(&inputPinId, &outputPinId)) {
            if (inputPinId && outputPinId) {
                NodePin* inPin = FindPin(inputPinId);
                NodePin* outPin = FindPin(outputPinId);

                if (inPin && outPin) {
                    if (inPin->Kind == ed::PinKind::Output) {
                        std::swap(inPin, outPin);
                        std::swap(inputPinId, outputPinId);
                    }

                    bool isSameNode = inPin->NodeID == outPin->NodeID;
                    bool isSameKind = inPin->Kind == outPin->Kind;
                    bool isSameType = inPin->Type == outPin->Type;

                    if (!isSameNode && !isSameKind && isSameType) {
                        if (ed::AcceptNewItem()) {
                            m_Links.push_back({ ed::LinkId(m_NextId++), inputPinId, outputPinId });
                        }
                    } else {
                        ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    }
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
                    [deletedNodeId](const EditorNode& node) { return node.ID == deletedNodeId; }),
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
        ImVec2 canvasPos = ed::ScreenToCanvas(ImGui::GetMousePosOnOpeningCurrentPopup());

        if (ImGui::MenuItem("Add Start Node")) {
            EditorNode node;
            node.ID = m_NextId++;
            node.Name = "Start";
            node.Type = NodeType::Start;
            node.Outputs.push_back(NodePin(m_NextId++, (int)node.ID.Get(), "Out", PinType::Flow, ed::PinKind::Output));
            m_Nodes.push_back(node);
            ed::SetNodePosition(node.ID, canvasPos);
        }

        if (ImGui::MenuItem("Add Action Node")) {
            EditorNode node;
            node.ID = m_NextId++;
            node.Name = "Action";
            node.Type = NodeType::Action;
            node.Inputs.push_back(NodePin(m_NextId++, (int)node.ID.Get(), "In", PinType::Flow, ed::PinKind::Input));
            node.Outputs.push_back(NodePin(m_NextId++, (int)node.ID.Get(), "Out", PinType::Flow, ed::PinKind::Output));
            m_Nodes.push_back(node);
            ed::SetNodePosition(node.ID, canvasPos);
        }

        if (ImGui::MenuItem("Add Branch Node")) {
            EditorNode node;
            node.ID = m_NextId++;
            node.Name = "Branch";
            node.Type = NodeType::Branch;
            node.Inputs.push_back(NodePin(m_NextId++, (int)node.ID.Get(), "In", PinType::Flow, ed::PinKind::Input));
            node.Outputs.push_back(NodePin(m_NextId++, (int)node.ID.Get(), "True", PinType::Flow, ed::PinKind::Output));
            node.Outputs.push_back(NodePin(m_NextId++, (int)node.ID.Get(), "False", PinType::Flow, ed::PinKind::Output));
            m_Nodes.push_back(node);
            ed::SetNodePosition(node.ID, canvasPos);
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