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
            if (ImGui::MenuItem("Lane Snapping")) {
                //Do something
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tool")) {
        }
        if (ImGui::BeginMenu("Play")) {
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
            // action
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}