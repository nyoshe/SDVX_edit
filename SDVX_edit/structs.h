#pragma once
#include "enums.h"
#include "chartLine.h"
#include <vector>
#include <string>
#include <map>
#include <SFML/Graphics.hpp>

struct Measure {
	//division of the measure (eg. 32, 16, 12, etc)
	//the standard measure division is 192, it can go higher if the time signature is different

	//this defines the top part of the time signature, the number of beats per measure
	int topSig = 4;

	//this essentially defines our speed, if it's 4/2, there's essentially twice the measures for a given song
	int bottomSig = 4;

	//the ratio between the top and the bottom determines the number of lines per measure, eg 4/2 would be 192 * 2
	//2/4 would be 192 / 2

	std::map<int, ChartLine*> lines;

	unsigned int pulses = 192;

	//position of measure start in 192 snapping
	unsigned int pos = 0;
	float msStart = 0.0;
};
