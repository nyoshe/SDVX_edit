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
#include <string>


int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "voltexEdit");
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    sf::Clock deltaClock2;
    int counter = 0;

    statusBar status_bar;
    toolWindow tool_window;
    editWindow::getInstance().setWindow(&window);
    editWindow::getInstance().loadFile("exh.ksh");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            
            

            editWindow::getInstance().handleEvent(event);
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        //ImGui::ShowDemoWindow();

       

        window.clear();
        status_bar.update();
        editWindow::getInstance().update();
        tool_window.update();
        ImGui::SFML::Render(window);
        window.display();
        
        if (counter == 10) {
            float endTime = deltaClock2.restart().asSeconds();
            window.setTitle("voltexEdit | FPS: " + std::to_string(10.0 / endTime));
            counter = 0;
        }
        else {
            counter++;
        }
        
    }

    ImGui::SFML::Shutdown();

    return 0;
}