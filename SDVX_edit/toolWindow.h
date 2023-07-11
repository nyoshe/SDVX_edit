#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include "EditWindow.h"
#include "Enums.h"
#include "fileDialog/ImGuiFileDialog.h"
#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"

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
