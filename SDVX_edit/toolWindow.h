#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "fileDialog/ImGuiFileDialog.h"
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>


class toolWindow
{
private:
    static toolWindow* instance;
    toolWindow() = default;
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
        instance.fxIcon.loadFromFile("textures/fxIcon.png");
        instance.btIcon.loadFromFile("textures/btIcon.png");
        instance.fxHoldIcon.loadFromFile("textures/fxHoldIcon.png");
        instance.btHoldIcon.loadFromFile("textures/btHoldIcon.png");
        instance.knobRIcon.loadFromFile("textures/knobLIcon.png");
        instance.knobLIcon.loadFromFile("textures/knobRIcon.png");

        return instance;
    }
    void update();
};