#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include "editWindow.h"

class ScrubBar
{
private:
	ScrubBar() = default;
	~ScrubBar() = default;
	ScrubBar* instance;
public:
	ScrubBar(const ScrubBar&) = delete;
	ScrubBar(ScrubBar&&) = delete;
	ScrubBar& operator=(const ScrubBar&) = delete;
	ScrubBar& operator=(ScrubBar&&) = delete;
	static ScrubBar& getInstance()
	{
		static ScrubBar instance;
		return instance;
	}

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

