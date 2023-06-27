#include "Chart.h"

Chart::~Chart() {
	//iterate through lines and delete them
	for (auto& it : lines) {
		delete it.second;
	}
	clearUndoStack();
	clearRedoStack();
}

void Chart::fixLaserPoint(ChartLine* line, int laser, bool addConnector) {
	//obviously very verbose but it helps me understand it lmao, I really should have used a different data structure for this
	if (line->prev->laserPos[laser] == L_NONE && line->next->laserPos[laser] == L_NONE) {
		addUndoBuffer(line);
		line->laserPos[laser] = L_NONE;
	}
	else if (line->prev->laserPos[laser] == L_NONE && line->next->laserPos[laser] == L_CONNECTOR) {
		addUndoBuffer(line);
		if (line->laserPos[laser] < 0) line->laserPos[laser] = addConnector ? L_CONNECTOR : L_NONE;
	}
	else if (line->prev->laserPos[laser] == L_NONE && line->next->laserPos[laser] >= 0) {
		addUndoBuffer(line);
		line->laserPos[laser] = addConnector ? L_CONNECTOR : L_NONE;
	}
	//this one we actually don't know technically speaking
	else if (line->prev->laserPos[laser] == L_CONNECTOR && line->next->laserPos[laser] == L_NONE) {
		addUndoBuffer(line);
		if (line->laserPos[laser] < 0) line->laserPos[laser] = addConnector ? L_CONNECTOR : L_NONE;
	}
	else if (line->prev->laserPos[laser] == L_CONNECTOR && line->next->laserPos[laser] == L_CONNECTOR) {
		addUndoBuffer(line);
		if (line->laserPos[laser] == L_NONE) line->laserPos[laser] = L_CONNECTOR;
	}
	else if (line->prev->laserPos[laser] == L_CONNECTOR && line->next->laserPos[laser] >= 0) {
		addUndoBuffer(line);
		line->laserPos[laser] = L_CONNECTOR;
	}
	else if (line->prev->laserPos[laser] >= 0 && line->next->laserPos[laser] == L_NONE) {
		addUndoBuffer(line);
		line->laserPos[laser] = addConnector ? L_CONNECTOR : L_NONE;
	}
	else if (line->prev->laserPos[laser] >= 0 && line->next->laserPos[laser] == L_CONNECTOR) {
		addUndoBuffer(line);
		line->laserPos[laser] = L_CONNECTOR;
	}
	else if (line->prev->laserPos[laser] >= 0 && line->next->laserPos[laser] >= 0) {
		addUndoBuffer(line);
		line->laserPos[laser] = L_CONNECTOR;
	}
}

//from start position + 1 to end position - 1, fixes laser weirdness
//also checks laser boundaries
void Chart::fixLaserConnections(int pos1, int pos2, int laser, bool dir) {
	ChartLine* startLine = lines[std::min(pos1, pos2)];
	ChartLine* endLine = lines[std::max(pos1, pos2)];

	
	//we don't want to mess with the start line

	/*
	bool addConnector = false;

	if ((endLine->laserPos[laser] >= 0 && startLine->laserPos[laser] >= 0) || 
		(endLine->laserPos[laser] == L_CONNECTOR && startLine->laserPos[laser] >= 0) ||
		(endLine->laserPos[laser] >= 0 && startLine->laserPos[laser] == L_CONNECTOR)) {
		addConnector = true;
	}
	if (endLine->laserPos[laser] == L_NONE || startLine->laserPos[laser] == L_NONE) {
		addConnector = false;
	}
	
	*/

	ChartLine* cLine = startLine;
	if (dir) {
		while (cLine != nullptr && cLine != endLine) {
			fixLaserPoint(cLine, laser, dir);
			cLine = cLine->next;
		}
	}
	else {
		cLine = endLine;
		while (cLine != nullptr && cLine != startLine) {
			fixLaserPoint(cLine, laser, dir);
			cLine = cLine->prev;
		}
	}
	addLaserConnections(startLine, laser);
	addLaserConnections(endLine, laser);
}

void Chart::addLaserConnections(ChartLine* line, int laser) {
	//now check if we have two lasers right next to eachother
	ChartLine newLaser;
	newLaser.laserPos[laser] = -2;
	if (line->prev != nullptr && line->prev->laserPos[laser] >= 0 && line->laserPos[laser] >= 0) {
		int diff = line->pos - line->prev->pos;
		if (diff >= 2) {
			insertChartLine(line->pos - diff / 2, newLaser, Mask::LASER_ALL);
		}
	}
	if (line->next != nullptr && line->next->laserPos[laser] >= 0 && line->laserPos[laser] >= 0) {
		int diff = line->next->pos - line->pos;
		if (diff >= 2) {
			insertChartLine(line->pos + diff / 2, newLaser, Mask::LASER_ALL);
		}
	}
}

float Chart::getMs(unsigned int lineNum) {
    float currentMs = 0;
    unsigned int lastChange = 0;
    float currentBpm = metadata.bpm;
    int topSig = std::stoi(metadata.beat.substr(0, metadata.beat.find('/')));
    int bottomSig = std::stoi(metadata.beat.substr(metadata.beat.find('/') + 1));
    for (auto cList : cmds) {
        if (cList.first > lineNum) break;
        for (auto cmd : cList.second) {
			int pulses = (192 * topSig) / bottomSig;
            if (cmd.type == CommandType::TEMPO_CHANGE) {
                currentMs += ((cList.first - lastChange) / pulses) * 1000.0 * topSig * (60.0 / currentBpm);
                lastChange = cList.first;
                currentBpm = std::stof(cmd.val);

            }
            else if (cmd.type == CommandType::BEAT_CHANGE) {
                currentMs += ((cList.first - lastChange) / pulses) * 1000.0 * topSig * (60.0 / currentBpm);
                lastChange = cList.first;

                topSig = std::stoi(cmd.val.substr(0, cmd.val.find('/')));
                bottomSig = std::stoi(cmd.val.substr(cmd.val.find('/') + 1));
            }
        }

    }

    currentMs += ((lineNum - lastChange) / 192.0) * 1000.0 * topSig * (60.0 / currentBpm);

    return currentMs;
}

void Chart::calcTimings() {
	for (int i = 0; i < measures.size(); i++) {
		measures[i].msStart = getMs(measures[i].pos);
	}
}

void Chart::connectLines(ChartLine* l1, ChartLine* l2) {
	l1->next = l2;
	l2->prev = l1;
}

template <typename T> void Chart::connectLines(T l1, T l2)
{
	connectLines(l1->second, l2->second);
}

void Chart::connectLines(ChartLine* l1, ChartLine* l2, ChartLine* l3) {
	connectLines(l1, l2);
	connectLines(l2, l3);
}

void Chart::insertChartLine(int line, std::map<unsigned int, ChartLine> lineMap) {
	
	int startLine = line;
	int endLine = line + (std::prev(lineMap.end(), 1)->second.pos - lineMap.begin()->second.pos);
	int offset = line - lineMap.begin()->second.pos;
	//insert and update or locations
	for (auto it = lineMap.begin(); it != lineMap.end(); it++) {
		insertChartLine(it->second.pos + offset, it->second, it->second.makeMask());
		
	}

	
	
	
	for (auto it = lines.find(startLine); it->first <= endLine; it++) {
		addUndoBuffer(it->second);
	}

	for (auto it = lines.find(startLine); it->first <= endLine; it++) {
		if (lineMap.find(it->first - offset) == lineMap.end()) {
			ChartLine beforeLine = ChartLine(std::prev(lineMap.lower_bound(it->first - offset), 1)->second);
			ChartLine afterLine = ChartLine(lineMap.lower_bound(it->first - offset)->second);
			for (int i = 0; i < 2; i++) {
				if ((beforeLine.laserPos[i] == -2 && afterLine.laserPos[i] == -2) ||
					(beforeLine.laserPos[i] >=  0 && afterLine.laserPos[i] == -2) ||
					(beforeLine.laserPos[i] == -2 && afterLine.laserPos[i] >=  0)) {
					it->second->laserPos[i] = -2;
				}
				if (beforeLine.fxVal[i] == 1) {
					it->second->fxVal[i] = 1;
				}
				if (beforeLine.isWide[i] == 1) {
					it->second->isWide[i] = 1;
				}
			}
			for (int i = 0; i < 4; i++) {
				if (beforeLine.btVal[i] == 2) {
					it->second->btVal[i] = 2;
				}
			}
		}
	}
	
	if (lineMap.size() >= 2) {
		LineMask mask;
		for (int i = 0; i < 2; i++) {
			if (std::prev(lineMap.end(), 2)->second.fxVal[i] == 1 && std::prev(lineMap.end(), 1)->second.fxVal[i] == 0) {
				mask.fx[i] = 1;
			}
		}
		for (int i = 0; i < 4; i++) {
			if (std::prev(lineMap.end(), 2)->second.btVal[i] == 2 && std::prev(lineMap.end(), 1)->second.btVal[i] == 0) {
				mask.bt[i] = 1;
			}
		}
		insertChartLine(endLine, std::prev(lineMap.end(), 1)->second, mask);
	}
	
}

ChartLine* Chart::insertChartLine(unsigned int line, ChartLine cLine, LineMask mask) {
	unsigned int absPos = line;
	//check to see if we are on a new measure
	if (measures.size() == 0) {
		appendNewMeasure();
	}

	int measure = 0;

	if (absPos >= measures.back().pos) {
		int loc = measures.back().pos;
		while (loc <= absPos) {
			loc += appendNewMeasure();
		}
		calcTimings();
	}
	else {
		measure = getMeasureFromPos(absPos);
	}

	measure = getMeasureFromPos(absPos);

	int localPos = line - measures[measure].pos;

	auto it = lines.find(absPos);
	//if we don't exist
	if (it == lines.end()) {

		ChartLine* newLine = new ChartLine();
		measures[measure].lines[localPos] = newLine;
		lines[absPos] = newLine;

		newLine->pos = absPos;
		newLine->measurePos = measure;
		it = lines.find(absPos);

		//check if we are not the first object
		if (it != lines.begin()) {
			connectLines(std::prev(it, 1), it);
		}

		//check if we are not the last object
		if (std::next(it, 1) != lines.end()) {
			connectLines(it, std::next(it, 1));
		}


		//here we extend  the buttons and holds if needed
		if (std::next(it, 1) != lines.end() && it != lines.begin()) {
			for (int i = 0; i < 2; i++) {
				if ((newLine->prev->laserPos[i] == -2 || newLine->next->laserPos[i] == -2)) {
					newLine->laserPos[i] = -2;
				}
				if ((newLine->prev->laserPos[i] >= 0 && newLine->next->laserPos[i] >= 0)) {
					newLine->laserPos[i] = -2;
				}
				//extend hold fx
				if (newLine->prev->fxVal[i] == 1 && newLine->fxVal[i] != 2) {
					newLine->fxVal[i] = 1;
				}
			}
			for (int i = 0; i < 4; i++) {
				//extend hold bt
				if (newLine->prev->btVal[i] == 2 && newLine->btVal[i] != 1) {
					newLine->btVal[i] = 2;
				}
			}
		}
		//addUndoBuffer(newLine);

		*newLine = newLine->replaceMask(mask, cLine);

		undoBuffer.push_back(std::make_pair(nullptr, newLine));
	}
	//if the object already exists, merge them
	else {
		ChartLine* existingLine = lines[absPos];

		addUndoBuffer(existingLine);
		//actionList.push_back(std::make_pair(existingLine, new ChartLine(*lines[absPos])));

		*existingLine = existingLine->replaceMask(mask, cLine);

	}
	return lines[absPos];
}

//this should only really be called for user input
void Chart::removeChartLine(unsigned int line, unsigned int lane, ToolType type) {

	if (lane > 3 || lane < 0) {
		return;
	}
	unsigned int absPos = line;
	if (lines.lower_bound(absPos) == lines.end()) return;
	int measure = lines.lower_bound(absPos)->second->measurePos;
	auto it = lines.find(absPos);

	std::vector<std::pair<ChartLine*, ChartLine*>> actionList;

	if (type == ToolType::BT) {
		//check to see if we have a hold before and after our note
		if (lines[getLineBefore(line)->first]->btVal[lane] == 2) {
			addUndoBuffer(lines[getLineBefore(line)->first]->clearBtHold(lane));
		}
	}
	if (type == ToolType::FX) {
		//check to see if we have a hold before and after our note
		if (lines[getLineBefore(line)->first]->fxVal[lane / 2] == 1) {
			addUndoBuffer(lines[getLineBefore(line)->first]->clearFxHold(lane / 2));
		}
	}

	if (it != lines.end()) {


		if (type == ToolType::BT) {
			//remove chip
			if (it->second->btVal[lane] == 1) {
				addUndoBuffer(it->second);
				it->second->btVal[lane] = 0;
			}
		}
		if (type == ToolType::FX) {
			//remove chip
			if (it->second->fxVal[lane / 2] == 2) {
				addUndoBuffer(it->second);
				it->second->fxVal[lane / 2] = 0;
			}
		}
	}
}


void Chart::undo() {
	if (undoStack.empty()) return;
	//std::vector<std::pair<ChartLine*, ChartLine*>> buffer;
	for (auto it : undoStack.top()) {
		if (it.first == nullptr) {
			//make sure we don't delete the first line
			redoBuffer.push_back(it);
			lines.erase(it.second->pos);
			measures[it.second->measurePos].lines.erase(it.second->pos - measures[it.second->measurePos].pos);
			it.second->next->prev = it.second->prev;
			it.second->prev->next = it.second->next;
		}
		else {
			addRedoBuffer(it.first);
			//buffer.push_back(std::make_pair(it.first, new ChartLine(*it.first)));
			*(it.first) = *(it.second);
			delete it.second;
		}
	}
	validateChart();
	//redoStack.push(buffer);
	pushRedoBuffer();
	undoStack.pop();
}

void Chart::redo() {
	if (redoStack.empty()) return;

	for (auto it : redoStack.top()) {
		if (it.first == nullptr) {
			//buffer.push_back(it);
			undoBuffer.push_back(it);
			lines[it.second->pos] = it.second;
			measures[it.second->measurePos].lines[it.second->pos - measures[it.second->measurePos].pos] = it.second;
			it.second->prev->next = it.second;
			it.second->next->prev = it.second;
		}
		else {
			//buffer.push_back(std::make_pair(it.first, new ChartLine(*it.first)));
			addUndoBuffer(it.first);
			*(it.first) = *(it.second);
			delete it.second;
		}
	}
	validateChart();
	//undoStack.push(buffer);
	pushUndoBuffer();
	redoStack.pop();
}


void Chart::clearRedoStack() {
	while (!redoStack.empty()) {
		for (auto it : redoStack.top()) {
			if (it.first != nullptr) {
				delete it.second;
			}
		}
		redoStack.pop();
	}
}


void Chart::clearUndoStack() {
	while (!undoStack.empty()) {
		for (auto it : undoStack.top()) {
			if (it.first != nullptr) {
				delete it.second;
			}
		}
		undoStack.pop();
	}
}

//insert new measure with a dummy line, and also returns the pulses of the appended measure
//TODO FOR TOMORROW: undo stack pulls the NULL pointer version 
int Chart::appendNewMeasure() {
	
	int measureStart = 0;
	if (measures.size() == 0) {
		Measure m;
		ChartLine* newLine = new ChartLine;
		measures.push_back(m);
		measures[0].lines[0] = newLine;
		lines[0] = newLine;
	}
	else {
		Measure prevMeasure = measures.back();
		measureStart = prevMeasure.pos + prevMeasure.pulses;
		Measure m;
		m.pos = measureStart;
		m.topSig = prevMeasure.topSig;
		m.bottomSig = prevMeasure.bottomSig;
		m.pulses = prevMeasure.pulses;

		measures.push_back(m);
		
		ChartLine* newLine = new ChartLine;
		newLine->prev = std::prev(prevMeasure.lines.end(), 1)->second;
		newLine->measurePos = measures.size() - 1;
		newLine->pos = measureStart;
		
		
		std::prev(prevMeasure.lines.end(), 1)->second->next = newLine;
		//addUndoBuffer(std::prev(prevMeasure.lines.end(), 1)->second);
		
		measures.back().lines[0] = newLine;
		lines[measureStart] = newLine;
		
		//insertChartLine(measureStart, ChartLine());
	}
	
	return measures.back().pulses;
}

void Chart::minimize() {

	for (int m = 0; m < measures.size(); m++){
		int nonEmpty = 0;
		int minJumpSize = 1;
		for (auto jumpSize : validSnapSizes) {
			int nonEmptyCounter = 0;
			bool valid = true;
			ChartLine* prev = nullptr;
			//we want to find the smallest number for jump size where all values are non empty
			for (int i = 0; i < measures[m].pulses; i += jumpSize) {
				if (measures[m].lines.find(i) == measures[m].lines.end()) continue;
				if (!measures[m].lines[i]->empty()) nonEmptyCounter++;
				if (prev != nullptr) {
					for (int j = 0; j < 2; j++) {
						if (prev->laserPos[j] >= 0 && measures[m].lines[i]->laserPos[j] >= 0) {
							valid = false;
						}
					}
				}
				if (!valid) break;
				prev = measures[m].lines[i];
			}
			if (nonEmptyCounter >= nonEmpty && valid) {
				nonEmpty = nonEmptyCounter;
				minJumpSize = jumpSize;
			}
		}
		//now clear our measures
		for (int i = 0; i < measures[m].pulses; i++) {
			if (i % minJumpSize != 0) {
				if (measures[m].lines.find(i) != measures[m].lines.end()) {
					ChartLine* line = measures[m].lines[i];
					if (line->prev != nullptr && line->next != nullptr) {
						line->prev->next = line->next;
						line->next->prev = line->prev;
					}
					if (line->prev != nullptr && line->next == nullptr) {
						line->prev->next = nullptr;
					}
					if (line->prev == nullptr && line->next != nullptr) {
						line->next->prev = nullptr;
					}
					int pos = line->pos;
					delete measures[m].lines[i];
					measures[m].lines.erase(i);
					lines.erase(pos);
				}
			}
		}
		//std::cout << m << ": " << 192 / minJumpSize << std::endl;
	}
	//reconnect between measure boundaries
	for (int i = 1; i < measures.size(); i++) {
		std::prev(measures[i-1].lines.end(), 1)->second->next = measures[i].lines.begin()->second;
	}
}

void Chart::addUndoBuffer(ChartLine* line) {
	undoBuffer.push_back(std::make_pair(line, new ChartLine(*line)));
}

//pushes the undo buffer onto the undo stack, this signifies the end of a list of actions
void Chart::pushUndoBuffer() {
	if (undoBuffer.size() > 0) {
		std::reverse(undoBuffer.begin(), undoBuffer.end());
		undoStack.push(undoBuffer);
		undoBuffer.clear();
	}
}


void Chart::addUndoBuffer(std::vector<std::pair<ChartLine*, ChartLine*>> actionList) {
	undoBuffer.insert(undoBuffer.end(), actionList.begin(), actionList.end());
}



void Chart::addRedoBuffer(ChartLine* line) {
	redoBuffer.push_back(std::make_pair(line, new ChartLine(*line)));
}

//pushes the undo buffer onto the undo stack, this signifies the end of a list of actions
void Chart::pushRedoBuffer() {
	if (redoBuffer.size() > 0) {
		std::reverse(redoBuffer.begin(), redoBuffer.end());
		redoStack.push(redoBuffer);
		redoBuffer.clear();
	}
}


void Chart::addRedoBuffer(std::vector<std::pair<ChartLine*, ChartLine*>> actionList) {
	redoBuffer.insert(redoBuffer.end(), actionList.begin(), actionList.end());
}

//I'm 90% sure that the bug is that it isn't also setting laser connection points for the ammount it moves
//also, it doesn't reset the select end on undo
//also, respect laser wide values
ChartLine* Chart::moveChartLine(int line, LineMask moveMask, int change) {
	
	if (line - change < 0) {
		change += line- change;
	}
	//if we can't find the position, create one
	if (lines.find(line + change) == lines.end()) {
		ChartLine newLine;
		insertChartLine(line + change, newLine, moveMask);
	}
	else {
		addUndoBuffer(lines[line + change]);
	}
	addUndoBuffer(lines[line]);


	*(lines[line + change]) += lines[line]->extractMask(moveMask);


	*lines[line] = lines[line]->extractMask(~moveMask);
	
	for (int i = 0; i < 2; i++) {
		if (moveMask.laser[i]) {
			lines[line]->laserPos[i] = L_CONNECTOR;
			fixLaserConnections(line, line + change, i, change > 0);
		}
	}

	validateChart();
	return lines[line + change];
	
}

lineIterator Chart::getLineBefore(int line) {
	if (lines.lower_bound(line) == lines.begin()) {
		return lines.begin();
	}
	return std::prev(lines.lower_bound(line), 1);
}


lineIterator Chart::getLineAfter(int line) {
	return lines.upper_bound(line);
}

//slow quick fix
int Chart::getMeasureFromPos(int pos) {
	if (lines.find(pos) != lines.end()) return lines[pos]->measurePos;
	return getLineBefore(pos)->second->measurePos;
	/*
	int i = 0;
	for (auto measure : measures) {
		if (measure.pos > pos) {
			return i - 1;
		}
		i++;
	}
	return i - 1;
	*/
}

void Chart::validateChart() {
	ChartLine* line = lines.begin()->second;
	ChartLine* prev = nullptr;
	int counter = 0;
	while (line != nullptr) {
		auto loc = lines.find(line->pos);

		if (loc == lines.end()) {
			PLOG_DEBUG << "line position not found in " << metadata.mapFileName << " line: " << line->pos;
			if (BREAK_ON_CHART_ERRORS) DebugBreak();
		}

		if (loc->second != line) {
			PLOG_DEBUG << "line is in wrong position in " << metadata.mapFileName << " line: " << line->pos;
			if (BREAK_ON_CHART_ERRORS) DebugBreak();
		}

		auto measureLoc = measures[line->measurePos].lines.find(line->pos - measures[line->measurePos].pos);

		if (measureLoc == measures[line->measurePos].lines.end()) {
			PLOG_DEBUG << "line was not found in measure in " << metadata.mapFileName << " line: " << line->pos;
			if (BREAK_ON_CHART_ERRORS) DebugBreak();
		}


		if (measureLoc->second != line) {
			PLOG_DEBUG << "line does not match the line found in measure in " << metadata.mapFileName << " line: " << line->pos;
			if (BREAK_ON_CHART_ERRORS) DebugBreak();
		}

		if (counter != 0) {
			if (prev != line->prev) {
				PLOG_DEBUG << "lines not properly connected in " << metadata.mapFileName << " line: " << line->pos;
				if (BREAK_ON_CHART_ERRORS) DebugBreak();
			}

			if (prev->next != line) {
				PLOG_DEBUG << "lines not properly connected in " << metadata.mapFileName << " line: " << line->pos;
				if (BREAK_ON_CHART_ERRORS) DebugBreak();
			}

			for (int i = 0; i < 2; i++) {
				if (prev->laserPos[i] >= 0 && line->laserPos[i] >= 0) {
					PLOG_DEBUG << "two laser positions in a row in " << metadata.mapFileName << " line: " << line->pos;
					if (BREAK_ON_CHART_ERRORS) DebugBreak();
				}
				if (prev->laserPos[i] == -1 && line->laserPos[i] == -2) {
					PLOG_DEBUG << "missing laser position before connector in " << metadata.mapFileName << " line: " << line->pos;
					if (BREAK_ON_CHART_ERRORS) DebugBreak();
				}
				if (prev->laserPos[i] == -2 && line->laserPos[i] == -1) {
					PLOG_DEBUG << "missing laser position after connector in " << metadata.mapFileName << " line: " << line->pos;
					if (BREAK_ON_CHART_ERRORS) DebugBreak();
				}

				if (line->next != nullptr) {
					if (prev->laserPos[i] == -1 && line->next->laserPos[i] == -1 && line->laserPos[i] >= 0) {
						PLOG_DEBUG << "laser pos with no connector in " << metadata.mapFileName << " line: " << line->pos;
						if (BREAK_ON_CHART_ERRORS) DebugBreak();
						if (TRY_FIX_CHART_ERRORS) line->laserPos[i] == -1;
					}
				}
			}
		}

		prev = line;
		line = line->next;
		counter++;
	}
	int i = 0;
	int measureCount = 0;
	for (auto measure : measures) {
		
		if (measure.lines.find(0) == measure.lines.end()) {
			PLOG_DEBUG << "did not find zero'th line in measure in " << metadata.mapFileName << " measure: " <<  i;
			if (BREAK_ON_CHART_ERRORS) DebugBreak();
		}

		measureCount += measure.lines.size();
		i++;
	}

	if (counter != lines.size()) {
		PLOG_DEBUG << "mismatch in line size and the number of found lines, counter: " << counter << " size: " << lines.size() << std::endl;
		if (BREAK_ON_CHART_ERRORS) DebugBreak();
	}

	if (measureCount != lines.size()) {
		PLOG_DEBUG << "mismatch in line size and the number of found measure lines, counter: " << measureCount << " size: " << lines.size() << std::endl;
		if (BREAK_ON_CHART_ERRORS) DebugBreak();
	}
	
	std::cout << "no issues found :)" << std::endl;
}

std::map<unsigned int, ChartLine> Chart::getSelection(unsigned int pos1, unsigned int pos2, LineMask mask) {
	unsigned int start = std::min(pos1, pos2);
	unsigned int end = std::max(pos1, pos2);
	std::vector<std::pair<ChartLine*, ChartLine>> out;
	//fix this lmao
	std::map<unsigned int, ChartLine> finalOut;

	if (lines.find(start) == lines.end()) {
		if (getLineAfter(start) == lines.end()) return finalOut;
		start = getLineAfter(start)->second->pos;
	}
	if (lines.find(end) == lines.end()) {
		if (getLineBefore(end) == lines.end()) return finalOut;
		end = getLineBefore(end)->second->pos;
	}

	ChartLine* line = lines[start]->prev;
	LineMask dropMask = *lines[start] & mask;
	//this basically just goes from front to bakc and using the chart line mask, drops out the hold values when we don't see them
	while (line != nullptr && int(*line & dropMask) != 0) {
		dropMask = *line & dropMask;
		out.push_back(std::make_pair(line, line->extractMask(dropMask)));
		line = line->prev;
	}

	std::reverse(out.begin(), out.end());

	line = lines[start];
	while (line != nullptr && line->pos <= end) {
		out.push_back(std::make_pair(line, line->extractMask(mask)));
		line = line->next;
	}

	line = lines[end]->next;
	dropMask = *lines[end] & mask;
	//same as previous comment but not in reverse
	while (line != nullptr && int(*line & dropMask) != 0) {
		dropMask = *line & dropMask;
		out.push_back(std::make_pair(line, line->extractMask(dropMask)));
		line = line->next;
	}

	if (out.size() == 0) return finalOut;
	//cleanup laser ends

	for (int i = 0; i < 2; i++) {
		LineMask laserMask = Mask::NONE;
		if (out.back().first->laserPos[i] == L_CONNECTOR && out.back().first->next->laserPos[i] >= 0) {
			laserMask.laser[i] = 1;
		}
		if (int(laserMask) != 0) {
			out.push_back(std::make_pair(out.back().first->next, out.back().first->next->extractMask(laserMask)));
		}
			

		laserMask = Mask::NONE;
		if (out.front().first->laserPos[i] == L_CONNECTOR && out.front().first->next->laserPos[i] >= 0) {
			laserMask.laser[i] = 1;
		}
		if (int(laserMask) != 0) {
			out.insert(out.begin(), std::make_pair(out.back().first->next, out.back().first->next->extractMask(laserMask)));
		}

		//check for cutoff lasers
		if (out.front().second.laserPos[i] == L_CONNECTOR) {
			for (auto& line : out) {
				if (line.second.laserPos[i] == L_CONNECTOR) {
					line.second.clearLaser(i);
				}
				else if (line.second.laserPos[i] >= 0) {
					break;
				}
			}
		}
		if (out.back().second.laserPos[i] == L_CONNECTOR) {
			for (int line = out.size() - 1; line > 0; line--) {
				if (out[line].second.laserPos[i] == L_CONNECTOR) {
					out[line].second.clearLaser(i);
				}
				else if (out[line].second.laserPos[i] >= 0) {
					break;
				}
			}
		}
	}
	LineMask endExtract;
	for (int i = 0; i < 2; i++) {
		if (out.back().second.fxVal[i] == 1) {
			endExtract.fx[i] = 1;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (out.back().second.btVal[i] == 2) {
			endExtract.bt[i] = 1;
		}
	}
	
	if (out.back().first->next != nullptr) {
		out.push_back(std::make_pair(out.back().first->next, out.back().first->next->extractMask(endExtract)));
	}
	else {
		out.push_back(std::make_pair(nullptr, ChartLine()));
	}
	

	for (auto line : out) {
		finalOut[line.second.pos] = line.second;
	}

	return finalOut;
}