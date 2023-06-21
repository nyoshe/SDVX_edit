#pragma once
#include <vector>
#include <string>
#include "enums.h"
#include <iostream>

struct Command {
    CommandType type = CommandType::INVALID;
    std::string val = "";
};

class ChartLine {
public:
    //2 for long note, 1 for chip
    std::vector <uint8_t> btVal = { 0, 0, 0, 0 };
    //1 for long note, 2 for chip (legacy reasons tl. stupid devs)
    std::vector <uint8_t> fxVal = { 0, 0 };
    std::vector <int8_t> laserPos = { -1, -1 };
    std::vector <bool>  isWide = { false, false };
    ChartLine* next = nullptr;
    ChartLine* prev = nullptr;
    //defines our position relative to 1/192 snapping
    unsigned int pos = 0;
    unsigned int measurePos = 0;
    std::vector <Command> cmds;
    bool empty();
    //merging
    ChartLine& operator+=(const ChartLine& b);

    ChartLine* getNextLaser(int laser);
    ChartLine* getPrevLaser(int laser);

    ChartLine* getBtStart(int lane);
    ChartLine* getBtEnd(int lane);

    ChartLine* getFxStart(int lane);
    ChartLine* getFxEnd(int lane);

    std::vector<std::pair<ChartLine*, ChartLine*>> clearBtHold(int lane);
    std::vector<std::pair<ChartLine*, ChartLine*>> clearFxHold(int lane);

    void modifyLaserPos(int laser, int val);
};

