#include "statusBar.h"

void StatusBar::setWindow(sf::RenderWindow* _window) {
	window = _window;
}

void StatusBar::update()
{
	ImGui::PushFont(FontManager::instance().getDefaultFont());
	static bool updatingScale = false;
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New")) {
				Chart newChart;
				EditWindow::instance().chart = newChart;
			}
			if (ImGui::MenuItem("Open")) {
				ImGuiFileDialog::Instance()->OpenDialog("OpenFileDlgKey", "Open", ".ksh", ".");
			}
			if (ImGui::MenuItem("Save")) {
				EditWindow::instance().saveFile();
			}
			if (ImGui::MenuItem("Save As")) {
				ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Save As", ".ksh", ".");
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			if (ImGui::BeginMenu(("Measures per column: " + std::to_string(EditWindow::instance().measuresPerColumn)).c_str())) {
				for (int i = 1; i < 13; i++) {
					if (ImGui::MenuItem(std::to_string(i).c_str(), NULL, i == EditWindow::instance().measuresPerColumn)) {
						EditWindow::instance().measuresPerColumn = i;
						EditWindow::instance().updateVars();
					}
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(("columns: " + std::to_string(EditWindow::instance().columns)).c_str())) {
				for (int i = 4; i < 13; i++) {
					if (ImGui::MenuItem(std::to_string(i).c_str(), NULL, i == EditWindow::instance().columns)) {
						EditWindow::instance().columns = i;
						EditWindow::instance().updateVars();
					}
				}
				ImGui::EndMenu();
			}
			static float scale = 1.0;
			std::ostringstream oss;
			oss << std::setprecision(2) << FontManager::instance().getAbsoluteScale();
			if (ImGui::BeginMenu(("GUI scale: " + oss.str() + "x").c_str())) {
				
				if (ImGui::SliderFloat("scale", &scale, 0.5f, 2.f, "%.2f")) {

				}
				if (ImGui::IsItemHovered()) {
					updatingScale = true;
				}
				else if (updatingScale) {
					FontManager::instance().setScale(scale);
					updatingScale = false;
				}
				
				ImGui::EndMenu();
			}
			
			/*
			if (ImGui::MenuItem("Show Scrub Bar", NULL, ScrubBar::instance().enabled)) {
				ScrubBar::instance().enabled = !ScrubBar::instance().enabled;
			}
			*/
			if (ImGui::MenuItem("Draw Debug", NULL, EditWindow::instance().DEBUG)) {
				EditWindow::instance().DEBUG = !EditWindow::instance().DEBUG;
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tool")) {
			if (ImGui::BeginMenu("Lane Snapping")) {
				if (ImGui::MenuItem("1/4", NULL, EditWindow::instance().snapGridSize == 4)) {
					EditWindow::instance().snapGridSize = 4;
				}
				if (ImGui::MenuItem("1/8", NULL, EditWindow::instance().snapGridSize == 8)) {
					EditWindow::instance().snapGridSize = 8;
				}
				if (ImGui::MenuItem("1/12", NULL, EditWindow::instance().snapGridSize == 12)) {
					EditWindow::instance().snapGridSize = 12;
				}
				if (ImGui::MenuItem("1/16", NULL, EditWindow::instance().snapGridSize == 16)) {
					EditWindow::instance().snapGridSize = 16;
				}
				if (ImGui::MenuItem("1/24", NULL, EditWindow::instance().snapGridSize == 24)) {
					EditWindow::instance().snapGridSize = 24;
				}
				if (ImGui::MenuItem("1/32", NULL, EditWindow::instance().snapGridSize == 32)) {
					EditWindow::instance().snapGridSize = 32;
				}
				if (ImGui::MenuItem("1/48", NULL, EditWindow::instance().snapGridSize == 48)) {
					EditWindow::instance().snapGridSize = 48;
				}
				if (ImGui::MenuItem("1/64", NULL, EditWindow::instance().snapGridSize == 64)) {
					EditWindow::instance().snapGridSize = 64;
				}
				if (ImGui::MenuItem("1/96", NULL, EditWindow::instance().snapGridSize == 96)) {
					EditWindow::instance().snapGridSize = 96;
				}
				if (ImGui::MenuItem("1/192", NULL, EditWindow::instance().snapGridSize == 192)) {
					EditWindow::instance().snapGridSize = 192;
				}
				//Do something
				ImGui::EndMenu();
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Laser Placement: Normal", NULL, laserSnap == LaserSnap::NORMAL)) {
				laserSnap = NORMAL;
				EditWindow::instance().laserMoveSize = 0.1;
			}
			if (ImGui::MenuItem("Laser Placement: Fine", NULL, laserSnap == LaserSnap::FINE)) {
				laserSnap = FINE;
				EditWindow::instance().laserMoveSize = 0.05;
			}
			if (ImGui::MenuItem("Laser Placement: Very Fine", NULL, laserSnap == LaserSnap::VERY_FINE)) {
				laserSnap = VERY_FINE;
				EditWindow::instance().laserMoveSize = 0.02;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Play")) {
			if (ImGui::MenuItem("Playback: 0.25x")) {
				EditWindow::instance().player.setSpeed(0.25);
			}
			if (ImGui::MenuItem("Playback: 0.5x")) {
				EditWindow::instance().player.setSpeed(0.5);
			}
			if (ImGui::MenuItem("Playback: 1.0x")) {
				EditWindow::instance().player.setSpeed(1.0);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	if (ImGuiFileDialog::Instance()->Display("OpenFileDlgKey", NULL, ImVec2(300.0, 200.0)))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			EditWindow::instance().loadFile(filePath, filePathName);
			// action
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
	if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey", NULL, ImVec2(300.0, 200.0)))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			EditWindow::instance().saveFile(filePathName);
			// action
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
	ImGui::PopFont();
}