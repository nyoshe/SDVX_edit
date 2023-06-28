#include "chartLine.h"

/*
LineMask::LineMask(const ChartLine& line) {
    for (int i = 0; i < 2; i++) {
        if (line.fxVal[i] > 0) {
            this->fx[i] = 1;
        }
        if (line.laserPos[i] != -1) {
            this->laser[i] = 1;
        }
    }
    for (int i = 0; i < 4; i++) {
        if (line.btVal[i] > 0) {
            this->bt[i] = 1;
        }
    }
}
*/
LineMask ChartLine::makeMask() const{
    LineMask out;
    for (int i = 0; i < 2; i++) {
        if (this->fxVal[i] > 0) {
            out.fx[i] = 1;
        }
        if (this->laserPos[i] != -1) {
            out.laser[i] = 1;
        }
    }
    for (int i = 0; i < 4; i++) {
        if (this->btVal[i] > 0) {
            out.bt[i] = 1;
        }
    }
    return out;
 }

bool ChartLine::empty() {
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
        if (next && prev && laserPos[i] == -2) {
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
    while (line && line->laserPos[laser] != -1) {
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
    while (line && line->laserPos[laser] != -1) {
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
    if (line->btVal[lane] != 2) return this;
    while (line->prev && line->prev->btVal[lane] == 2) {
        line = line->prev;
    }
    return line;
}

ChartLine* ChartLine::getBtEnd(int lane) {
    ChartLine* line = this;
    if (line->btVal[lane] != 2) return this;
    while (line->next && line->next->btVal[lane] == 2) {
        line = line->next;
    }
    return line;
}

ChartLine* ChartLine::getFxStart(int lane) {
    ChartLine* line = this;
    if (line->fxVal[lane] != 1) return this;
    while (line->prev && line->prev->fxVal[lane] == 1) {
        line = line->prev;
    }
    return line;
}

ChartLine* ChartLine::getFxEnd(int lane) {
    ChartLine* line = this;
    if (line->fxVal[lane] != 1) return this;
    while (line->next && line->next->fxVal[lane] == 1) {
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

//essentially overwrites if mask bits are set
ChartLine ChartLine::replaceMask(LineMask mask, const ChartLine& b) {
    ChartLine out = *this;
    for (int i = 0; i < 2; i++) {
        if (mask.laser[i]) {
            out.laserPos[i] = b.laserPos[i];
        }
        //a merge overwrites the laser position
        if (mask.fx[i]) {
            out.fxVal[i] = b.fxVal[i];
        }
        if (b.isWide[i] && !this->isWide[i]) out.isWide[i] = 1;
    }
    for (int i = 0; i < 4; i++) {
        if (mask.bt[i]) {
            out.btVal[i] = b.btVal[i];
        }
    }
    return out;
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
        if (line.laser[i] && (laserPos[i] >= 0)) out.laser[i] = 0;
    }
    return out;
}

void ChartLine::clearLaser(int laser) {
    laserPos[laser] = L_NONE;
    isWide[laser] = false;
}