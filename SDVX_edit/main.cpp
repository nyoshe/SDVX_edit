#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "statusBar.h"
#include "editWindow.h"
#include "parser.h"
#include "toolWindow.h"


#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>


int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "voltexEdit");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    editWindow::getInstance();
    editWindow::getInstance().setWindow(&window);
    editWindow::getInstance().loadFile("exh.ksh");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                    if (editWindow::getInstance().editorMeasure + event.mouseWheelScroll.delta * editWindow::getInstance().measuresPerColumn >= 0) {
                        editWindow::getInstance().editorMeasure += event.mouseWheelScroll.delta * editWindow::getInstance().measuresPerColumn;
                    }
                }
            }

            editWindow::getInstance().handleEvent(event);
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }

        }

        
        ImGui::SFML::Update(window, deltaClock.restart());

        //ImGui::ShowDemoWindow();

       

        window.clear();
        statusBar::getInstance().update();
        editWindow::getInstance().update();
        toolWindow::getInstance().update();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}