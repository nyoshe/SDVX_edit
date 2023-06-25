#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "editWindow.h"
#include "fileDialog/ImGuiFileDialog.h"
#include "scrubBar.h"
#include <string>

enum LaserSnap {
    NORMAL, 
    FINE, 
    VERY_FINE
};


class StatusBar
{
private:
    ImFont* font;

public:
    StatusBar();
    ~StatusBar() = default;
    void update();
    LaserSnap laserSnap = LaserSnap::NORMAL;
};