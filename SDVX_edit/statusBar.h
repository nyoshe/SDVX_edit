#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "editWindow.h"
#include "fileDialog/ImGuiFileDialog.h"
#include <string>
class StatusBar
{
private:
    

public:
    StatusBar() = default;
    ~StatusBar() = default;
    void update();
};