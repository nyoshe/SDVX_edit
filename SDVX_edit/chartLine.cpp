#include "chartLine.h"

bool ChartLine::empty() {
    for (int i = 0; i < 4; i++) {
        if (btVal[i]) {
            return false;
        }
    }
    for (int i = 0; i < 2; i++) {
        if (fxVal[i]) {
            return false;
        }
        if (laserPos[i] != -1) {
            return false;
        }
    }
    return true;
}
//merging
ChartLine& ChartLine::operator+=(const ChartLine& b) {
    for (int i = 0; i < 2; i++) {
        if (b.laserPos[i] != -1) {
            this->laserPos[i] = b.laserPos[i];
        }
        //a merge overwrites the laser position
        if (b.laserPos[i] >= 0) {
            this->laserPos[i] = b.laserPos[i];
        }
        if (b.fxVal[i] != 0) {
            this->fxVal[i] = b.fxVal[i];
        }
    }
    for (int i = 0; i < 4; i++) {
        //extend hold notes
        if (b.btVal[i] != 0) {
            this->btVal[i] = b.btVal[i];
        }
    }
    return *this;
}

ChartLine* ChartLine::getNextLaser(int laser) {
    ChartLine* line = this->next;
    while (line != nullptr && line->laserPos[laser] != -1) {
        if (line->laserPos[laser] == -1) {
            return nullptr;
        }
        if (line->laserPos[laser] >= 0) {
            return line;
        }
        line = line->next;
    }
    return nullptr;
}

ChartLine* ChartLine::getPrevLaser(int laser) {
    ChartLine* line = this->prev;
    while (line != nullptr && line->laserPos[laser] != -1) {
        if (line->laserPos[laser] == -1) {
            return nullptr;
        }
        if (line->laserPos[laser] >= 0) {
            return line;
        }
        line = line->prev;
    }
    return nullptr;
}

ChartLine* ChartLine::getBtStart(int lane) {
    ChartLine* line = this;
    while (line->prev != nullptr && line->btVal[lane] == 2) {
        line = line->prev;
    }
    return line;
}

ChartLine* ChartLine::getBtEnd(int lane) {
    ChartLine* line = this;
    while (line->next != nullptr && line->btVal[lane] == 2) {
        line = line->next;
    }
    return line;
}

ChartLine* ChartLine::getFxStart(int lane) {
    ChartLine* line = this;
    while (line->prev != nullptr && line->fxVal[lane] == 1) {
        line = line->prev;
    }
    return line;
}

ChartLine* ChartLine::getFxEnd(int lane) {
    ChartLine* line = this;
    while (line->next != nullptr && line->fxVal[lane] == 1) {
        line = line->next;
    }
    return line;
}

std::vector<std::pair<ChartLine*, ChartLine*>> ChartLine::clearBtHold(int lane) {
    std::vector<std::pair<ChartLine*, ChartLine*>> actionList;
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

std::vector<std::pair<ChartLine*, ChartLine*>> ChartLine::clearFxHold(int lane) {
    std::vector<std::pair<ChartLine*, ChartLine*>> actionList;
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