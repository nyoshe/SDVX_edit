#pragma once
#include <bitset>
#include <iostream>
#include <string>
#include <vector>
#include "Enums.h"

//really dumb, but these were initially ints
const float L_CONNECTOR = -2;
const float L_NONE = -1;

struct Command
{
	CommandType type = INVALID;
	std::string val;
};

struct LineMask
{
	std::bitset<4> bt = 0x00;
	std::bitset<2> fx = 0x00;
	std::bitset<2> laser = 0x00;

	LineMask() = default;

	//LineMask(const ChartLine& line);

	LineMask(std::bitset<4> _bt, std::bitset<2> _fx, std::bitset<2> _laser) : bt(_bt), fx(_fx), laser(_laser) {};

	LineMask operator~()
	{
		LineMask maskNot;
		maskNot.bt = ~bt;
		maskNot.fx = ~fx;
		maskNot.laser = ~laser;
		return maskNot;
	}
	std::string toString()
	{
		std::string out;
		for (int i = 0; i < 4; i++) {
			out.push_back(static_cast<char>(bt[i] + '0'));
		}
		out.push_back('|');
		for (int i = 0; i < 2; i++) {
			out.push_back(static_cast<char>(fx[i] + '0'));
		}
		out.push_back('|');
		for (int i = 0; i < 2; i++) {
			if (laser[i]) {
				out.push_back('1');
			}
			else {
				out.push_back('0');
			}
		}
		return out;
	}

	explicit operator int() const { return bt.count() + fx.count() + laser.count(); }
};

namespace Mask
{
	static const LineMask BT(0x0F, 0x00, 0x00);
	static const LineMask FX(0x00, 0x03, 0x00);
	static const LineMask LASER_L(0x00, 0x00, 0x01);
	static const LineMask LASER_R(0x00, 0x00, 0x02);
	static const LineMask LASER_ALL(0x00, 0x00, 0x03);
	static const LineMask LASER[2] = {LASER_L, LASER_R};
	static const LineMask NONE(0x00, 0x00, 0x00);
	static const LineMask ALL(0x0F, 0x03, 0x03);
}

class ChartLine
{
public:
	//2 for long note, 1 for chip
	std::vector<uint8_t> btVal = {0, 0, 0, 0};
	//1 for long note, 2 for chip (legacy reasons tl. stupid devs)
	std::vector<uint8_t> fxVal = {0, 0};
	std::vector<float> laserPos = {L_NONE, L_NONE};
	std::vector<bool> isWide = {false, false};
	ChartLine* next = nullptr;
	ChartLine* prev = nullptr;
	//defines our position relative to 1/192 snapping
	unsigned int pos = 0;
	unsigned int measurePos = 0;
	std::vector<Command> cmds;
	bool empty();

	LineMask operator&(const LineMask& line);

	ChartLine replaceMask(const LineMask& mask, const ChartLine& b);

	ChartLine* getNextLaser(int laser);
	ChartLine* getPrevLaser(int laser);

	ChartLine* getBtStart(int lane);
	ChartLine* getBtEnd(int lane);

	ChartLine* getFxStart(int lane);
	ChartLine* getFxEnd(int lane);

	std::vector<std::pair<ChartLine*, ChartLine*>> clearBtHold(int lane);
	std::vector<std::pair<ChartLine*, ChartLine*>> clearFxHold(int lane);

	void modifyLaserPos(int laser, float val);

	ChartLine* extractMask(const LineMask& mask);

	LineMask makeMask() const;

	void clearLaser(int laser);

	std::vector<Command> getCommandType(CommandType c);

	std::string toString();

	static constexpr uint8_t FX_HOLD = 1;
	static constexpr uint8_t BT_HOLD = 2;

	static constexpr uint8_t FX_CHIP = 2;
	static constexpr uint8_t BT_CHIP = 1;
};
