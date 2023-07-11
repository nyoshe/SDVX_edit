#pragma once
#include "chart.h"
#include "ChartLine.h"
#include "Structs.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

static std::string laserVals = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmno";

static const std::unordered_map<std::string, CommandType> cmdTable = {
	{"t", TEMPO_CHANGE},
	{"beat", BEAT_CHANGE},
	{"chokkakuvol", SLAM_VOLUME},
	{"chokkakuse", SLAM_SETTING},
	{"pfiltergain", PEAK_FILTER_GAIN},
	{"stop", STOP},
	{"tilt", TILT},
	{"zoom_top", ZOOM_TOP},
	{"zoom_bottom", ZOOM_BOTTOM},
	{"zoom_side", ZOOM_SIDE},
	{"center_split", CENTER_SPLIT},
	{"laserrange_l", WIDE_LASER_L},
	{"laserrange_r", WIDE_LASER_R},
	{"fx-l", FX_L},
	{"fx-r", FX_R},
	{"fx-l_se", FX_L_EFFECT},
	{"fx-r_se", FX_R_EFFECT},
	{"filtertype", FILTER_TYPE},
	{"tickrate_offset", TICKRATE_OFFSET},
	{"@(", SPIN_L},
	{"@)", SPIN_R},
	{"@<", HALF_SPIN_L},
	{"@>", HALF_SPIN_R},
	{"S<", SWING_L},
	{"S>", SWING_R}
};

class Parser
{
private:
	Command parseCommand(std::string s);

public:
	void loadFile(std::string fileName, Chart& chart);
	void saveFile(Chart& chart, std::string fileName);
};
