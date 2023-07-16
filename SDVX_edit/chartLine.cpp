#include "ChartLine.h"

#include "Parser.h"

LineMask ChartLine::makeMask() const
{
	LineMask out;
	for (int i = 0; i < 2; i++) {
		if (this->fxVal[i] > 0) {
			out.fx[i] = true;
		}
		if (this->laserPos[i] != L_NONE) {
			out.laser[i] = true;
		}
	}
	for (int i = 0; i < 4; i++) {
		if (this->btVal[i] > 0) {
			out.bt[i] = true;
		}
	}
	return out;
}

bool ChartLine::empty()
{
	for (int i = 0; i < 4; i++) {
		if (btVal[i] == 1) {
			return false;
		}
		//we are considered non empty if we are the start of a hold note
		if (prev) {
			if ((prev->btVal[i] == 0 || prev->btVal[i] == 1) && btVal[i] == 2) {
				return false;
			}
		}
	}
	for (int i = 0; i < 2; i++) {
		if (fxVal[i] == 2) {
			return false;
		}
		if (prev) {
			if ((prev->fxVal[i] == 0 || prev->fxVal[i] == 2) && fxVal[i] == 1) {
				return false;
			}
		}
		if (laserPos[i] >= 0) {
			return false;
		}
		//we are considered empty only if we are the connector to a following laser point
		if (next && prev && laserPos[i] == L_CONNECTOR) {
			if (next->laserPos[i] >= 0 && prev->laserPos[i] >= 0) {
				return false;
			}
		}
	}
	if (cmds.size() > 0) {
		return false;
	}
	return true;
}

ChartLine* ChartLine::getNextLaser(int laser)
{
	ChartLine* line = this->next;
	while (line && line->laserPos[laser] != L_NONE) {
		if (line->laserPos[laser] == L_NONE) {
			return nullptr;
		}
		if (line->laserPos[laser] >= 0) {
			return line;
		}
		line = line->next;
	}
	return nullptr;
}

ChartLine* ChartLine::getPrevLaser(int laser)
{
	ChartLine* line = this->prev;
	while (line && line->laserPos[laser] != L_NONE) {
		if (line->laserPos[laser] == L_NONE) {
			return nullptr;
		}
		if (line->laserPos[laser] >= 0) {
			return line;
		}
		line = line->prev;
	}
	return nullptr;
}

ChartLine* ChartLine::getBtStart(int lane)
{
	auto line = this;
	if (line->btVal[lane] != 2) {
		return this;
	}
	while (line->prev && line->prev->btVal[lane] == 2) {
		line = line->prev;
	}
	return line;
}

ChartLine* ChartLine::getBtEnd(int lane)
{
	auto line = this;
	if (line->btVal[lane] != 2) {
		return this;
	}
	while (line->next && line->next->btVal[lane] == 2) {
		line = line->next;
	}
	return line;
}

ChartLine* ChartLine::getFxStart(int lane)
{
	auto line = this;
	if (line->fxVal[lane] != 1) {
		return this;
	}
	while (line->prev && line->prev->fxVal[lane] == 1) {
		line = line->prev;
	}
	return line;
}

ChartLine* ChartLine::getFxEnd(int lane)
{
	auto line = this;
	if (line->fxVal[lane] != 1) {
		return this;
	}
	while (line->next && line->next->fxVal[lane] == 1) {
		line = line->next;
	}
	return line;
}

std::vector<std::pair<ChartLine*, ChartLine*>> ChartLine::clearBtHold(int lane)
{
	std::vector<std::pair<ChartLine*, ChartLine*>> actionList;
	if (this->btVal[lane] == 1) return actionList;
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

std::vector<std::pair<ChartLine*, ChartLine*>> ChartLine::clearFxHold(int lane)
{
	std::vector<std::pair<ChartLine*, ChartLine*>> actionList;
	if (this->fxVal[lane] == 2) return actionList;
	ChartLine* start = getFxStart(lane);
	ChartLine* end = getFxEnd(lane);
	while (start != end) {
		actionList.push_back(std::make_pair(start, new ChartLine(*start)));
		start->fxVal[lane] = 0;
		start = start->next;
	}
	actionList.push_back(std::make_pair(end, new ChartLine(*end)));
	end->fxVal[lane] = 0;
	return actionList;
}

void ChartLine::modifyLaserPos(int laser, float val)
{
	if (laserPos[laser] + val >= 1.0) {
		laserPos[laser] = 1.0;
	}
	else if (laserPos[laser] + val <= 0) {
		laserPos[laser] = 0;
	}
	else {
		laserPos[laser] += val;
	}
}

//essentially overwrites if mask bits are set
ChartLine ChartLine::replaceMask(const LineMask& mask, const ChartLine& b)
{
	ChartLine out = *this;
	for (int i = 0; i < 2; i++) {
		if (mask.laser[i]) {
			out.laserPos[i] = b.laserPos[i];
			if (b.isWide[i] && !this->isWide[i]) {
				out.isWide[i] = true;
			}
			else if (!b.isWide[i]) {
				out.isWide[i] = false;
			}
		}
		//a merge overwrites the laser position
		if (mask.fx[i]) {
			out.fxVal[i] = b.fxVal[i];
		}
	}
	for (int i = 0; i < 4; i++) {
		if (mask.bt[i]) {
			out.btVal[i] = b.btVal[i];
		}
	}
	return out;
}

ChartLine* ChartLine::extractMask(const LineMask& mask)
{
	ChartLine* output = new ChartLine();
	for (int i = 0; i < 4; i++) {
		if (mask.bt[i]) {
			output->btVal[i] = btVal[i];
		}
	}
	for (int i = 0; i < 2; i++) {
		if (mask.fx[i]) {
			output->fxVal[i] = fxVal[i];
		}
		if (mask.laser[i]) {
			output->laserPos[i] = laserPos[i];
			output->isWide[i] = isWide[i];
		}
	}
	output->next = next;
	output->prev = prev;
	output->pos = pos;
	output->measurePos = measurePos;

	return output;
}

LineMask ChartLine::operator&(const LineMask& line)
{
	LineMask out = line;
	for (int i = 0; i < 4; i++) {
		if (line.bt[i] && (btVal[i] != BT_HOLD)) {
			out.bt[i] = false;
		}
	}
	for (int i = 0; i < 2; i++) {
		if (line.fx[i] && (fxVal[i] != FX_HOLD)) {
			out.fx[i] = false;
		}
		if (line.laser[i] && (laserPos[i] == L_NONE)) {
			out.laser[i] = false;
		}
		if (line.laser[i] && (laserPos[i] >= 0)) {
			out.laser[i] = false;
		}
	}
	return out;
}

void ChartLine::clearLaser(int laser)
{
	laserPos[laser] = L_NONE;
	isWide[laser] = false;
}

std::vector<Command> ChartLine::getCommandType(CommandType c)
{
	std::vector<Command> out;
	for (auto command : cmds) {
		out.push_back(command);
	}
	return out;
}

std::string ChartLine::toString()
{
	std::string out;
	for(int i = 0; i < 4; i++) {
		out.push_back(static_cast<char>(btVal[i] + '0'));
	}
	out.push_back('|');
	for (int i = 0; i < 2; i++) {
		out.push_back(static_cast<char>(fxVal[i] + '0'));
	}
	out.push_back('|');
	for (int i = 0; i < 2; i++) {
		if (laserPos[i] == L_NONE) {
			out.push_back('-');
		}
		else if (laserPos[i] == L_CONNECTOR) {
			out.push_back(':');
		}
		else {
			out.push_back(laserVals[std::clamp(static_cast<int>(std::lrint(laserPos[i] * 50.f)), 0,50)]);
		}
	}
	return out;
}
