#pragma once
#include "enums.h"
#include <vector>
#include <string>

struct Command {
    CommandType type = CommandType::INVALID;
    std::string val = "";
};

typedef struct ChartLine {
    //2 for long note, 1 for chip
    uint8_t btVal[4] = { 0, 0, 0, 0 };
    //1 for long note, 2 for chip (legacy reasons tl. stupid devs)
    uint8_t fxVal[2] = { 0, 0 };
    int8_t laserPos[2] = { -1, -1 };
    bool isWide[2] = { false, false };
    //this is used for drawing, and represents and active laser position at that point
    float laserConnectionPos[2] = { 0.0, 0.0 };
    std::vector<Command> cmds;
};

typedef struct Measure {
    //division of the measure (eg. 32, 16, 12, etc)
    //the standard measure division is 192
    int division;
    std::vector<ChartLine> lines;
};

typedef struct Chart {
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

