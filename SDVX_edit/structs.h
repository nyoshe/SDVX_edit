#pragma once
#include "enums.h"
#include <vector>
#include <string>
#include <map>
#include <SFML/Graphics.hpp>

struct Command {
    CommandType type = CommandType::INVALID;
    std::string val = "";
};

struct ChartLine {
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
    bool empty() {
        for (int i = 0; i < 4; i++) {
            if (btVal[i]) {
                return false;
            }
        }
        for (int i = 0; i < 2; i++) {
            if (fxVal[i]) {
                return false;
            }
            if (laserPos[i] != -1) {
                return false;
            }
        }
        return true;
    }
    //merging
    ChartLine& operator+=(const ChartLine& b) {
        for (int i = 0; i < 2; i++) {
            if (b.laserPos[i] != -1) {
                this->laserPos[i] = b.laserPos[i];
            }
            //a merge overwrites the laser position
            if (b.laserPos[i] >= 0) {
                this->laserPos[i] = b.laserPos[i];
            }
            if (b.fxVal[i] != 0) {
                this->fxVal[i] = b.fxVal[i];
            }
        }
        for (int i = 0; i < 4; i++) {
            //extend hold notes
            if (b.btVal[i] != 0) {
                this->btVal[i] = b.btVal[i];
            }
        }
        return *this;
    }

    ChartLine* getNextLaser(int laser) {
        ChartLine* line = this->next;
        while (line != nullptr && line->laserPos[laser] != -1) {
            if (line->laserPos[laser] == -1) {
                return nullptr;
            }
            if (line->laserPos[laser] >= 0) {
                return line;
            }
            line = line->next;
        }
        return nullptr;
    }

    ChartLine* getPrevLaser(int laser) {
        ChartLine* line = this->prev;
        while (line != nullptr && line->laserPos[laser] != -1) {
            if (line->laserPos[laser] == -1) {
                return nullptr;
            }
            if (line->laserPos[laser] >= 0) {
                return line;
            }
            line = line->prev;
        }
        return nullptr;
    }

    ChartLine* getBtStart(int lane) {
        ChartLine* line = this;
        while (line != nullptr && line->btVal[lane] == 2) {
            line = line->prev;
        }
        return line;
    }

    ChartLine* getBtEnd(int lane) {
        ChartLine* line = this;
        while (line != nullptr && line->btVal[lane] == 2) {
            line = line->next;
        }
        return line;
    }

    ChartLine* getFxStart(int lane) {
        ChartLine* line = this;
        while (line != nullptr && line->fxVal[lane] == 1) {
            line = line->prev;
        }
        return line;
    }

    ChartLine* getFxEnd(int lane) {
        ChartLine* line = this;
        while (line != nullptr && line->fxVal[lane] == 1) {
            line = line->next;
        }
        return line;
    }

    std::vector<std::pair<ChartLine*, ChartLine*>> clearBtHold(int lane) {
        std::vector<std::pair<ChartLine*, ChartLine*>> actionList;
        ChartLine* start = getBtStart(lane);
        ChartLine* end = getBtEnd(lane);
        while (start != end) {
            actionList.push_back(std::make_pair(start, new ChartLine(*start)));
            start->btVal[lane] = 0;
            start = start->next;
        }
        actionList.push_back(std::make_pair(end, new ChartLine(*end)));
        end->btVal[lane] = 0;
        return actionList;
    }

    std::vector<std::pair<ChartLine*, ChartLine*>> clearFxHold(int lane) {
        std::vector<std::pair<ChartLine*, ChartLine*>> actionList;
        ChartLine* start = getBtStart(lane);
        ChartLine* end = getBtEnd(lane);
        while (start != end) {
            actionList.push_back(std::make_pair(start, new ChartLine(*start)));
            start->fxVal[lane] = 0;
            start = start->next;
        }
        actionList.push_back(std::make_pair(end, new ChartLine(*end)));
        end->fxVal[lane] = 0;
        return actionList;
    }
};

 struct Measure {
    //division of the measure (eg. 32, 16, 12, etc)
    //the standard measure division is 192, it can go higher if the time signature is different

    //this defines the top part of the time signature, the number of beats per measure
    int topSig = 4;

    //this essentially defines our speed, if it's 4/2, there's essentially twice the measures for a given song
    int bottomSig = 4;

    //the ratio between the top and the bottom determines the number of lines per measure, eg 4/2 would be 192 * 2
    //2/4 would be 192 / 2

    std::map<int, ChartLine*> lines;

    unsigned int pulses = 192;

    //position of measure start in 192 snapping
    unsigned int pos = 0;
    float msStart = 0.0;
};


