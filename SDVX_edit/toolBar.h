#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include "editWindow.h"
#include "fontManager.h"

const int snapSizes[10] = { 4, 8, 12, 16, 24, 32, 48, 64, 96, 192 };

class ToolBar final : public Unique <ToolBar>
{
private:
	sf::Texture selectIcon;
	sf::Texture fxIcon;
	sf::Texture btIcon;
	sf::Texture fxHoldIcon;
	sf::Texture btHoldIcon;
	sf::Texture knobRIcon;
	sf::Texture knobLIcon;
	ImVec4 hoverColor = ImVec4(0.200f, 0.410f, 0.680f, 1.000f);

public:
	sf::RenderWindow* window;
	int snapSelection;
	float iconSize = 50;
	float iconScale = 1.0;
	ToolBar();
	void setWindow(sf::RenderWindow* _window);
	~ToolBar() = default;
	void update();
};
