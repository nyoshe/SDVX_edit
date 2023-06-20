#include "toolWindow.h"


toolWindow::toolWindow() {
	selectIcon.loadFromFile("textures/selectIcon.png");
	fxIcon.loadFromFile("textures/fxIcon.png");
	btIcon.loadFromFile("textures/btIcon.png");
	fxHoldIcon.loadFromFile("textures/fxHoldIcon.png");
	btHoldIcon.loadFromFile("textures/btHoldIcon.png");
	knobRIcon.loadFromFile("textures/knobLIcon.png");
	knobLIcon.loadFromFile("textures/knobRIcon.png");
}


void toolWindow::update() {
	ImGui::Begin("tools");

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::SameLine();
	ImGui::BeginGroup();
	
	if (ImGui::ImageButton(selectIcon, ImVec2(iconSize, iconSize))) {
		EditWindow::getInstance().select = !EditWindow::getInstance().select;
	}
	if (ImGui::ImageButton(fxIcon, ImVec2(iconSize, iconSize))) {
		EditWindow::getInstance().tool = ToolType::FX;
	}
	if (ImGui::ImageButton(fxHoldIcon, ImVec2(iconSize, iconSize))) {

	}
	
	if (ImGui::ImageButton(btIcon, ImVec2(iconSize, iconSize))) {
		EditWindow::getInstance().tool = ToolType::BT;
	}
	if (ImGui::ImageButton(btHoldIcon, ImVec2(iconSize, iconSize))) {

	}

	if (ImGui::ImageButton(knobRIcon, ImVec2(iconSize, iconSize))) {
		EditWindow::getInstance().tool = ToolType::LASER_R;
	}
	if (ImGui::ImageButton(knobLIcon, ImVec2(iconSize, iconSize))) {
		EditWindow::getInstance().tool = ToolType::LASER_L;
	}

	ImGui::EndGroup();
	ImGui::PopStyleColor(1);
	ImGui::End();
}