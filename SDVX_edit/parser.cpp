#include "parser.h"


Command Parser::parseCommand(std::string s) {
	

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
				{"title",   [&]() { chart.title = val; }},
				{"title_img",   [&]() { chart.titleImg = val; }},
				{"artist",   [&]() { chart.artist = val; }},
				{"artist_img", [&]() { chart.artistImg = opt; }},
				{ "effect",   [&]() { chart.effect = val; } },
				{ "jacket",   [&]() { chart.jacket = val; } },
				{ "illustrator",   [&]() { chart.illustrator = val; } },
				{ "difficulty",   [&]() { chart.difficulty = val; } },
				{ "level",   [&]() { chart.level = std::stoi(val); } },
				{ "t",   [&]() { chart.bpm = std::stof(val); } },
				{ "to",   [&]() { chart.bpmHi = std::stof(val); } },
				{ "beat",   [&]() { chart.beat = val; } },
				{ "m",   [&]() { chart.songFile = val; } },
				{ "mvol",   [&]() { chart.volume = stoi(val); } },
				{ "o",   [&]() { chart.offset = stoi(val); } },
				{ "bg",   [&]() { chart.bg = val; } },
				{ "layer",   [&]() { chart.layer = val; } },
				{ "po",   [&]() { chart.previewOffset = stoi(val); } },
				{ "plength",   [&]() { chart.previewLength = stoi(val); } },
				{ "total",   [&]() { chart.total = stoi(val); } },
				{ "chokkakuvol",   [&]() { chart.slamVol = stoi(val); } },
				{ "chokkakuautovol",   [&]() { chart.slamToVol = stoi(val); } },
				{ "filtertype",   [&]() { chart.filterType = val; } },
				{ "pfiltergain",   [&]() { chart.pFilterGain = stoi(val); } },
				{ "pfilterdelay",   [&]() { chart.pFilterDelay = stoi(val); } },
				{ "v",   [&]() { chart.videoFile = val; } },
				{ "vo",   [&]() { chart.videoOffset = stoi(val); } },
				{ "ver",   [&]() { chart.version = val; } },
				{ "info",   [&]() { chart.info = val; } },
				{ "icon",   [&]() { chart.icon = val; } },
		};
		//read header
		while (getline(mapFile, s)) {
			//chart start
			if (s == "--") break;
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
		std::vector<bool> laserWide = { false, false };

		std::vector <ChartLine*> lastLaser = { nullptr, nullptr };
		ChartLine* prev = nullptr;
		unsigned int pos = 0;

		int timeSigTop = std::stoi(chart.beat.substr(0, chart.beat.find('/')));
		int timeSigBottom = std::stoi(chart.beat.substr(chart.beat.find('/') + 1));

		unsigned int measurePos = 0;
		//read chart body
		while (getline(mapFile, s)) {
			//check for comments
			if (s.size() >= 2 && s.substr(0, 2) == "//") continue;
			std::vector<std::vector<ChartLine *>> laserLines[2];

			if (s == "") continue;
			//check for measure end
			if (s == "--") {
				Measure m;
				
				int length = lineBuffer.size();
				m.topSig = timeSigTop;
				m.bottomSig = timeSigBottom;
				m.pos = pos;

				for (int i = 0; i < lineBuffer.size(); i++) {
					lineBuffer[i]->pos = pos;
					lineBuffer[i]->measurePos = measurePos;
					m.lines.insert({ pos - m.pos, lineBuffer[i]});
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
			ChartLine* line = new ChartLine;

			while (s.find('=') != std::string::npos) {
				Command cmd = parseCommand(s);
				if (cmd.type == CommandType::BEAT_CHANGE) {
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
			//-1 represents no laser, -2 represents connection
			for (int i = 0; i < 2; i++) {
				switch (s[8 + i]) {
				case '-':
					line->laserPos[i] = -1;
					lastLaser[i] = nullptr;
					break;
				case ':':
					line->laserPos[i] = -2;
					break;
				default:
					line->laserPos[i] = laserVals.find(s[8 + i]);
					break;
				}

				
			}

			for (auto it : line->cmds) {
				if (it.type == CommandType::WIDE_LASER_L && line->laserPos[0] != -1) {
					laserWide[0] = true;
				}
				else if (line->laserPos[0] == -1) {
					laserWide[0] = false;
				}
				if (it.type == CommandType::WIDE_LASER_R && line->laserPos[1] != -1) {
					laserWide[1] = true;
				}
				else if (line->laserPos[1] == -1) {
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

}

void Parser::saveFile(Chart& chart, std::string fileName)
{
	//TODO: handle comments
	std::ofstream mapFile;
	mapFile.open(fileName, std::ofstream::trunc);

	//write header
	unsigned char bom[] = { 0xEF,0xBB,0xBF };
	mapFile.write((char*)bom, sizeof(bom));

	//swap the key and value of the command table
	std::unordered_map<CommandType, std::string> swappedCmdTable;
	for (auto i : cmdTable)
		swappedCmdTable.emplace(i.second, i.first);

	mapFile << "title=" << chart.title << std::endl;
	mapFile << "title_img=" << chart.titleImg << std::endl;
	mapFile << "artist=" << chart.artist << std::endl;
	mapFile << "artist_img=" << chart.artistImg << std::endl;
	mapFile << "effect=" << chart.effect << std::endl;
	mapFile << "jacket=" << chart.jacket << std::endl;
	mapFile << "illustrator=" << chart.illustrator << std::endl;
	mapFile << "difficulty=" << chart.difficulty << std::endl;
	mapFile << "level=" << chart.level << std::endl;
	mapFile << "t=" << chart.bpm << std::endl;
	mapFile << "to=" << chart.bpmHi << std::endl;
	mapFile << "beat=" << chart.beat << std::endl;
	mapFile << "m=" << chart.songFile << std::endl;
	mapFile << "mvol=" << chart.volume << std::endl;
	mapFile << "o=" << chart.offset << std::endl;
	mapFile << "bg=" << chart.bg << std::endl;
	mapFile << "layer=" << chart.layer << std::endl;
	mapFile << "po=" << chart.previewOffset << std::endl;
	mapFile << "plength=" << chart.previewLength << std::endl;
	mapFile << "total=" << chart.total << std::endl;
	mapFile << "chokkakuvol=" << chart.slamVol << std::endl;
	mapFile << "chokkakuautovol=" << chart.slamToVol << std::endl;
	mapFile << "filtertype=" << chart.filterType << std::endl;
	mapFile << "pfiltergain=" << chart.pFilterGain << std::endl;
	mapFile << "pfilterdelay=" << chart.pFilterDelay << std::endl;
	mapFile << "v=" << chart.videoFile << std::endl;
	mapFile << "vo=" << chart.videoOffset << std::endl;
	mapFile << "ver=" << chart.version << std::endl;
	mapFile << "info=" << chart.info << std::endl;
	mapFile << "icon=" << chart.icon << std::endl;

	mapFile << "--" << std::endl;
	
	for (auto measure : chart.measures) {	
		for (auto line : measure.lines) {
			bool hasSpin = false;
			std::string spinVal = "";
			//first output commands
			for (auto command : line.second->cmds) {
				auto it = swappedCmdTable.find(command.type);
				switch (it->first) {
				case CommandType::SPIN_R:
				case CommandType::SPIN_L:
				case CommandType::HALF_SPIN_R:
				case CommandType::HALF_SPIN_L:
				case CommandType::SWING_R:
				case CommandType::SWING_L:
					hasSpin = true;
					spinVal = command.val;
					break;
				default:
					mapFile << it->second << "=" << command.val << std::endl;
					break;
				}
			}

			//output the actual line
			mapFile << char(line.second->btVal[0] + '0') << char(line.second->btVal[1] + '0') << char(line.second->btVal[2] + '0') << char(line.second->btVal[3] + '0') << "|";
			mapFile << char(line.second->fxVal[0] + '0') << char(line.second->fxVal[1] + '0') << "|";
			for (int i = 0; i < 2; i++) {
				switch (line.second->laserPos[i]) {
				case -1: mapFile << "-"; break;
				case -2: mapFile << ":"; break;
				default:
					mapFile << laserVals[line.second->laserPos[i]];
					break;
				}
			}

			//now do spins
			if (hasSpin){
				mapFile << swappedCmdTable.find(line.second->cmds.back().type)->second << line.second->cmds.back().val;
			}

			mapFile << std::endl;
		}

		mapFile << "--" << std::endl;
	}

	mapFile.close();
	int x = 0;
}