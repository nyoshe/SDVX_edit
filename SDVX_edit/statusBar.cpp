#include "statusBar.h"

void StatusBar::update()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                Chart newChart;
                EditWindow::getInstance().chart = newChart;
            }
            if (ImGui::MenuItem("Open")) {
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".ksh", ".");
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                    // action
                }
            }
            if (ImGui::MenuItem("Save")) {
                //Do something
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::BeginMenu(("Measures per column: " + std::to_string(EditWindow::getInstance().measuresPerColumn)).c_str())) {
                for (int i = 1; i < 10; i++) {
                    if (ImGui::MenuItem(std::to_string(i).c_str())) {
                        EditWindow::getInstance().measuresPerColumn = i;
                        EditWindow::getInstance().updateVars();
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tool")) {
            if (ImGui::BeginMenu("Lane Snapping")) {
                if (ImGui::MenuItem("1/4")) { 
                    EditWindow::getInstance().snapGridSize = 4;
                }
                if (ImGui::MenuItem("1/8")) {
                    EditWindow::getInstance().snapGridSize = 8;
                }
                if (ImGui::MenuItem("1/12")) {
                    EditWindow::getInstance().snapGridSize = 12;
                }
                if (ImGui::MenuItem("1/16")) {
                    EditWindow::getInstance().snapGridSize = 16;
                }
                if (ImGui::MenuItem("1/24")) {
                    EditWindow::getInstance().snapGridSize = 24;
                }
                if (ImGui::MenuItem("1/32")) {
                    EditWindow::getInstance().snapGridSize = 32;
                }
                if (ImGui::MenuItem("1/48")) {
                    EditWindow::getInstance().snapGridSize = 48;
                }
                if (ImGui::MenuItem("1/64")) {
                    EditWindow::getInstance().snapGridSize = 64;
                }
                if (ImGui::MenuItem("1/96")) {
                    EditWindow::getInstance().snapGridSize = 96;
                }
                if (ImGui::MenuItem("1/192")) {
                    EditWindow::getInstance().snapGridSize = 192;
                }
                //Do something
                ImGui::EndMenu();
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
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            EditWindow::getInstance().mapFilePath = filePath;
            EditWindow::getInstance().loadFile(filePathName);
            // action
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}