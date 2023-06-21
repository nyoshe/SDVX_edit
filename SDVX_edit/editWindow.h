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

#define DEBUG false

typedef std::vector<std::pair<ChartLine*, std::vector<sf::VertexArray>>> QuadArray;
typedef std::map<unsigned int, ChartLine*>::iterator LineIterator;

class EditWindow
{
private:
    static EditWindow* instance;
    EditWindow() = default;
    ~EditWindow() = default;
    
    void drawLineButtons(ChartLine* line);
    sf::RenderWindow* window = nullptr;
    int topPadding = 50;
    int bottomPadding = 100;
    float width = 0;
    float height = 0;
    float laneWidth = 0;
    float columnWidth = 0;
    float columnHeight = 0;
    float measureHeight = 0;
    float viewLines = 0;
    
    unsigned int selectStart = 0;
    unsigned int selectEnd = 0;
    unsigned int playbackPos = 0;

    std::pair<int, ChartLine*> laserHover;
    std::pair<int, ChartLine*> selectedLaser;
    std::pair<int, ChartLine*> selectedLaserEnd;

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

    sf::Font font;
public:
    Chart chart;
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

    //the line specifies where we want to start drawing the measure, it returns the end location
    int drawMeasure(unsigned int measure, unsigned int startLine);

    //hackey but this function essentially generates the laser vertices
    QuadArray generateLaserQuads(int l, const std::map<unsigned int, ChartLine*> &objects, LineIterator startIter, LineIterator endIter);
    void drawChart();
    std::vector<sf::VertexArray> generateSlamQuads(int llineNumine, int start, int end, int laser, bool isWide);

    int getMouseLane();
    int getMouseMeasure();
    int getMouseLine();
    int getMouseSnappedLine();
    int getMouseLaserPos(bool isWide);

    int getMeasureFromGlobal(unsigned int loc);

    void handleEvent(sf::Event event);
    std::vector <float> getLaserX(ChartLine* line);
    sf::Vector2f getMeasureStart(int measure);

    sf::Vector2f getSnappedPos(ToolType type);
    //get the note location from measure, lane, and line positio
    //sf::Vector2f getNoteLocation(int measure, int lane, int line);

    //get the note location from line (global) and lane
    sf::Vector2f getNoteLocation(int lane, int line);
    //get the note location from line (global) 
    sf::Vector2f getNoteLocation(int line);

    float triArea(sf::Vector2f A, sf::Vector2f B, sf::Vector2f C);
    bool getMouseOverlap(const sf::VertexArray quad);

    void loadFile(std::string mapFilePath, std::string mapFileName);
    void saveFile(std::string fileName);
    void saveFile();

    LineIterator getEditorStartLine();
    LineIterator getEditorEndLine();

    //vars
    boost::interprocess::managed_shared_memory memSegment;
    int editorMeasure = 0;
    int editorLineStart = 0;
    int columns = 6;
    
    //this is relative to 4/4, if we have say 6/4 it would only draw 3 columns
    int measuresPerColumn = 4;

    //derived from 4/4 with 4 measures per column
    int beatsPerColumn = 16;
    int snapGridSize = 16;
    int pulsesPerBeat = 48;
    int laserMoveSize = 5;
    float playbackSpeed = 1.0;
    AudioManager player;

    bool select = false;

    ToolType tool = ToolType::BT;

    gameControl* controlPtr;

    std::unordered_map<int, ChartLine*> selectedLines;
    
};