#include "statusBar.h"
StatusBar::StatusBar() {
    ImGuiIO& io = ImGui::GetIO();
    font = io.Fonts->AddFontFromFileTTF("Fonts/CONSOLA.TTF", 16);
}
void StatusBar::update()
{
    ImGui::PushFont(font);
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                Chart newChart;
                EditWindow::getInstance().chart = newChart;
            }
            if (ImGui::MenuItem("Open")) {
                ImGuiFileDialog::Instance()->OpenDialog("OpenFileDlgKey", "Open", ".ksh", ".");
            }
            if (ImGui::MenuItem("Save")) {
                EditWindow::getInstance().saveFile();
            }
            if (ImGui::MenuItem("Save As")) {
                ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Save As", ".ksh", ".");
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::BeginMenu(("Measures per column: " + std::to_string(EditWindow::getInstance().measuresPerColumn)).c_str())) {
                for (int i = 1; i < 10; i++) {
                    if (ImGui::MenuItem(std::to_string(i).c_str(), NULL, i == EditWindow::getInstance().measuresPerColumn)) {
                        EditWindow::getInstance().measuresPerColumn = i;
                        EditWindow::getInstance().updateVars();
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(("columns: " + std::to_string(EditWindow::getInstance().columns)).c_str())) {
                for (int i = 1; i < 10; i++) {
                    if (ImGui::MenuItem(std::to_string(i).c_str(), NULL, i == EditWindow::getInstance().columns)) {
                        EditWindow::getInstance().columns = i;
                        EditWindow::getInstance().updateVars();
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Show Scrub Bar", NULL, ScrubBar::getInstance().enabled)) {
                ScrubBar::getInstance().enabled = !ScrubBar::getInstance().enabled;
            }


            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tool")) {

            if (ImGui::BeginMenu("Lane Snapping")) {
                
                if (ImGui::MenuItem("1/4", NULL, EditWindow::getInstance().snapGridSize == 4)) {
                    EditWindow::getInstance().snapGridSize = 4;
                }
                if (ImGui::MenuItem("1/8", NULL, EditWindow::getInstance().snapGridSize == 8)) {
                    EditWindow::getInstance().snapGridSize = 8;
                }
                if (ImGui::MenuItem("1/12", NULL, EditWindow::getInstance().snapGridSize == 12)) {
                    EditWindow::getInstance().snapGridSize = 12;
                }
                if (ImGui::MenuItem("1/16", NULL, EditWindow::getInstance().snapGridSize == 16)) {
                    EditWindow::getInstance().snapGridSize = 16;
                }
                if (ImGui::MenuItem("1/24", NULL, EditWindow::getInstance().snapGridSize == 24)) {
                    EditWindow::getInstance().snapGridSize = 24;
                }
                if (ImGui::MenuItem("1/32", NULL, EditWindow::getInstance().snapGridSize == 32)) {
                    EditWindow::getInstance().snapGridSize = 32;
                }
                if (ImGui::MenuItem("1/48", NULL, EditWindow::getInstance().snapGridSize == 48)) {
                    EditWindow::getInstance().snapGridSize = 48;
                }
                if (ImGui::MenuItem("1/64", NULL, EditWindow::getInstance().snapGridSize == 64)) {
                    EditWindow::getInstance().snapGridSize = 64;
                }
                if (ImGui::MenuItem("1/96", NULL, EditWindow::getInstance().snapGridSize == 96)) {
                    EditWindow::getInstance().snapGridSize = 96;
                }
                if (ImGui::MenuItem("1/192", NULL, EditWindow::getInstance().snapGridSize == 192)) {
                    EditWindow::getInstance().snapGridSize = 192;
                }
                //Do something
                ImGui::EndMenu();
            }
            ImGui::Separator();
            
            if (ImGui::MenuItem("Laser Placement: Normal", NULL, laserSelection & NORMAL)) {
                laserSelection = NORMAL;
                EditWindow::getInstance().laserMoveSize = 5;
            }
            if (ImGui::MenuItem("Laser Placement: Fine", NULL, laserSelection & FINE)) {
                laserSelection = FINE;
                EditWindow::getInstance().laserMoveSize = 3;
            }
            if (ImGui::MenuItem("Laser Placement: Very Fine", NULL, laserSelection & VERY_FINE)) {
                laserSelection = VERY_FINE;
                EditWindow::getInstance().laserMoveSize = 1;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Play")) {
            if (ImGui::MenuItem("half speed")) {
                EditWindow::getInstance().player.setSpeed(0.5);
            }
            if (ImGui::MenuItem("full speed")) {
                EditWindow::getInstance().player.setSpeed(1.0);
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
            EditWindow::getInstance().loadFile(filePath, filePathName);
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
            EditWindow::getInstance().saveFile(filePathName);
            // action
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
    ImGui::PopFont();
}