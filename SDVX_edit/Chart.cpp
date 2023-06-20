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
    float currentBpm = bpm;
    int topSig = std::stoi(beat.substr(0, beat.find('/')));
    int bottomSig = std::stoi(beat.substr(beat.find('/') + 1));
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

	std::vector<std::pair<ChartLine*, ChartLine*>> actionList;

	//if we don't exist
	if (it == lines.end()) {

		measures[measure].lines[localPos] = cLine;
		lines[absPos] = cLine;

		cLine->pos = absPos;
		cLine->measurePos = measure;
		it = lines.find(absPos);

		//check if we are not the first object
		if (it != lines.begin()) {
			actionList.push_back(std::make_pair(std::prev(it, 1)->second, new ChartLine(*std::prev(it, 1)->second)));
			connectLines(std::prev(it, 1), it);
			//we must push the change in the next line
		}

		//check if we are not the first object
		if (std::next(it, 1) != lines.end()) {
			actionList.push_back(std::make_pair(std::next(it, 1)->second, new ChartLine(*std::next(it, 1)->second)));
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



		//add to the undo list, the new pointer we created
		actionList.push_back(std::make_pair(nullptr, cLine));

	}
	//if the object already exists, merge them
	else {
		ChartLine* existingLine = lines[absPos];

		actionList.push_back(std::make_pair(existingLine, new ChartLine(*lines[absPos])));

		*existingLine += *cLine;

		delete cLine;
	}

	undoStack.push(actionList);
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
			std::vector<std::pair<ChartLine*, ChartLine*>> newList = lines.lower_bound(line)->second->clearBtHold(lane);
			actionList.insert(actionList.end(), newList.begin(), newList.end());
		}
	}
	if (type == ToolType::FX) {
		//check to see if we have a hold before and after our note
		if (lines.lower_bound(line)->second->fxVal[lane / 2] == 1 && lines.upper_bound(line)->second->fxVal[lane / 2] == 1) {
			std::vector<std::pair<ChartLine*, ChartLine*>> newList = lines.lower_bound(line)->second->clearFxHold(lane / 2);
			actionList.insert(actionList.end(), newList.begin(), newList.end());
		}
	}

	if (it != lines.end()) {


		if (type == ToolType::BT) {
			//remove chip
			if (it->second->btVal[lane] == 1) {
				actionList.push_back(std::make_pair(it->second, new ChartLine(*it->second)));
				it->second->btVal[lane] = 0;
			}
		}
		if (type == ToolType::FX) {
			//remove chip
			if (it->second->fxVal[lane / 2] == 2) {
				actionList.push_back(std::make_pair(it->second, new ChartLine(*it->second)));
				it->second->fxVal[lane / 2] = 0;
			}
		}
	}
	if (actionList.size() > 0) {
		undoStack.push(actionList);
	}
}


void Chart::undo() {
	if (undoStack.empty()) return;
	std::vector<std::pair<ChartLine*, ChartLine*>> redoBuffer;
	for (auto it : undoStack.top()) {
		if (it.first == nullptr) {
			redoBuffer.push_back(it);
			lines.erase(it.second->pos);
			measures[it.second->measurePos].lines.erase(it.second->pos - measures[it.second->measurePos].pos);
		}
		else {
			redoBuffer.push_back(std::make_pair(it.first, new ChartLine(*it.first)));
			*(it.first) = *(it.second);
			delete it.second;
		}
	}
	redoStack.push(redoBuffer);
	undoStack.pop();
}

void Chart::redo() {
	if (redoStack.empty()) return;
	std::vector<std::pair<ChartLine*, ChartLine*>> undoBuffer;
	for (auto it : redoStack.top()) {
		if (it.first == nullptr) {
			undoBuffer.push_back(it);
			lines[it.second->pos] = it.second;
			measures[it.second->measurePos].lines[it.second->pos - measures[it.second->measurePos].pos] = it.second;
		}
		else {
			undoBuffer.push_back(std::make_pair(it.first, new ChartLine(*it.first)));
			*(it.first) = *(it.second);
			delete it.second;
		}
	}
	undoStack.push(undoBuffer);
	redoStack.pop();
}


void Chart::clearRedoStack() {
	while (!redoStack.empty()) {
		for (auto it : redoStack.top()) {
			delete it.second;
		}
		redoStack.pop();
	}
}


void Chart::clearUndoStack() {
	while (!undoStack.empty()) {
		for (auto it : undoStack.top()) {
			delete it.second;
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