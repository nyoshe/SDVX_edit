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
    std::vector <ChartLine*> nextLaser = { nullptr, nullptr };
    std::vector <ChartLine*> prevLaser = { nullptr, nullptr };
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

    
};

 struct Measure {
    //division of the measure (eg. 32, 16, 12, etc)
    //the standard measure division is 192, it can go higher if the time signature is different

    //this defines the top part of the time signature
    int topSig = 4;

    std::map<int, ChartLine*> lines;

    //position of measure start in 192 snapping
    unsigned int pos = 0;
};


