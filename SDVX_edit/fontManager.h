#pragma once
#include <map>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "Unique.h"
#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"

struct FontInfo
{
	std::string file = "Fonts/CONSOLA.TTF";
	ImFont* font = nullptr;
	float scale = 0;
};

class FontManager final : public Unique<FontManager>
{
	sf::RenderWindow* window;
	float scale = 1.0;
	float absoluteScale = 1.0;
	std::map<std::string, FontInfo> fontMap;
	bool needsUpdate = false;
	std::string defaultFontFile = "Fonts/CONSOLA.TTF";
	void loadDefault();

public:
	FontManager();
	~FontManager() = default;
	void setWindow(sf::RenderWindow* _window);
	void setScale(float _scale);
	float getScale();
	float getAbsoluteScale();
	void addFont(std::string id, std::string file, float scale);
	void addFont(std::string id, float scale);
	void updateFonts();
	ImFont* getFont(std::string id);
	ImFont* getDefaultFont();
	void update();
};
