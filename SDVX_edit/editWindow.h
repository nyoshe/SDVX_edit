#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include "parser.h"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "structs.h"
#include "audioManager.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <stack>


#include <boost/interprocess/managed_shared_memory.hpp>
#include <cstdlib> //std::system
#include <sstream>

struct gameControl
{
    bool paused = true;
    bool seek = false;
    uint32_t seekPos = 0;
    float speed = 1.0;
};



class EditWindow
{
private:
    static EditWindow* instance;
    EditWindow() = default;
    ~EditWindow() = default;
    
    void drawLineButtons(ChartLine* line);
    sf::RenderWindow* window = nullptr;
    int topPadding = 50;
    int bottomPadding = 50;
    float width = 0;
    float height = 0;
    float laneWidth = 0;
    float columnWidth = 0;
    float measureHeight = 0;
    unsigned int selectStart = 0;
    unsigned int selectEnd = 0;
    unsigned int playbackPos = 0;
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

    sf::Texture entryTex;
    sf::Sprite entrySprite;

    sf::Sprite toolSprite;

    Chart chart;
public:

    //functions

    EditWindow(const EditWindow&) = delete;
    EditWindow(EditWindow&&) = delete;
    EditWindow& operator=(const EditWindow&) = delete;
    EditWindow& operator=(EditWindow&&) = delete;
    static EditWindow& getInstance()
    {
        static EditWindow instance;
        return instance;
    }
    void setWindow(sf::RenderWindow* _window);
    void update();
    void updateVars();
    void drawMap();
    void drawChart();
    int getMouseLane();
    int getMouseMeasure();
    int getMouseLine();
    int getMouseLine(int snapSize);
    int getMouseGlobalLine();
    int getMeasureFromGlobal(unsigned int loc);


    void handleEvent(sf::Event event);
    std::vector <float> getLaserX(ChartLine* line);
    sf::Vector2f getMeasureStart(int measure);
    sf::Vector2f getSnappedPos(ToolType type);
    //get the note location from measure, lane, and line position given a snapping size
    sf::Vector2f getNoteLocation(int measure, int lane, int line);
    void loadFile(std::string fileName);

    //vars
    boost::interprocess::managed_shared_memory memSegment;
    int editorMeasure = 0;
    int columns = 6;
    int measuresPerColumn = 4;
    int snapGridSize = 16;
    float playbackSpeed = 1.0;
    AudioManager player;
    std::string mapFilePath;

    ToolType tool = ToolType::BT;

    gameControl* controlPtr;
    
};