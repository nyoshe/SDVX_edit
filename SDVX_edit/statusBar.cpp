#include "statusBar.h"

void statusBar::update()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                //Do something
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
            if (ImGui::BeginMenu(("Measures per column: " + std::to_string(editWindow::getInstance().measuresPerColumn)).c_str())) {
                for (int i = 1; i < 10; i++) {
                    if (ImGui::MenuItem(std::to_string(i).c_str())) {
                        //Do something
                        editWindow::getInstance().measuresPerColumn = i;
                        editWindow::getInstance().updateVars();
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tool")) {
            if (ImGui::BeginMenu("Lane Snapping")) {
                if (ImGui::MenuItem("1/4")) { 
                    editWindow::getInstance().snapGridSize = 4;
                }
                if (ImGui::MenuItem("1/8")) {
                    editWindow::getInstance().snapGridSize = 8;
                }
                if (ImGui::MenuItem("1/12")) {
                    editWindow::getInstance().snapGridSize = 12;
                }
                if (ImGui::MenuItem("1/16")) {
                    editWindow::getInstance().snapGridSize = 16;
                }
                if (ImGui::MenuItem("1/24")) {
                    editWindow::getInstance().snapGridSize = 24;
                }
                if (ImGui::MenuItem("1/32")) {
                    editWindow::getInstance().snapGridSize = 32;
                }
                if (ImGui::MenuItem("1/48")) {
                    editWindow::getInstance().snapGridSize = 48;
                }
                if (ImGui::MenuItem("1/64")) {
                    editWindow::getInstance().snapGridSize = 64;
                }
                if (ImGui::MenuItem("1/96")) {
                    editWindow::getInstance().snapGridSize = 96;
                }
                if (ImGui::MenuItem("1/192")) {
                    editWindow::getInstance().snapGridSize = 192;
                }
                //Do something
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        //if (ImGui::BeginMenu("Play")) {
        //}
        
        ImGui::EndMainMenuBar();
    }
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}