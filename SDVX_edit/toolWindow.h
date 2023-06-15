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
    static toolWindow* instance;
    toolWindow();
    ~toolWindow() = default;
    
    sf::Texture fxIcon;
    sf::Texture btIcon;
    sf::Texture fxHoldIcon;
    sf::Texture btHoldIcon;
    sf::Texture knobRIcon;
    sf::Texture knobLIcon;

public:
    toolWindow(const toolWindow&) = delete;
    toolWindow(toolWindow&&) = delete;
    toolWindow& operator=(const toolWindow&) = delete;
    toolWindow& operator=(toolWindow&&) = delete;
    static toolWindow& getInstance()
    {
        static toolWindow instance;

        return instance;
    }

    void update();
};