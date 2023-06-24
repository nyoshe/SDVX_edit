#pragma once
#include <vector>
#include <string>
#include "enums.h"
#include <iostream>
#include <bitset>



struct Command {
    CommandType type = CommandType::INVALID;
    std::string val = "";
};

struct LineMask {
    std::bitset <4> bt = 0x00;
    std::bitset <2> fx = 0x00;
    std::bitset <2> laser = 0x00;
    LineMask operator~() {
        LineMask maskNot;
        maskNot.bt = ~bt;
        maskNot.fx = ~fx;
        maskNot.laser = ~laser;
        return maskNot;
    }
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

    ChartLine extractMask(LineMask mask);
};

