#pragma once
#include <string>
#include <map>
#include <stack>
#include <vector>
#include "structs.h"
#include "chartLine.h"
#include <iostream>
#include <windows.h>
#include <plog/Log.h>
#include "config.h"

//typedef std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const unsigned int, ChartLine *>>>> lineIterator;
typedef std::map<unsigned int, ChartLine*>::iterator lineIterator;
const int validSnapSizes[12] = { 1, 2, 4, 8, 12, 16, 24, 32, 48, 64, 96, 192 };


struct ChartMetadata {

    std::string mapFileName;
    std::string mapFilePath;

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
};

class Chart
{
private:
    void fixLaserPoint(ChartLine* line, int laser, bool dir);
    //this connects to positions with laser connectors, it does expect to have valid lines
    void fixLaserConnections(int pos1, int pos2, int laser, bool dir);
    void addLaserConnections(ChartLine* line, int laser);

public:
    ChartMetadata metadata;

    //collection of all measures

    std::vector<Measure> measures = {};
    std::map<unsigned int, ChartLine*> lines;
    std::map<int, std::vector<Command>> cmds;

    std::vector<std::pair<ChartLine*, ChartLine*>> redoBuffer;
    std::vector<std::pair<ChartLine*, ChartLine*>> undoBuffer;

    //this is our undo stack, the first pair value is the pointer value, and the second is the previous value
    //the vector is to group multiple movements into one group
    std::stack<std::vector<std::pair<ChartLine*, ChartLine*>>> undoStack;

    //the redo stick contains the next value in the second field
    std::stack<std::vector<std::pair<ChartLine*, ChartLine*>>> redoStack;

    int selectStart;
    int selectEnd;

    ~Chart();
    Chart() = default;

    float getMs(unsigned int lineNum);
    void connectLines(ChartLine* l1, ChartLine* l2);
    template <typename T> void connectLines(T l1, T l2);
    void connectLines(ChartLine* l1, ChartLine* l2, ChartLine* l3);

    void undo();
    void redo();
    void clearRedoStack();
    void clearUndoStack();
    int appendNewMeasure();
    void minimize();
    //this must be done whenever we change the bpm or update the time signature
    void calcTimings();

    void insertChartLine(unsigned int line, std::map<int, ChartLine> lineMap);
    ChartLine* insertChartLine(unsigned int line, ChartLine cLine);

    //returns a pointer to the moved object
    ChartLine* moveChartLine(int line, LineMask moveMask, int change);
    void removeChartLine(unsigned int line, unsigned int lane, ToolType type);
    //void moveChartLine();

    //this should be done before an action is performed
    void addUndoBuffer(std::vector<std::pair<ChartLine*, ChartLine*>> actionList);
    void addUndoBuffer(ChartLine* line);
    void pushUndoBuffer();

    void addRedoBuffer(std::vector<std::pair<ChartLine*, ChartLine*>> actionList);
    void addRedoBuffer(ChartLine* line);
    void pushRedoBuffer();

    //exclusive of the input line
    lineIterator getLineBefore(int line);
    lineIterator getLineAfter(int line);

    int getMeasureFromPos(int pos);

    //quick function to make sure we didn't mess anything up
    void validateChart();

    std::vector<std::pair<ChartLine*, ChartLine>> getSelection(unsigned int pos1, unsigned int pos2, LineMask mask);
};

