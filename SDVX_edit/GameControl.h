#pragma once
#include <cstdint>
#include <cstdlib> //std::system
#include <sstream>
#include <boost/interprocess/managed_shared_memory.hpp>

struct GameControl
{
	bool paused = true;
	bool seek = false;
	uint32_t seekPos = 0;
	float speed = 1.0;
	//vars
	boost::interprocess::managed_shared_memory memSegment;
	GameControl();
};

GameControl* controlPtr;
