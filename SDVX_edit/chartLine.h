#pragma once
#include <vector>
#include <string>
#include "enums.h"
#include <iostream>
#include <bitset>

const int L_CONNECTOR = -2;
const int L_NONE = -1;

struct Command {
	CommandType type = CommandType::INVALID;
	std::string val = "";
};

struct LineMask {
	std::bitset <4> bt = 0x00;
	std::bitset <2> fx = 0x00;
	std::bitset <2> laser = 0x00;

	LineMask() = default;

	//LineMask(const ChartLine& line);

	LineMask(std::bitset <4> _bt, std::bitset <2> _fx, std::bitset <2> _laser) : bt(_bt), fx(_fx), laser(_laser) {};

	LineMask operator~() {
		LineMask maskNot;
		maskNot.bt = ~bt;
		maskNot.fx = ~fx;
		maskNot.laser = ~laser;
		return maskNot;
	}

	explicit operator int() const { return bt.count() + fx.count() + laser.count(); }
};

namespace Mask {
	static const LineMask BT(0x0F, 0x00, 0x00);
	static const LineMask FX(0x00, 0x03, 0x00);
	static const LineMask LASER_L(0x00, 0x00, 0x01);
	static const LineMask LASER_R(0x00, 0x00, 0x02);
	static const LineMask LASER_ALL(0x00, 0x00, 0x03);
	static const LineMask LASER[2] = { LASER_L, LASER_R };
	static const LineMask NONE(0x00, 0x00, 0x00);
	static const LineMask ALL(0x0F, 0x03, 0x03);
}

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

	LineMask operator&(const LineMask& line);

	ChartLine replaceMask(LineMask mask, const ChartLine& b);

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

	LineMask makeMask() const;

	void clearLaser(int laser);

	static const uint8_t FX_HOLD = 1;
	static const uint8_t BT_HOLD = 2;

	static const uint8_t FX_CHIP = 2;
	static const uint8_t BT_CHIP = 1;
};
