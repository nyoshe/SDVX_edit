#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "fileDialog/ImGuiFileDialog.h"
#include <string>
#include <iostream>
#include "editWindow.h"
#include "enums.h"
#include <SFML/Graphics.hpp>


class toolWindow
{
private:
    sf::Texture selectIcon;
    sf::Texture fxIcon;
    sf::Texture btIcon;
    sf::Texture fxHoldIcon;
    sf::Texture btHoldIcon;
    sf::Texture knobRIcon;
    sf::Texture knobLIcon;

public:
    float iconSize = 50;
    toolWindow();
    ~toolWindow() = default;
    void update();
};