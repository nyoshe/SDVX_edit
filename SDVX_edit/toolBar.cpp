#include "ToolBar.h"

ToolBar::ToolBar()
{
	selectIcon.loadFromFile("textures/selectIcon.png");
	fxIcon.loadFromFile("textures/fxIcon.png");
	btIcon.loadFromFile("textures/btIcon.png");
	fxHoldIcon.loadFromFile("textures/fxHoldIcon.png");
	btHoldIcon.loadFromFile("textures/btHoldIcon.png");
	knobRIcon.loadFromFile("textures/knobRIcon.png");
	knobLIcon.loadFromFile("textures/knobLIcon.png");
	FontManager::instance().addFont("snapFont", 2.0);
}

void ToolBar::setWindow(sf::RenderWindow* _window)
{
	window = _window;
}

void ToolBar::update()
{
	int styleChanges = 0;
	float iconScale = 1.0;
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	iconSize = iconScale * 50 * FontManager::instance().getScale();
	ImGui::SetNextWindowPos(ImVec2(0, window->getSize().y - iconSize * 1.3));
	ImGui::SetNextWindowSize(ImVec2(window->getSize().x, iconSize * 1.5));


	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(iconSize * 0.1, iconSize * 0.1));

	if (ImGui::Begin("tools", nullptr, window_flags)) {
		ImGui::SameLine();
		//ImGui::SetCursorPosX();
		std::string buttonText = std::to_string(EditWindow::instance().snapGridSize);

		ImGui::PushFont(FontManager::instance().getFont("snapFont"));
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
		if (EditWindow::instance().tool.select) {
			ImGui::PushStyleColor(ImGuiCol_Button, hoverColor);
			styleChanges++;
		}
		if (ImGui::ImageButton(selectIcon, ImVec2(iconSize, iconSize))) {
			EditWindow::instance().tool.select = !EditWindow::instance().tool.select;
		}
		if (styleChanges) {
			ImGui::PopStyleColor(styleChanges--);
		}


		ImGui::SameLine();
		if (EditWindow::instance().tool.type == FX) {
			ImGui::PushStyleColor(ImGuiCol_Button, hoverColor);
			styleChanges++;
		}
		if (ImGui::ImageButton(fxIcon, ImVec2(iconSize, iconSize))) {
			EditWindow::instance().tool.type = FX;
		}
		if (styleChanges) {
			ImGui::PopStyleColor(styleChanges--);
		}

		ImGui::SameLine();
		if (EditWindow::instance().tool.type == BT) {
			ImGui::PushStyleColor(ImGuiCol_Button, hoverColor);
			styleChanges++;
		}
		if (ImGui::ImageButton(btIcon, ImVec2(iconSize, iconSize))) {
			EditWindow::instance().tool.type = BT;
		}
		if (styleChanges) {
			ImGui::PopStyleColor(styleChanges--);
		}

		ImGui::SameLine();
		if (EditWindow::instance().tool.type == LASER_R) {
			ImGui::PushStyleColor(ImGuiCol_Button, hoverColor);
			styleChanges++;
		}
		if (ImGui::ImageButton(knobRIcon, ImVec2(iconSize, iconSize))) {
			EditWindow::instance().tool.type = LASER_R;
		}
		if (styleChanges) {
			ImGui::PopStyleColor(styleChanges--);
		}

		ImGui::SameLine();
		if (EditWindow::instance().tool.type == LASER_L) {
			ImGui::PushStyleColor(ImGuiCol_Button, hoverColor);
			ImGui::SameLine();
			styleChanges++;
		}
		if (ImGui::ImageButton(knobLIcon, ImVec2(iconSize, iconSize))) {
			EditWindow::instance().tool.type = LASER_L;
		}
		if (EditWindow::instance().tool.type == LASER_L || EditWindow::instance().tool.type == LASER_R) {
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMax().x + iconSize * 0.3,
			                               window->getSize().y - iconSize * 1.3));

			if (ImGui::BeginChild(
				"options", ImVec2(ImGui::GetContentRegionAvail().x, iconScale * 1.5 * window->getSize().y), true,
				window_flags)) {
				static bool wide = false;
				if (ImGui::Checkbox("Wide Laser", &wide)) {
					EditWindow::instance().tool.wideLaser = wide;
				}
				//ImGui::SameLine();
				static bool startAtZero = false;
				if (ImGui::Checkbox("default start", &startAtZero)) {
					EditWindow::instance().tool.laserDefaultStart = startAtZero;
				}
			}
			ImGui::EndChild();
		}
		if (styleChanges) {
			ImGui::PopStyleColor(styleChanges--);
		}
		ImGui::End();
	}

	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar(1);
}
