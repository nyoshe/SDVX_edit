#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "statusBar.h"
#include "editWindow.h"
#include "parser.h"
#include "toolWindow.h"
#include "toolBar.h"
#include "scrubBar.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <string>


int main() {
    
    sf::RenderWindow window(sf::VideoMode(1200, 900), "voltexEdit");
    ImGui::SFML::Init(window);
    IMGUI_IMPL_API
    sf::Clock deltaClock;
    sf::Clock deltaClock2;
    int counter = 0;

    ScrubBar scrub_bar;
    scrub_bar.setWindow(&window);
    StatusBar status_bar;
    //toolWindow tool_window;
    ToolBar tool_bar(&window);
    EditWindow::getInstance().setWindow(&window);
    std::string filePath = "C:\\Users\\niayo\\source\\repos\\SDVX_edit\\SDVX_edit\\";
    std::string filePathName = "C:\\Users\\niayo\\source\\repos\\SDVX_edit\\SDVX_edit\\exh.ksh";
    EditWindow::getInstance().loadFile(filePath, filePathName);


    while (window.isOpen()) {
        sf::Event event;
        window.clear();

        while (window.pollEvent(event)) {
            
            

            EditWindow::getInstance().handleEvent(event);
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
                //ImGui::PopFont();
            }
        }
        
        ImGui::SFML::Update(window, deltaClock.restart());
        //ImGui::ShowDemoWindow();


        status_bar.update();
        EditWindow::getInstance().update();
        //tool_window.update();
        tool_bar.update();
        

        
        
        if (counter == 10) {
            //scrub_bar.update();
            
            float endTime = deltaClock2.restart().asSeconds();
            window.setTitle("voltexEdit | FPS: " + std::to_string(10.0 / endTime));
            counter = 0;
        }
        else {
            counter++;
        }
        //scrub_bar.draw();
        ImGui::SFML::Render(window);
        window.display();
        
    }

    ImGui::SFML::Shutdown();

    return 0;
}