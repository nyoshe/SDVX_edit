#pragma once
#include <string>
#include <map>
#include <stack>
#include <vector>
#include "structs.h"
class Chart
{
public:
    //metadata
    std::string title = "";
    std::string titleImg = "";
    std::string artist = "";
    std::string artistImg = "";
    std::string effect = "";
    std::string jacket = "";
    std::string illustrator = "";
    std::string difficulty = "";
    std::string icon = "";

    int level = 1;
    float bpm = 120;
    float bpmHi = 0;
    std::string beat = "4/4";
    std::string songFile = "";
    int volume = 100;
    int offset = 0;
    std::string bg = "desert";
    std::string layer = "arrow";
    int previewOffset = 0;
    int previewLength = 0;
    int total = 0;
    int slamVol = 50;
    int slamToVol = 1;
    std::string filterType = "peak";
    int pFilterGain = 50;
    int pFilterDelay = 40;
    std::string videoFile = "";
    int videoOffset = 0;
    std::string version = "";
    std::string info = "";

    //collection of all measures

    std::vector<Measure> measures = {};
    std::map<int, ChartLine*> lines;
    std::map<int, std::vector<Command>> cmds;

    //this is our undo stack, the first pair value is the pointer value, and the second is the previous value
    //the vector is to group multiple movements into one group
    std::stack<std::vector<std::pair<ChartLine*, ChartLine*>>> undoStack;

    //the redo stick contains the next value in the second field
    std::stack<std::vector<std::pair<ChartLine*, ChartLine*>>> redoStack;

    float getMs(unsigned int lineNum);
    void connectLines(ChartLine* l1, ChartLine* l2);
    template <typename T> void connectLines(T l1, T l2);
    void connectLines(ChartLine* l1, ChartLine* l2, ChartLine* l3);

    //this must be done whenever we change the bpm or update the time signature
    void calcTimings();
    ChartLine* insertChartLine(unsigned int measure, unsigned int line, ChartLine* cLine);
    ChartLine* removeChartLine(unsigned int measure, unsigned int line, unsigned int lane, ToolType type);
    void undo();
    void redo();
    void clearRedoStack();
};

