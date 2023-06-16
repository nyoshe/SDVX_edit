#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "editWindow.h"
#include "fileDialog/ImGuiFileDialog.h"
#include <string>
class statusBar
{
private:
    

public:
    statusBar() = default;
    ~statusBar() = default;
    void update();
};