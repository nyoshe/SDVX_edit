#include "Chart.h"

Chart::~Chart() {
	//iterate through lines and delete them
	for (auto& it : lines) {
		delete it.second;
	}
	clearUndoStack();
	clearRedoStack();
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

ChartLine* Chart::insertChartLine(unsigned int line, ChartLine* cLine) {
	unsigned int absPos = line;
	auto it = lines.find(absPos);
	//check to see if we are on a new measure
	if (measures.size() == 0) {
		appendNewMeasure();
	}

	int measure = 0;

	if (absPos > measures.back().pos + measures.back().pulses) {
		int loc = measures.back().pos + measures.back().pulses;
		while (loc < absPos) {
			loc += appendNewMeasure();
			measure += 1;
		}
		calcTimings();
	}
	else {
		if (lines.lower_bound(absPos) == lines.end()) {
			measure = std::prev(lines.end(), 1)->second->measurePos;
		}
		else {
			measure = lines.lower_bound(absPos)->second->measurePos;
		}
	}

	int localPos = measures[measure].pulses + line - measures[measure].pos;


	//if we don't exist
	if (it == lines.end()) {

		measures[measure].lines[localPos] = cLine;
		lines[absPos] = cLine;

		cLine->pos = absPos;
		cLine->measurePos = measure;
		it = lines.find(absPos);

		//check if we are not the first object
		if (it != lines.begin()) {
			addUndoBuffer(std::prev(it, 1)->second);
			connectLines(std::prev(it, 1), it);
			//we must push the change in the next line
		}

		//check if we are not the first object
		if (std::next(it, 1) != lines.end()) {
			addUndoBuffer(std::next(it, 1)->second);
			connectLines(it, std::next(it, 1));
			//we must push the change in the next line
		}

		//here we extend  the buttons and holds if needed
		if (std::next(it, 1) != lines.end() && it != lines.begin()) {
			for (int i = 0; i < 2; i++) {
				if ((cLine->prev->laserPos[i] == -2 || cLine->next->laserPos[i] == -2)) {
					cLine->laserPos[i] = -2;
				}
				//extend hold fx
				if (cLine->prev->fxVal[i] == 1) {
					cLine->fxVal[i] = 1;
				}
			}
			for (int i = 0; i < 4; i++) {
				//extend hold bt
				if (cLine->prev->btVal[i] == 2) {
					cLine->btVal[i] = 2;
				}
			}
		}



		//should fix this and not access directly
		undoBuffer.push_back(std::make_pair(nullptr, cLine));

	}
	//if the object already exists, merge them
	else {
		ChartLine* existingLine = lines[absPos];

		addUndoBuffer(existingLine);
		//actionList.push_back(std::make_pair(existingLine, new ChartLine(*lines[absPos])));

		*existingLine += *cLine;

		delete cLine;
	}

	pushUndoBuffer();
	return lines[absPos];
}

//this should only really be called for user input
void Chart::removeChartLine(unsigned int line, unsigned int lane, ToolType type) {

	if (lane > 3 || lane < 0) {
		return;
	}
	unsigned int absPos = line;
	int measure = lines.lower_bound(absPos)->second->measurePos;
	auto it = lines.find(absPos);

	std::vector<std::pair<ChartLine*, ChartLine*>> actionList;

	if (type == ToolType::BT) {
		//check to see if we have a hold before and after our note
		if (lines.lower_bound(line)->second->btVal[lane] == 2 && lines.upper_bound(line)->second->btVal[lane] == 2) {
			addUndoBuffer(lines.lower_bound(line)->second->clearBtHold(lane));
		}
	}
	if (type == ToolType::FX) {
		//check to see if we have a hold before and after our note
		if (lines.lower_bound(line)->second->fxVal[lane / 2] == 1 && lines.upper_bound(line)->second->fxVal[lane / 2] == 1) {
			addUndoBuffer(lines.lower_bound(line)->second->clearFxHold(lane / 2));
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
	pushUndoBuffer();
}


void Chart::undo() {
	if (undoStack.empty()) return;
	//std::vector<std::pair<ChartLine*, ChartLine*>> buffer;
	for (auto it : undoStack.top()) {
		if (it.first == nullptr) {
			//buffer.push_back(it);
			redoBuffer.push_back(it);
			lines.erase(it.second->pos);
			measures[it.second->measurePos].lines.erase(it.second->pos - measures[it.second->measurePos].pos);
		}
		else {
			addRedoBuffer(it.first);
			//buffer.push_back(std::make_pair(it.first, new ChartLine(*it.first)));
			*(it.first) = *(it.second);
			delete it.second;
		}
	}
	
	//redoStack.push(buffer);
	pushRedoBuffer();
	undoStack.pop();
}

void Chart::redo() {
	if (redoStack.empty()) return;
	//std::vector<std::pair<ChartLine*, ChartLine*>> buffer;
	for (auto it : redoStack.top()) {
		if (it.first == nullptr) {
			//buffer.push_back(it);
			undoBuffer.push_back(it);
			lines[it.second->pos] = it.second;
			measures[it.second->measurePos].lines[it.second->pos - measures[it.second->measurePos].pos] = it.second;
		}
		else {
			//buffer.push_back(std::make_pair(it.first, new ChartLine(*it.first)));
			addUndoBuffer(it.first);
			*(it.first) = *(it.second);
			delete it.second;
		}
	}
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
int Chart::appendNewMeasure() {
	
	int measureStart = 0;
	if (measures.size() == 0) {
		Measure m;
		ChartLine* newLine = new ChartLine;
		measures.push_back(m);
		measures[0].lines[0] = newLine;
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
		measures.back().lines[0] = newLine;
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

void moveChartLine(unsigned int line, int change) {

}

