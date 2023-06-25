#include "chartLine.h"



bool ChartLine::empty() {
    for (int i = 0; i < 4; i++) {
        if (btVal[i] == 1) {
            return false;
        }
        //we are considered non empty if we are the start of a hold note
        if (prev != nullptr) {
            if ((prev->btVal[i] == 0 || prev->btVal[i] == 1) && btVal[i] == 2) {
                return false;
            }
        }
    }
    for (int i = 0; i < 2; i++) {
        if (fxVal[i] == 2) {
            return false;
        }
        if (prev != nullptr) {
            if ((prev->fxVal[i] == 0 || prev->fxVal[i] == 2) && fxVal[i] == 1) {
                return false;
            }
        }
        if (laserPos[i] >= 0) {
            return false;
        }
        //we are considered empty only if we are the connector to a following laser point
        if (next != nullptr && prev != nullptr && laserPos[i] == -2) {
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
        if (b.isWide[i] && !isWide[i]) isWide[i] = 1;
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

void ChartLine::modifyLaserPos(int laser, int val) {
    if (laserPos[laser] + val >= 50) {
        laserPos[laser] = 50;
    }
    else if (laserPos[laser] + val <= 0) {
        laserPos[laser] = 0;
    }
    else {
        laserPos[laser] += val;
    }
}

ChartLine ChartLine::extractMask(LineMask mask) {
    ChartLine output;
    for (int i = 0; i < 4; i++) {
        if (mask.bt[i]) output.btVal[i] = btVal[i];
    }
    for (int i = 0; i < 2; i++) {
        if (mask.fx[i]) output.fxVal[i] = fxVal[i];
        if (mask.laser[i]) output.laserPos[i] = laserPos[i];
    }
    output.next = next;
    output.prev = prev;
    output.pos = pos;
    output.measurePos = measurePos;
    output.isWide = isWide;
    return output;
}

LineMask ChartLine::operator&(const LineMask& line) {
    LineMask out = line;
    for (int i = 0; i < 4; i++) {
        if (line.bt[i] && (btVal[i] != BT_HOLD)) out.bt[i] = 0;
    }
    for (int i = 0; i < 2; i++) {
        if (line.fx[i] && (fxVal[i] != FX_HOLD)) out.fx[i] = 0;
        if (line.laser[i] && (laserPos[i] == L_NONE)) out.laser[i] = 0;
    }
    return out;
}

void ChartLine::clearLaser(int laser) {
    laserPos[laser] = L_NONE;
    isWide[laser] = false;
}