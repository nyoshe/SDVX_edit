#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "editWindow.h"
#include "fileDialog/ImGuiFileDialog.h"
#include "scrubBar.h"
#include "fontManager.h"
#include <string>
#include "Unique.h"
#include <iomanip>
enum LaserSnap {
	NORMAL,
	FINE,
	VERY_FINE
};
	
class StatusBar final : public Unique <StatusBar>
{
private:
	sf::RenderWindow* window;

public:
	~StatusBar() = default;
	void setWindow(sf::RenderWindow* window);
	void update();
	LaserSnap laserSnap = LaserSnap::NORMAL;
};