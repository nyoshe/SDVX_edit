#pragma once
#include "enums.h"
#include <vector>
#include <string>

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
    //this is used for drawing, and represents and active laser position at that point
    std::vector <float> laserConnectionPos = { 0.0, 0.0 };
    std::vector <Command> cmds;
};

 struct Measure {
    //division of the measure (eg. 32, 16, 12, etc)
    //the standard measure division is 192
    int division = 32;
    //miiiiiiight have a memory leak, not sure if the pointers delete themselves
    std::vector<ChartLine*> lines;
};

 struct Chart {
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
};

