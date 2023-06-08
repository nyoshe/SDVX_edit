#include "statusBar.h"

void statusBar::update()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New"))
            {
                //Do something
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}