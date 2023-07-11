#pragma once
#include "Structs.h"
#include "chart.h"
#include "ChartLine.h"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <functional>
#include <string>

static std::string laserVals = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmno";

static std::unordered_map<std::string, CommandType> const cmdTable = {
			{"t",CommandType::TEMPO_CHANGE},
			{"beat",CommandType::BEAT_CHANGE},
			{"chokkakuvol",CommandType::SLAM_VOLUME},
			{"chokkakuse",CommandType::SLAM_SETTING},
			{"pfiltergain",CommandType::PEAK_FILTER_GAIN},
			{"stop",CommandType::STOP},
			{"tilt",CommandType::TILT},
			{"zoom_top",CommandType::ZOOM_TOP},
			{"zoom_bottom",CommandType::ZOOM_BOTTOM},
			{"zoom_side",CommandType::ZOOM_SIDE},
			{"center_split",CommandType::CENTER_SPLIT},
			{"laserrange_l",CommandType::WIDE_LASER_L},
			{"laserrange_r",CommandType::WIDE_LASER_R},
			{"fx-l",CommandType::FX_L},
			{"fx-r",CommandType::FX_R},
			{"fx-l_se",CommandType::FX_L_EFFECT},
			{"fx-r_se",CommandType::FX_R_EFFECT},
			{"filtertype",CommandType::FILTER_TYPE},
			{"tickrate_offset",CommandType::TICKRATE_OFFSET},
			{"@(", CommandType::SPIN_L},
			{"@)", CommandType::SPIN_R},
			{"@<", CommandType::HALF_SPIN_L},
			{"@>", CommandType::HALF_SPIN_R},
			{"S<", CommandType::SWING_L},
			{"S>", CommandType::SWING_R}
};

class Parser
{
private:
	Command parseCommand(std::string s);

public:
	void loadFile(std::string fileName, Chart& chart);
	void saveFile(Chart& chart, std::string fileName);
};
