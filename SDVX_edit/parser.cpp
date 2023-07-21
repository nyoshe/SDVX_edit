#include "Parser.h"

#include <set>

Command Parser::parseCommand(std::string s)
{
	Command c;
	int delim = s.find('=');
	std::string val = s.substr(delim + 1);
	std::string opt = s.substr(0, delim);
	auto it = cmdTable.find(opt);
	if (it != cmdTable.end()) {
		c.type = it->second;
		c.val = val;
	}
	else {
		//TODO: error
	}

	return c;
}

void Parser::loadFile(std::string fileName, Chart& chart)
{
	std::ifstream mapFile;
	mapFile.open(fileName);
	//ignore first 3 bytes
	mapFile.seekg(3);
	//Chart chart;

	if (mapFile.is_open()) {
		std::string s;
		std::string val;
		std::string opt;
		//switch statement
		const std::unordered_map<std::string, std::function<void()>> m{
			{"title", [&]() { chart.metadata.title = val; }},
			{"title_img", [&]() { chart.metadata.titleImg = val; }},
			{"artist", [&]() { chart.metadata.artist = val; }},
			{"artist_img", [&]() { chart.metadata.artistImg = opt; }},
			{"effect", [&]() { chart.metadata.effect = val; }},
			{"jacket", [&]() { chart.metadata.jacket = val; }},
			{"illustrator", [&]() { chart.metadata.illustrator = val; }},
			{"difficulty", [&]() { chart.metadata.difficulty = val; }},
			{"level", [&]() { chart.metadata.level = std::stoi(val); }},
			{"t", [&]() { chart.metadata.bpm = std::stof(val); }},
			{"to", [&]() { chart.metadata.bpmHi = std::stof(val); }},
			{"beat", [&]() { chart.metadata.beat = val; }},
			{"m", [&]() { chart.metadata.songFile = val; }},
			{"mvol", [&]() { chart.metadata.volume = stoi(val); }},
			{"o", [&]() { chart.metadata.offset = stoi(val); }},
			{"bg", [&]() { chart.metadata.bg = val; }},
			{"layer", [&]() { chart.metadata.layer = val; }},
			{"po", [&]() { chart.metadata.previewOffset = stoi(val); }},
			{"plength", [&]() { chart.metadata.previewLength = stoi(val); }},
			{"total", [&]() { chart.metadata.total = stoi(val); }},
			{"chokkakuvol", [&]() { chart.metadata.slamVol = stoi(val); }},
			{"chokkakuautovol", [&]() { chart.metadata.slamToVol = stoi(val); }},
			{"filtertype", [&]() { chart.metadata.filterType = val; }},
			{"pfiltergain", [&]() { chart.metadata.pFilterGain = stoi(val); }},
			{"pfilterdelay", [&]() { chart.metadata.pFilterDelay = stoi(val); }},
			{"v", [&]() { chart.metadata.videoFile = val; }},
			{"vo", [&]() { chart.metadata.videoOffset = stoi(val); }},
			{"ver", [&]() { chart.metadata.version = val; }},
			{"info", [&]() { chart.metadata.info = val; }},
			{"icon", [&]() { chart.metadata.icon = val; }},
		};
		//read header
		while (getline(mapFile, s)) {
			//chart start
			if (s == "--") {
				break;
			}
			int delim = s.find('=');
			val = s.substr(delim + 1);
			opt = s.substr(0, delim);

			auto it = m.find(opt);
			if (it != m.end() && val != "") {
				it->second();
			}
			else {
				//default behavior
			}
		}

		std::vector<ChartLine*> lineBuffer;
		std::vector<bool> laserWide = {false, false};

		std::vector<ChartLine*> lastLaser = {nullptr, nullptr};
		ChartLine* prev = nullptr;
		unsigned int pos = 0;

		int timeSigTop = std::stoi(chart.metadata.beat.substr(0, chart.metadata.beat.find('/')));
		int timeSigBottom = std::stoi(chart.metadata.beat.substr(chart.metadata.beat.find('/') + 1));

		unsigned int measurePos = 0;
		//read chart body
		while (getline(mapFile, s)) {
			//check for comments
			if (s.size() >= 2 && s.substr(0, 2) == "//") {
				continue;
			}
			std::vector<std::vector<ChartLine*>> laserLines[2];

			if (s == "") {
				continue;
			}
			//check for measure end
			if (s == "--") {
				Measure m;

				int length = lineBuffer.size();
				m.topSig = timeSigTop;
				m.bottomSig = timeSigBottom;
				m.pos = pos;
				lineBuffer[0]->isMeasureStart = true;
				for (int i = 0; i < lineBuffer.size(); i++) {
					lineBuffer[i]->pos = pos;
					lineBuffer[i]->measurePos = measurePos;
					m.lines.insert({pos - m.pos, lineBuffer[i]});
					chart.lines[pos] = lineBuffer[i];
					if (!lineBuffer[i]->cmds.empty()) {
						chart.cmds[pos] = lineBuffer[i]->cmds;
					}
					int linesPerMeasure = ((192 * timeSigTop) / timeSigBottom) / length;
					pos += linesPerMeasure;
				}
				m.pulses = ((192 * timeSigTop) / timeSigBottom);

				chart.measures.push_back(m);
				lineBuffer.clear();
				measurePos++;
				continue;
			}

			//check for commands, these always have equal signs
			std::vector<Command> commands;
			auto line = new ChartLine;

			while (s.find('=') != std::string::npos) {
				Command cmd = parseCommand(s);
				if (cmd.type == BEAT_CHANGE) {
					timeSigTop = std::stoi(cmd.val.substr(0, cmd.val.find('/')));

					//unnused
					timeSigBottom = std::stoi(cmd.val.substr(cmd.val.find('/') + 1));
				}

				commands.push_back(cmd);
				getline(mapFile, s);
			}

			line->cmds = commands;

			//now do buttons
			line->btVal[0] = s[0] - '0';
			line->btVal[1] = s[1] - '0';
			line->btVal[2] = s[2] - '0';
			line->btVal[3] = s[3] - '0';

			//fx
			line->fxVal[0] = s[5] - '0';
			line->fxVal[1] = s[6] - '0';

			//lasers
			for (int i = 0; i < 2; i++) {
				switch (s[8 + i]) {
				case '-':
					line->laserPos[i] = L_NONE;
					lastLaser[i] = nullptr;
					break;
				case ':':
					line->laserPos[i] = L_CONNECTOR;
					break;
				default:
					line->laserPos[i] = laserVals.find(s[8 + i]) / 50.f;
					break;
				}
			}

			for (auto it : line->cmds) {
				if (it.type == WIDE_LASER_L && line->laserPos[0] != L_NONE) {
					laserWide[0] = true;
				}
				else if (line->laserPos[0] == L_NONE) {
					laserWide[0] = false;
				}
				if (it.type == WIDE_LASER_R && line->laserPos[1] != L_NONE) {
					laserWide[1] = true;
				}
				else if (line->laserPos[1] == L_NONE) {
					laserWide[1] = false;
				}
			}
			line->isWide[0] = laserWide[0];
			line->isWide[1] = laserWide[1];

			//now check for spin commands cause they're special snowflakes
			if (s.size() > 10) {
				Command spinCommand;
				spinCommand.type = cmdTable.find(s.substr(10, 2))->second;
				//TODO: error if not found

				spinCommand.val = s.substr(12);

				line->cmds.push_back(spinCommand);
			}

			//doubly linked list stuff
			line->prev = prev;
			if (prev != nullptr) {
				prev->next = line;
			}

			prev = line;
			lineBuffer.push_back(line);
		}
	}
	chart.appendNewMeasure();
}

void Parser::saveFile(Chart& chart, std::string fileName)
{
	//TODO: handle comments
	std::ofstream mapFile;
	mapFile.open(fileName, std::ofstream::trunc);

	//write header
	unsigned char bom[] = {0xEF, 0xBB, 0xBF};
	mapFile.write((char*)bom, sizeof(bom));

	//swap the key and value of the command table
	std::unordered_map<CommandType, std::string> swappedCmdTable;
	for (auto i : cmdTable) {
		swappedCmdTable.emplace(i.second, i.first);
	}

	mapFile << "title=" << chart.metadata.title << std::endl;
	mapFile << "title_img=" << chart.metadata.titleImg << std::endl;
	mapFile << "artist=" << chart.metadata.artist << std::endl;
	mapFile << "artist_img=" << chart.metadata.artistImg << std::endl;
	mapFile << "effect=" << chart.metadata.effect << std::endl;
	mapFile << "jacket=" << chart.metadata.jacket << std::endl;
	mapFile << "illustrator=" << chart.metadata.illustrator << std::endl;
	mapFile << "difficulty=" << chart.metadata.difficulty << std::endl;
	mapFile << "level=" << chart.metadata.level << std::endl;
	mapFile << "t=" << chart.metadata.bpm << std::endl;
	mapFile << "to=" << chart.metadata.bpmHi << std::endl;
	mapFile << "beat=" << chart.metadata.beat << std::endl;
	mapFile << "m=" << chart.metadata.songFile << std::endl;
	mapFile << "mvol=" << chart.metadata.volume << std::endl;
	mapFile << "o=" << chart.metadata.offset << std::endl;
	mapFile << "bg=" << chart.metadata.bg << std::endl;
	mapFile << "layer=" << chart.metadata.layer << std::endl;
	mapFile << "po=" << chart.metadata.previewOffset << std::endl;
	mapFile << "plength=" << chart.metadata.previewLength << std::endl;
	mapFile << "total=" << chart.metadata.total << std::endl;
	mapFile << "chokkakuvol=" << chart.metadata.slamVol << std::endl;
	mapFile << "chokkakuautovol=" << chart.metadata.slamToVol << std::endl;
	mapFile << "filtertype=" << chart.metadata.filterType << std::endl;
	mapFile << "pfiltergain=" << chart.metadata.pFilterGain << std::endl;
	mapFile << "pfilterdelay=" << chart.metadata.pFilterDelay << std::endl;
	mapFile << "v=" << chart.metadata.videoFile << std::endl;
	mapFile << "vo=" << chart.metadata.videoOffset << std::endl;
	mapFile << "ver=" << chart.metadata.version << std::endl;
	mapFile << "info=" << chart.metadata.info << std::endl;
	mapFile << "icon=" << chart.metadata.icon << std::endl;

	mapFile << "--" << std::endl;

	for (auto measure : chart.measures) {
		for (auto line : measure.lines) {
			bool hasSpin = false;
			std::string spinVal = "";
			//first output commands
			for (auto command : line.second->cmds) {
				auto it = swappedCmdTable.find(command.type);
				switch (it->first) {
				case SPIN_R:
				case SPIN_L:
				case HALF_SPIN_R:
				case HALF_SPIN_L:
				case SWING_R:
				case SWING_L:
					hasSpin = true;
					spinVal = command.val;
					break;
				default:
					mapFile << it->second << "=" << command.val << std::endl;
					break;
				}
			}

			//output the actual line
			mapFile << static_cast<char>(line.second->btVal[0] + '0') << static_cast<char>(line.second->btVal[1] + '0')
				<< static_cast<char>(line.second->btVal[2] + '0') << static_cast<char>(line.second->btVal[3] + '0') <<
				"|";
			mapFile << static_cast<char>(line.second->fxVal[0] + '0') << static_cast<char>(line.second->fxVal[1] + '0')
				<< "|";
			for (int i = 0; i < 2; i++) {
				if (line.second->laserPos[i] == L_NONE) {
					mapFile << "-";
				}
				else if (line.second->laserPos[i] == L_CONNECTOR) {
					mapFile << ":";
				}
				else {
					mapFile << laserVals[std::clamp(static_cast<int>(std::lrint(line.second->laserPos[i] * 50.f)), 0,
					                                50)];
				}
			}

			//now do spins
			if (hasSpin) {
				mapFile << swappedCmdTable.find(line.second->cmds.back().type)->second << line.second->cmds.back().val;
			}

			mapFile << std::endl;
		}

		mapFile << "--" << std::endl;
	}

	mapFile.close();
	int x = 0;
}
