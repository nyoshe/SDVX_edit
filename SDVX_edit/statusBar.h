#pragma once
#include <iomanip>
#include <string>
#include "EditWindow.h"
#include "FontManager.h"
#include "ScrubBar.h"
#include "Unique.h"
#include "fileDialog/ImGuiFileDialog.h"
#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"

enum LaserSnap
{
	NORMAL,
	FINE,
	VERY_FINE
};

class StatusBar final : public Unique<StatusBar>
{
private:
	sf::RenderWindow* window;

public:
	~StatusBar() = default;
	void setWindow(sf::RenderWindow* window);
	void update();
	LaserSnap laserSnap = NORMAL;
};
