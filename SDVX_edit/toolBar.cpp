#include "toolBar.h"

ToolBar::ToolBar(sf::RenderWindow* _window) {
	selectIcon.loadFromFile("textures/selectIcon.png");
	fxIcon.loadFromFile("textures/fxIcon.png");
	btIcon.loadFromFile("textures/btIcon.png");
	fxHoldIcon.loadFromFile("textures/fxHoldIcon.png");
	btHoldIcon.loadFromFile("textures/btHoldIcon.png");
	knobRIcon.loadFromFile("textures/knobLIcon.png");
	knobLIcon.loadFromFile("textures/knobRIcon.png");
	window = _window;
	ImGuiIO& io = ImGui::GetIO();
	font = io.Fonts->AddFontFromFileTTF("Fonts/CONSOLA.TTF", 30);
	//ImGui::SFML::UpdateFontTexture();
	ImGui::SFML::UpdateFontTexture();
}

void ToolBar::update() {
	int styleChanges = 0;

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	//window_flags |= ImGuiWindowFlags_NoNav;
	//window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::SetNextWindowPos(ImVec2(0, window->getSize().y - iconScale * 1.3 * window->getSize().y));
	ImGui::SetNextWindowSize(ImVec2(window->getSize().x, iconScale * 1.5 * window->getSize().y));
	iconSize = iconScale * window->getSize().y;

	//ImGui::Begin("tools", NULL, window_flags);
	//ImGui::Text("test");
	//ImGui::End();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(iconSize * 0.1, iconSize * 0.1));

	if (ImGui::Begin("tools", NULL, window_flags)) {
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + iconSize * 3);
		std::string buttonText = std::to_string(EditWindow::instance().snapGridSize);

		ImGui::PushFont(font);
		ImGui::Button(buttonText.c_str(), ImVec2(iconSize * 1.1, iconSize * 1.1));
		if (ImGui::IsItemHovered()) {
			if (ImGui::GetIO().MouseClicked[ImGuiMouseButton_Right]) {
				snapSelection = snapSelection == 0 ? 9 : snapSelection - 1;
				EditWindow::instance().snapGridSize = snapSizes[snapSelection];
			}
			else if (ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left]) {
				snapSelection = (snapSelection + 1) % 10;
				EditWindow::instance().snapGridSize = snapSizes[snapSelection];
			}
		}
		ImGui::PopFont();

		ImGui::SameLine();
		if (EditWindow::instance().select) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.200f, 0.410f, 0.680f, 1.000f));
			styleChanges++;
		}
		if (ImGui::ImageButton(selectIcon, ImVec2(iconSize, iconSize))) {
			EditWindow::instance().select = !EditWindow::instance().select;
		}
		if (styleChanges) ImGui::PopStyleColor(styleChanges--);

		ImGui::SameLine();
		if (EditWindow::instance().tool == ToolType::FX) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.200f, 0.410f, 0.680f, 1.000f));
			styleChanges++;
		}
		if (ImGui::ImageButton(fxIcon, ImVec2(iconSize, iconSize))) {
			EditWindow::instance().tool = ToolType::FX;
		}
		if (styleChanges) ImGui::PopStyleColor(styleChanges--);

		ImGui::SameLine();
		if (EditWindow::instance().tool == ToolType::BT) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.200f, 0.410f, 0.680f, 1.000f));
			styleChanges++;
		}
		if (ImGui::ImageButton(btIcon, ImVec2(iconSize, iconSize))) {
			EditWindow::instance().tool = ToolType::BT;
		}
		if (styleChanges) ImGui::PopStyleColor(styleChanges--);

		ImGui::SameLine();
		if (EditWindow::instance().tool == ToolType::LASER_R) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.200f, 0.410f, 0.680f, 1.000f));
			styleChanges++;
		}
		if (ImGui::ImageButton(knobRIcon, ImVec2(iconSize, iconSize))) {
			EditWindow::instance().tool = ToolType::LASER_R;
		}
		if (styleChanges) ImGui::PopStyleColor(styleChanges--);

		ImGui::SameLine();
		if (EditWindow::instance().tool == ToolType::LASER_L) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.200f, 0.410f, 0.680f, 1.000f));
			styleChanges++;
		}
		if (ImGui::ImageButton(knobLIcon, ImVec2(iconSize, iconSize))) {
			EditWindow::instance().tool = ToolType::LASER_L;
		}
		if (styleChanges) ImGui::PopStyleColor(styleChanges--);

		ImGui::End();
	}

	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar(1);
}