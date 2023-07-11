#pragma once
#include <string>
#include <plog/Log.h>
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
	init(plog::debug, "logs/log.txt", 1000000, 3);
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

		std::vector<sf::Vector2f> points = {{200, 300}, {500, 300}, {800, 500}, {1000, 1200}}; // Example points
		int numPoints = 100;

		std::vector<sf::Vector2f> bezierCurve = calculateBezierCurve(points, numPoints);

		sf::VertexArray curve(sf::LineStrip);

		sf::VertexArray ctrlPoints(sf::LineStrip);

		for (auto point : bezierCurve) {
			curve.append(sf::Vertex(point, sf::Color::Red));
		}
		for (auto point : points) {
			ctrlPoints.append(sf::Vertex(point, sf::Color::Green));
		}

		//curve.append(sf::Vertex({0,0}, sf::Color::Red));
		window.draw(curve);

		window.draw(ctrlPoints);

		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}
