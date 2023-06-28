#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include "editWindow.h"

const int snapSizes[10] = { 4, 8, 12, 16, 24, 32, 48, 64, 96, 192 };

class ToolBar
{
private:
	sf::Texture selectIcon;
	sf::Texture fxIcon;
	sf::Texture btIcon;
	sf::Texture fxHoldIcon;
	sf::Texture btHoldIcon;
	sf::Texture knobRIcon;
	sf::Texture knobLIcon;
	ImFont* font;

public:
	sf::RenderWindow* window;
	int snapSelection;
	float iconSize = 50;
	float iconScale = 0.05;
	ToolBar(sf::RenderWindow* _window);
	~ToolBar() = default;
	void update();
};
