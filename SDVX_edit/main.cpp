#pragma once
#include <string>
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include "bezier.h"
#include "EditWindow.h"
#include "FontManager.h"
#include "Input.h"
#include "Parser.h"
#include "ScrubBar.h"
#include "StatusBar.h"
#include "ToolBar.h"
#include "ToolWindow.h"
#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"
#include "plog/Initializers/RollingFileInitializer.h"


int main()
{
	static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
	init(plog::verbose, "logs/log.txt", 1000000, 3).addAppender(&consoleAppender);

	PLOG_INFO << "initialized logging";
	sf::RenderWindow window(sf::VideoMode(1200, 900), "voltexEdit");
	ImGui::SFML::Init(window);

	sf::Clock deltaClock;
	FontManager::instance().setWindow(&window);
	ScrubBar::instance().setWindow(&window);
	StatusBar::instance().setWindow(&window);
	//toolWindow tool_window;
	ToolBar::instance().setWindow(&window);
	EditWindow::instance().setWindow(&window);
	std::string filePath = "C:\\Users\\niayo\\source\\repos\\SDVX_edit\\SDVX_edit\\";
	std::string filePathName = "C:\\Users\\niayo\\source\\repos\\SDVX_edit\\SDVX_edit\\exh.ksh";
	EditWindow::instance().loadFile(filePath, filePathName);

	while (window.isOpen()) {
		sf::Event event;
		window.clear();

		ImGui::SFML::Update(window, deltaClock.restart());

		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(window, event);
			Input::instance().handleEvent(event);
			EditWindow::instance().handleEvent(event);

			if (event.type == sf::Event::Closed) {
				EditWindow::instance().saveFile("editor_backup.ksh");
				window.close();
			}
			else if (event.type == sf::Event::Resized) {
				sf::View view = window.getDefaultView();
				// resize my view
				view.setSize({
					static_cast<float>(event.size.width),
					static_cast<float>(event.size.height)
				});
				view.setCenter({
					static_cast<float>(event.size.width / 2),
					static_cast<float>(event.size.height / 2)
				});
				//ImGui::SFML::Render(window);
				window.setView(view);
				FontManager::instance().setWindow(&window);
				StatusBar::instance().setWindow(&window);
				EditWindow::instance().setWindow(&window);
				ScrubBar::instance().setWindow(&window);
				ToolBar::instance().setWindow(&window);
				//ImGui::NewFrame();
			}
		}

		//ImGui::ShowDemoWindow();
		StatusBar::instance().update();
		FontManager::instance().update();
		EditWindow::instance().update();
		ToolBar::instance().update();
		ScrubBar::instance().update();
		
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}
