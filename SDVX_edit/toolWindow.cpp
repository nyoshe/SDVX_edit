#include "toolWindow.h"

void toolWindow::update() {
	ImGui::Begin("tools");

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::SameLine();
	ImGui::BeginGroup();
	
	ImGui::ImageButton(fxIcon, ImVec2(100, 100));
	ImGui::ImageButton(fxHoldIcon, ImVec2(100, 100));
	
	ImGui::ImageButton(btIcon, ImVec2(100, 100));
	ImGui::ImageButton(btHoldIcon, ImVec2(100, 100));

	ImGui::ImageButton(knobRIcon, ImVec2(100, 100));
	ImGui::ImageButton(knobLIcon, ImVec2(100, 100));

	ImGui::EndGroup();
	ImGui::PopStyleColor(1);
	ImGui::End();
}