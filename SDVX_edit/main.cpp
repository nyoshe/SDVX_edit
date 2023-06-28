#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "statusBar.h"
#include "editWindow.h"
#include "parser.h"
#include "toolWindow.h"
#include "toolBar.h"
#include "scrubBar.h"
#include "Input.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <string>
#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

int main() {
	plog::init(plog::debug, "logs/log.txt", 1000000, 3);
	PLOG_INFO << "initialized logging";
	sf::RenderWindow window(sf::VideoMode(1200, 900), "voltexEdit");
	ImGui::SFML::Init(window);
	//IMGUI_IMPL_API

	sf::Clock deltaClock;
	sf::Clock deltaClock2;
	int counter = 0;
	ScrubBar::instance().setWindow(&window);
	StatusBar status_bar;
	//toolWindow tool_window;
	ToolBar tool_bar(&window);
	EditWindow::instance().setWindow(&window);
	//std::string filePath = "C:\\Users\\niayo\\source\\repos\\SDVX_edit\\SDVX_edit\\";
   // std::string filePathName = "C:\\Users\\niayo\\source\\repos\\SDVX_edit\\SDVX_edit\\exh.ksh";
	//EditWindow::instance().loadFile(filePath, filePathName);

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
				//ImGui::PopFont();
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

				window.setView(view);
				EditWindow::instance().setWindow(&window);
				ScrubBar::instance().setWindow(&window);
			}
		}

		//ImGui::ShowDemoWindow();

		status_bar.update();
		EditWindow::instance().update();
		//tool_window.update();
		tool_bar.update();
		ScrubBar::instance().update();

		if (counter == 10) {
			float endTime = deltaClock2.restart().asSeconds();
			window.setTitle("voltexEdit | FPS: " + std::to_string(10.0 / endTime));
			counter = 0;
		}
		else {
			counter++;
		}

		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}