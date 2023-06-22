#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include "editWindow.h"

class ScrubBar
{
public:
	sf::RenderTarget* window = nullptr;
	sf::RenderTexture tex;
	void update();
	void draw();
	void setWindow(sf::RenderWindow* _window);
};

