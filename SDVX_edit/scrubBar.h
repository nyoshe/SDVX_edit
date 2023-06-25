#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include "editWindow.h"
#include "Unique.h"
class ScrubBar final : public Unique <ScrubBar>
{
public:
	ScrubBar() = default;
	~ScrubBar() = default;

	bool enabled = false;
	int displayMeasures = 100;
	int currentMeasure = 0;
	sf::RenderTarget* window = nullptr;
	sf::RenderTexture tex;
	sf::Sprite displaySpr;
	float yScale = 0.1;
	float topPadding = 0.03;
	void update();
	void setWindow(sf::RenderWindow* _window);
};

