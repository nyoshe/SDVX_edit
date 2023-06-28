#pragma once
#include "imgui.h"
#include "imgui-SFML.h"

class MouseInfo
{
public:
    int mouseDownLine = 0;
    int mouseUpLine = 0;
    int mouseDownLane = 0;
    int mouseUpLane = 0;
    int mouseDownLaserPos = 0;
    int mouseUpLaserPos = 0;
    int mouseLaserPos = 0;
    int mouseLane = 0;
    int mouseLine = 0;

    bool hoveredWindow = false;
    void update() {
        hoveredWindow = !(ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow));
    }
};

