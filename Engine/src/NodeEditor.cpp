#include "../include/NodeEditor.h"

NodeEditor::NodeEditor() {
    ed::Config config;
    config.SettingsFile = "Nella_Nodes.json";
    m_Context = ed::CreateEditor(&config);
}

NodeEditor::~NodeEditor() {
    ed::DestroyEditor(m_Context);
}

void NodeEditor::draw() {
    ImGui::SetNextWindowSize(ImVec2(800.0f, 600.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Nella Visual Scripting");

    ed::SetCurrentEditor(m_Context);

    ed::Begin("My Editor", ImVec2(0.0f, 0.0f));

    int uniqueId = 1;

    ed::BeginNode(uniqueId++);
    ImGui::Text("Node_WhatsApp_Message");

    ed::BeginPin(uniqueId++, ed::PinKind::Input);
    ImGui::Text("-> Trigger");
    ed::EndPin();

    ImGui::SameLine(150);

    ed::BeginPin(uniqueId++, ed::PinKind::Output);
    ImGui::Text("Next ->");
    ed::EndPin();
    ed::EndNode();

    ed::End();
    ed::SetCurrentEditor(nullptr);

    ImGui::End();
}