#include "Chart.h"
float Chart::getMs(unsigned int lineNum) {
    float currentMs = 0;
    unsigned int lastChange = 0;
    float currentBpm = bpm;
    int topSig = std::stoi(beat.substr(0, beat.find('/')));
    int bottomSig = std::stoi(beat.substr(beat.find('/') + 1));
    for (auto cList : cmds) {
        if (cList.first > lineNum) break;
        for (auto cmd : cList.second) {
            if (cmd.type == CommandType::TEMPO_CHANGE) {
                currentMs += ((cList.first - lastChange) / 192.0) * 1000.0 * topSig * (60.0 / currentBpm);
                lastChange = cList.first;
                currentBpm = std::stof(cmd.val);

            }
            else if (cmd.type == CommandType::BEAT_CHANGE) {
                currentMs += ((cList.first - lastChange) / 192.0) * 1000.0 * topSig * (60.0 / currentBpm);
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

ChartLine* Chart::insertChartLine(unsigned int measure, unsigned int line, ChartLine* cLine) {
	unsigned int absPos = measures[measure].pos + line;
	auto it = lines.find(absPos);

	std::vector<std::pair<ChartLine*, ChartLine*>> actionList;

	//if we don't exist
	if (it == lines.end()) {
		//add to the undo list, the new pointer we created
		actionList.push_back(std::make_pair(nullptr, cLine));

		measures[measure].lines[line] = cLine;
		lines[absPos] = cLine;

		cLine->pos = absPos;
		cLine->measurePos = measure;

		it = lines.find(absPos);

		if (it == lines.begin()) {
			it++;
			//check if we are not the only object
			if (it != lines.end()) {
				actionList.push_back(std::make_pair(it->second, new ChartLine(*it->second)));

				it->second->prev = cLine;
				cLine->next = it->second;
			}
			undoStack.push(actionList);
			return lines[absPos];
		}

		it++;
		//we are last object
		if (it == lines.end()) {
			it--;
			it--;
			actionList.push_back(std::make_pair(it->second, new ChartLine(*it->second)));

			it->second->next = cLine;
			cLine->prev = it->second;

			undoStack.push(actionList);
			return lines[absPos];
		}

		//we are middle object
		actionList.push_back(std::make_pair(it->second, new ChartLine(*it->second)));

		it->second->prev = cLine;
		cLine->next = it->second;
		it--;
		it--;

		actionList.push_back(std::make_pair(it->second, new ChartLine(*it->second)));

		it->second->next = cLine;
		cLine->prev = it->second;

		for (int i = 0; i < 2; i++) {
			if (cLine->prev->laserPos[i] == -2 || cLine->next->laserPos[i] == -2) {
				cLine->laserPos[i] = -2;
			}
			//extend hold notes
			if (cLine->prev->fxVal[i] == 1) {
				cLine->fxVal[i] = 1;
			}
		}
		for (int i = 0; i < 4; i++) {
			//extend hold notes
			if (cLine->prev->btVal[i] == 2) {
				cLine->btVal[i] = 2;
			}
		}
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
ChartLine* Chart::removeChartLine(unsigned int measure, unsigned int line, unsigned int lane, ToolType type) {
	unsigned int absPos = measures[measure].pos + line;
	auto it = lines.find(absPos);

	std::vector<std::pair<ChartLine*, ChartLine*>> actionList;

	if (it != lines.end()) {


		if (type == ToolType::BT) {
			//traverse list forward and back
			if (it->second->btVal[lane / 2] == 2) {
				ChartLine* cLine = it->second;
				while (cLine != nullptr && cLine->btVal[lane] == 2) {
					actionList.push_back(std::make_pair(cLine, new ChartLine(*cLine)));
					cLine->btVal[lane] = 0;
					cLine = cLine->prev;
				}
				cLine = it->second->next;
				while (cLine != nullptr && cLine->btVal[lane] == 2) {
					actionList.push_back(std::make_pair(cLine, new ChartLine(*cLine)));
					cLine->btVal[lane] = 0;
					cLine = cLine->next;
				}
			}
			//remove chip
			if (it->second->btVal[lane] == 1) {
				actionList.push_back(std::make_pair(it->second, new ChartLine(*it->second)));
				it->second->btVal[lane] = 0;
			}
		}
		if (type == ToolType::FX) {
			//traverse list forward and back
			if (it->second->fxVal[lane / 2] == 1) {
				ChartLine* cLine = it->second;
				while (cLine != nullptr && cLine->fxVal[lane / 2] == 1) {
					actionList.push_back(std::make_pair(cLine, new ChartLine(*cLine)));
					cLine->fxVal[lane / 2] = 0;
					cLine = cLine->prev;
				}
				cLine = it->second->next;
				while (cLine != nullptr && cLine->fxVal[lane / 2] == 1) {
					actionList.push_back(std::make_pair(cLine, new ChartLine(*cLine)));
					cLine->fxVal[lane / 2] = 0;
					cLine = cLine->next;
				}
			}
			//remove chip
			if (it->second->fxVal[lane / 2] == 2) {
				actionList.push_back(std::make_pair(it->second, new ChartLine(*it->second)));
				it->second->fxVal[lane / 2] = 0;
			}
		}

		undoStack.push(actionList);
	}
	else {
		return nullptr;
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