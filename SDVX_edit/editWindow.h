#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include "parser.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "structs.h"
#include <cmath>




class editWindow
{
private:
    static editWindow* instance;
    editWindow() = default;
    ~editWindow() = default;
    sf::RenderWindow* window = nullptr;
    int topPadding = 50;
    int bottomPadding = 50;
    float width = 0;
    float height = 0;
    float laneWidth = 0;
    float columnWidth = 0;
    float measureHeight = 0;
    std::vector<Measure> measures;
    float mouseX = 0;
    float mouseY = 0;
    sf::Texture btTex;
    sf::Sprite btSprite;

    sf::Texture fxTex;
    sf::Sprite fxSprite;

    sf::Texture btHoldTex;
    sf::Sprite btHoldSprite;

    sf::Texture fxHoldTex;
    sf::Sprite fxHoldSprite;

    Chart chart;
public:

    //functions

    editWindow(const editWindow&) = delete;
    editWindow(editWindow&&) = delete;
    editWindow& operator=(const editWindow&) = delete;
    editWindow& operator=(editWindow&&) = delete;
    static editWindow& getInstance()
    {
        static editWindow instance;
        return instance;
    }
    void setWindow(sf::RenderWindow* _window);
    void update();
    void drawMap();
    void drawChart();
    int getMouseLane();
    int getMouseMeasure();
    int getMouseLine();
    float getLaserX(float pos, bool wide);
    sf::Vector2f getMeasureStart(int measure);
    sf::Vector2f getSnappedPos(NoteType type);
    //get the note location from measure, lane, and line position given a snapping size
    sf::Vector2f getNoteLocation(int measure, int lane, int line, int snapSize);
    void loadFile(std::string fileName);

    //vars

    int editorMeasure = 0;
    int columns = 6;
    int measuresPerColumn = 4;
    int snapGridSize = 16;
};