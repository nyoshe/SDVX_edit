#include "editWindow.h"
void EditWindow::loadFile(std::string fileName) {
	Parser p;
	chart = p.loadFile(fileName);
	chart.calcTimings();

	struct shm_remove
	{
		shm_remove() { boost::interprocess::shared_memory_object::remove("MySharedMemory"); }
		~shm_remove() { boost::interprocess::shared_memory_object::remove("MySharedMemory"); }
	} remover;
	//Create a managed shared memory segment
	memSegment = boost::interprocess::managed_shared_memory(boost::interprocess::create_only, "MySharedMemory", 1000);

	controlPtr = static_cast<gameControl*>(memSegment.allocate(sizeof(gameControl)));
	controlPtr->speed = 20.0;
	controlPtr->seekPos = 50;


	//An handle from the base address can identify any byte of the shared
	//memory segment even if it is mapped in different base addresses
	boost::interprocess::managed_shared_memory::handle_t handle = memSegment.get_handle_from_address(controlPtr);

	std::stringstream s;
	s << "start C:/Users/niayo/source/repos/unnamed-sdvx-clone/bin/usc-game_Debug.exe" << " " << handle << " C:/Users/niayo/source/repos/SDVX_edit/SDVX_edit/exh.ksh" << " -autoplay";
	//Launch child process
	//std::system(s.str().c_str());

	player.loadFile(mapFilePath + "\\" + chart.songFile);

	p.saveFile(chart, "test.ksh");
}

void EditWindow::updateVars() {
	//calculate lane Width, there is 11 lanes allocate per column
	laneWidth = width / (11 * columns);
	columnWidth = width / columns;
	measureHeight = float(height) / measuresPerColumn;
}

void EditWindow::setWindow(sf::RenderWindow* _window) {
	window = _window;
	width = window->getSize().x;
	height = window->getSize().y - topPadding - bottomPadding;

	updateVars();
	
	//bt button
	if (!entryTex.loadFromFile("textures/entryTex.png"))
		std::cout << "failed to load entry sprite!";
	entrySprite.setTexture(entryTex);

	entrySprite.setScale(laneWidth / entrySprite.getTexture()->getSize().x, laneWidth / entrySprite.getTexture()->getSize().x);
	entrySprite.setOrigin(0, entrySprite.getTexture()->getSize().y);

	//bt button
	if (!btTex.loadFromFile("textures/button.png"))
		std::cout << "failed to load bt sprite!";
	btSprite.setTexture(btTex);

	btSprite.setScale(laneWidth/btSprite.getTexture()->getSize().x, laneWidth / btSprite.getTexture()->getSize().x);
	btSprite.setOrigin(0, btSprite.getTexture()->getSize().y);
	
	sf::Texture testTex;
	//btHold
	if (!testTex.loadFromFile("textures/buttonhold.png"))
		std::cout << "failed to load fx sprite!";

	if (!btHoldTex.loadFromFile("textures/buttonhold.png", sf::IntRect(0, testTex.getSize().y - 1, testTex.getSize().x, testTex.getSize().y)))
		std::cout << "failed to load fx sprite!";
	btHoldSprite.setTexture(btHoldTex);
	btHoldSprite.setTextureRect(sf::IntRect(0, btHoldTex.getSize().y - 1, btHoldTex.getSize().x, btHoldTex.getSize().y));
	//resize this when needed
	btHoldSprite.setScale(laneWidth / btHoldSprite.getTexture()->getSize().x, 1);
	btHoldSprite.setOrigin(0, 1);

	//fx button
	if (!fxTex.loadFromFile("textures/fxbutton.png"))
		std::cout << "failed to load fx sprite!";
	fxSprite.setTexture(fxTex);

	fxSprite.setScale(2 * laneWidth / fxSprite.getTexture()->getSize().x, 2 * laneWidth / fxSprite.getTexture()->getSize().x);
	fxSprite.setOrigin(0, fxSprite.getTexture()->getSize().y);

	
	//fx hold
	if (!testTex.loadFromFile("textures/fxbuttonhold.png"))
		std::cout << "failed to load fx sprite!";

	if (!fxHoldTex.loadFromFile("textures/fxbuttonhold.png", sf::IntRect(0, testTex.getSize().y - 1, testTex.getSize().x, testTex.getSize().y)))
		std::cout << "failed to load fx sprite!";
	fxHoldSprite.setTexture(fxHoldTex);
	fxHoldSprite.setTextureRect(sf::IntRect(0, fxHoldTex.getSize().y - 1, fxHoldTex.getSize().x, fxHoldTex.getSize().y));
	//resize this when needed
	fxHoldSprite.setScale(2 * laneWidth / fxHoldSprite.getTexture()->getSize().x, 1);
	fxHoldSprite.setOrigin(0, 1);
}

//returns the lane # the mouse is in, -1 if not found
int EditWindow::getMouseLane() {
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < 4; j++) {
			if (
				mouseX >= (4 * laneWidth) + (columnWidth * i) + (laneWidth * j) &&
				mouseX <= (4 * laneWidth) + (columnWidth * i) + (laneWidth * (j + 1)))
			{
				return j;
			}
		}
	}

	return -1;
}

//returns the measure mouse is in relative to the start of file, -1 if not found
int EditWindow::getMouseMeasure() {
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < measuresPerColumn; j++) {
			if (
				mouseX >= (1 * laneWidth) + (columnWidth * i) &&
				mouseX <= (11 * laneWidth) + (columnWidth * i) &&
				mouseY >= (topPadding + measureHeight * j) &&
				mouseY <= (topPadding + measureHeight * (j + 1)))
			{
				return i * measuresPerColumn + (measuresPerColumn - j - 1) + editorMeasure;
			}
		}
	}
	return -1;
}

//returns the line relative to the selected snapping value, -1 if not found
int EditWindow::getMouseLine() {
	for (int j = 0; j < measuresPerColumn; j++) {
		if (
			mouseY >= (topPadding + measureHeight * j) &&
			mouseY <= (topPadding + measureHeight * (j + 1)))
		{
			float pos = (topPadding + measureHeight * j) - mouseY;
			return (pos / measureHeight) * snapGridSize + snapGridSize;
		}
	}
	return -1;
}

int EditWindow::getMouseLine(int snapSize) {
	for (int j = 0; j < measuresPerColumn; j++) {
		if (
			mouseY >= (topPadding + measureHeight * j) &&
			mouseY <= (topPadding + measureHeight * (j + 1)))
		{
			float pos = (topPadding + measureHeight * j) - mouseY;
			return (pos / measureHeight) * snapSize + snapSize;
		}
	}
	return -1;
}

//gives is the global location in 1/192 snapping
int EditWindow::getMouseGlobalLine() {
	if (getMouseMeasure() != -1) {
		//unsigned int measure = chart.measures[getMouseMeasure()].pos;
		//return getMouseLine(192) + measure;
		for (int i = 0; i < columns; i++) {
			if (mouseX >= (1 * laneWidth) + (columnWidth * i) &&
				mouseX <= (11 * laneWidth) + (columnWidth * i)) {
				return editorLineStart + (i + 1) * beatsPerColumn * pulsesPerBeat - ((mouseY - topPadding) / height) * beatsPerColumn * pulsesPerBeat;
			}
		}
		return editorLineStart;
	}
	return -1;
}

int EditWindow::getMeasureFromGlobal(unsigned int loc) {
	int n = 0;
	if (chart.measures.empty()) return -1;
	auto nearest = chart.lines.lower_bound(loc);
	if (nearest == chart.lines.end()) {
		nearest = std::prev(nearest, 1);
	}
	return nearest->second->measurePos;
}



//gets left x position of leaser start
std::vector <float> EditWindow::getLaserX(ChartLine* line) {
	std::vector <float> xVec = { 0, 0 };

	
	for (int i = 0; i < 2; i++) {
		float ourPos = 0;
		if (line->laserPos[i] == -2) {
			ChartLine* prevLaser = line->getPrevLaser(i);
			ChartLine* nextLaser = line->getNextLaser(i);
			float startPos = prevLaser->laserPos[i];
			float endPos = nextLaser->laserPos[i];
			float diff = nextLaser->pos - prevLaser->pos;
			ourPos = startPos - (startPos - endPos) * (float(line->pos - prevLaser->pos) / diff);
		}
		else {
			ourPos = line->laserPos[i];
		}
		
		if (line->isWide[i]) {
			xVec[i] = ourPos * (9 * laneWidth) / 50.0 - 3 * laneWidth;
		}
		else {
			xVec[i] = ourPos * (5 * laneWidth) / 50.0 - laneWidth;
		}
	}
	return xVec;
	
}

int EditWindow::drawMeasure(unsigned int measure, unsigned int startLine) {
	int mPulses = 192;

	if (measure < chart.measures.size() && chart.measures.size() != 0) {
		mPulses = chart.measures[measure].pulses;
	}

	//draw in beats
	for (int i = startLine + pulsesPerBeat; i < startLine + mPulses; i += pulsesPerBeat) {
		sf::Vector2f v = getNoteLocation(i);
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(v.x, v.y), sf::Color(50, 50, 50)),
			sf::Vertex(sf::Vector2f(v.x + laneWidth * 4, v.y), sf::Color(50, 50, 50))
		};
		window->draw(line, 2, sf::Lines);
	}

	//draw the start line
	if (editorLineStart <= startLine) {
		sf::Vector2f v = getNoteLocation(startLine);
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(v.x, v.y), sf::Color(255, 255, 0)),
			sf::Vertex(sf::Vector2f(v.x + laneWidth * 4, v.y), sf::Color(255, 255, 0))
		};
		window->draw(line, 2, sf::Lines);
	}


	//draw the end line
	if (editorLineStart + beatsPerColumn * pulsesPerBeat * columns >= startLine + mPulses) {
		sf::Vector2f v = getNoteLocation(startLine + mPulses);
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(v.x, v.y), sf::Color(255, 255, 0)),
			sf::Vertex(sf::Vector2f(v.x + laneWidth * 4, v.y), sf::Color(255, 255, 0))
		};
		window->draw(line, 2, sf::Lines);
	}


	return mPulses;
}

void EditWindow::drawMap() {	
	for (int i = 0; i < columns; i++) {
		//draw measure lines
		for (int j = 0; j < (measuresPerColumn + 1); j++) {
			sf::Vertex line[] = {
				sf::Vertex(sf::Vector2f((4 * laneWidth) + (columnWidth * i) , topPadding + measureHeight * j), sf::Color(255, 255, 0)),
				sf::Vertex(sf::Vector2f((8 * laneWidth) + (columnWidth * i) , topPadding + measureHeight * j), sf::Color(255, 255, 0))
			};
			window->draw(line, 2, sf::Lines);
		}
	
		//draw horizontal lines
		for (int j = 0; j < measuresPerColumn; j++) {
			for (int k = 1; k < 4; k++) {
				sf::Vertex line[] = {
					sf::Vertex(sf::Vector2f((4 * laneWidth) + (columnWidth * i) , topPadding + (measureHeight * j) + ((measureHeight / 4) * k)), sf::Color(50, 50, 50)),
					sf::Vertex(sf::Vector2f((8 * laneWidth) + (columnWidth * i) , topPadding + (measureHeight * j) + ((measureHeight / 4) * k)), sf::Color(50, 50, 50))
				};
				window->draw(line, 2, sf::Lines);
			}
		}

		//draw vertical lines
		for (int j = 0; j < 5; j++) {
			sf::Vertex line[] = {
				sf::Vertex(sf::Vector2f((4 * laneWidth) + (columnWidth * i) + (laneWidth * j), topPadding), sf::Color(100, 100, 100)),
				sf::Vertex(sf::Vector2f((4 * laneWidth) + (columnWidth * i) + (laneWidth * j), height + topPadding), sf::Color(100, 100, 100))
			};
			window->draw(line, 2, sf::Lines);
		}
	}
}

void EditWindow::drawLineButtons(ChartLine* line) {
	int pos = line->pos;
	for (int lane = 0; lane < 2; lane += 1) {
		//fx chips
		if (line->fxVal[lane] == 2) {
			fxSprite.setPosition(getNoteLocation(lane * 2, pos));
			window->draw(fxSprite);
		}
		//fx hold
		else if (line->fxVal[lane] == 1) {
			fxHoldSprite.setPosition(getNoteLocation(lane * 2, pos));
			fxHoldSprite.setScale(fxHoldSprite.getScale().x, (measureHeight / 192) * (line->next->pos - line->pos));
			window->draw(fxHoldSprite);
		}
		if (DEBUG) {
			sf::Vector2f v = getNoteLocation(-1, pos);
			sf::Vertex l[] = {
				sf::Vertex(sf::Vector2f(v.x, v.y), sf::Color(0, 255, 0)),
				sf::Vertex(sf::Vector2f(v.x - laneWidth, v.y), sf::Color(0, 255, 0))
			};
			window->draw(l, 2, sf::Lines);
		}
	}


	for (int lane = 0; lane < 4; lane++) {
		//bt chips
		if (line->btVal[lane] == 1) {
			btSprite.setPosition(getNoteLocation(lane, pos));
			window->draw(btSprite);
		}
		//bt hold
		else if (line->btVal[lane] == 2) {
			btHoldSprite.setPosition(getNoteLocation(lane, pos));
			btHoldSprite.setScale(btHoldSprite.getScale().x, (measureHeight / 192) * (line->next->pos - line->pos));
			window->draw(btHoldSprite);
		}
	}
}

std::vector<std::pair<ChartLine*, std::vector<sf::VertexArray>>> EditWindow::generateLaserQuads(int l){
	std::vector<sf::VertexArray> laserBuffer;
	std::vector<std::pair<ChartLine*, std::vector<sf::VertexArray>>> vertexBuffer;
	if (editorMeasure >= chart.measures.size())
		return vertexBuffer;
	ChartLine* line = chart.lines.lower_bound(editorLineStart)->second;
	ChartLine* start = line;
	
	bool isSelected = false;
	while (line != nullptr && line->measurePos <= (editorMeasure + columns * measuresPerColumn)) {

		//int measureNum = line->measurePos - editorMeasure;
		sf::Color c;
		if (l == 1) {
			//pink
			c = sf::Color(255, 0, 200, 150);
		}
		else {
			//blue
			c = sf::Color(0, 160, 255, 150);
		}

		if (line->laserPos[l] == -1 ||
			line->next == nullptr ||
			line->next->laserPos[l] == -1 ||
			line->getNextLaser(l) == nullptr) {

			if (laserBuffer.size() > 0) {
				vertexBuffer.push_back(std::make_pair(line, laserBuffer));
			}
			laserBuffer.clear();
			line = line->next;
			continue;
		}

		sf::VertexArray quad(sf::Quads, 4);
		int lineNum = line->pos;
		float x = 0;
		float y = 0;

		//draw entry point
		if (line->getPrevLaser(l) == nullptr && line->laserPos[l] >= 0) {
			//check wrapping
			x = getNoteLocation(lineNum).x + getLaserX(line)[l];
			//TODO update this for weird time signatures
			y = getNoteLocation(lineNum).y;

			quad[0] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);
			quad[1] = sf::Vertex(sf::Vector2f(x, y), c);
			quad[2] = sf::Vertex(sf::Vector2f(x, y + laneWidth), c);
			quad[3] = sf::Vertex(sf::Vector2f(x + laneWidth, y + laneWidth), c);
			//window->draw(quad);
			laserBuffer.push_back(quad);

			quad[0].color = sf::Color(255, 255, 255);
			quad[1].color = sf::Color(255, 255, 255);
			quad[2].color = sf::Color(255, 255, 255);
			quad[3].color = sf::Color(255, 255, 255);
			quad[0].texCoords = sf::Vector2f(0.f, 0.f);
			quad[1].texCoords = sf::Vector2f(128.f, 0.f);
			quad[2].texCoords = sf::Vector2f(128.f, 128.f);
			quad[3].texCoords = sf::Vector2f(0.f, 128.f);
			window->draw(quad, &entryTex);
		}

		ChartLine* nextLaser = line->getNextLaser(l);
		//check for slams, this is not kson compliant and checks based on timing
		if ((nextLaser->pos - line->pos) <= (192 / 32) &&
			line->laserPos[l] != -2 &&
			line->laserPos[l] != nextLaser->laserPos[l]) {

			//vertexBuffer.push_back(std::make_pair(line, laserBuffer));
			//laserBuffer.clear();

			x = std::max(getLaserX(line)[l], getLaserX(nextLaser)[l]) + getNoteLocation(lineNum).x;
			y = getNoteLocation(lineNum).y;
			// build laser quad

			float diff = getNoteLocation(lineNum).y - getNoteLocation(nextLaser->pos).y;
			quad[0] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);
			quad[1] = sf::Vertex(sf::Vector2f(x + laneWidth, y - diff), c);

			x = std::min(getLaserX(line)[l], getLaserX(nextLaser)[l]) + getNoteLocation(lineNum).x;
			quad[2] = sf::Vertex(sf::Vector2f(x, y - diff), c);
			quad[3] = sf::Vertex(sf::Vector2f(x, y), c);

			//window->draw(quad);
			laserBuffer.push_back(quad);

			//draw tail if we need it
			if (nextLaser->getNextLaser(l) == nullptr) {
				x = getLaserX(nextLaser)[l] + getNoteLocation(lineNum).x;
				quad[0] = sf::Vertex(sf::Vector2f(x, y - diff), c);
				quad[1] = sf::Vertex(sf::Vector2f(x + laneWidth, y - diff), c);
				quad[2] = sf::Vertex(sf::Vector2f(x + laneWidth, y - laneWidth * 1.5), c);
				quad[3] = sf::Vertex(sf::Vector2f(x, y - laneWidth * 1.5), c);
				//window->draw(quad);
				laserBuffer.push_back(quad);

			}

			vertexBuffer.push_back(std::make_pair(line, laserBuffer));
			laserBuffer.clear();

			line = nextLaser;
			//line = line->next;
			continue;
		}


		x = getLaserX(line)[l] + getNoteLocation(lineNum).x;
		y = getNoteLocation(lineNum).y;
		quad[0] = sf::Vertex(sf::Vector2f(x, y), c);
		quad[1] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);

		int nextLineNum = line->getNextLaser(l)->pos;
		ChartLine* nextLine = line->getNextLaser(l);


		x = getLaserX(nextLine)[l] + getNoteLocation(lineNum).x;
		y = getNoteLocation(nextLineNum).y;

		//check wraping, cutoff laser early if we wrap the measure
		if (line->measurePos != nextLine->measurePos) {
			nextLine = chart.measures[line->measurePos + 1].lines.begin()->second;
			nextLineNum = nextLine->pos - 1;
			x = getLaserX(nextLine)[l] + getNoteLocation(lineNum).x;
			y = getNoteLocation(nextLineNum).y - height / (beatsPerColumn * pulsesPerBeat);
		}

		// build laser quad
		quad[2] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);
		quad[3] = sf::Vertex(sf::Vector2f(x, y), c);

		//window->draw(quad);
		laserBuffer.push_back(quad);

		if (nextLine->laserPos[l] >= 0 ||
			line->measurePos == nextLine->measurePos ||
			nextLine->measurePos > (editorMeasure + columns * measuresPerColumn)) {

			vertexBuffer.push_back(std::make_pair(line, laserBuffer));
			laserBuffer.clear();
		}

		line = nextLine;

	}

	return vertexBuffer;
}

void EditWindow::drawChart() {

	int drawLine = editorLineStart;
	while (drawLine <= editorLineStart + beatsPerColumn * columns * pulsesPerBeat) {
		int m = getMeasureFromGlobal(drawLine);
		int add = drawMeasure(m, drawLine);
		drawLine += add;
	}
	for (int i = 0; i < columns; i++) {
		//draw vertical lines
		for (int j = 0; j < 5; j++) {
			sf::Vertex line[] = {
				sf::Vertex(sf::Vector2f((4 * laneWidth) + (columnWidth * i) + (laneWidth * j), topPadding), sf::Color(100, 100, 100)),
				sf::Vertex(sf::Vector2f((4 * laneWidth) + (columnWidth * i) + (laneWidth * j), height + topPadding), sf::Color(100, 100, 100))
			};
			window->draw(line, 2, sf::Lines);
		}
	}

	for (int i = 0; i < (measuresPerColumn * columns); i++) {
		if ((i + editorMeasure) >= chart.measures.size())
			break;
		for (auto line : chart.measures[i + editorMeasure].lines) {
			if (line.second->pos >= std::min(selectStart, selectEnd) &&
				line.second->pos <= std::max(selectStart, selectEnd))
			{
				fxSprite.setColor(sf::Color(100, 255, 0));
				btSprite.setColor(sf::Color(100, 255, 0));
			}
			drawLineButtons(line.second);

			fxSprite.setColor(sf::Color(255, 255, 255));
			btSprite.setColor(sf::Color(255, 255, 255));
		}
	}

	for (int l = 0; l < 2; l++) {
		
		std::vector<std::pair<ChartLine*, std::vector<sf::VertexArray>>> vertexBuffer = generateLaserQuads(l);
		
		//run over the buffer and then check for collision
		for (auto& vBuffer : vertexBuffer) {
			if (vBuffer.first->laserPos[l] == -2) {
				vBuffer.first = vBuffer.first->getPrevLaser(l);
			}
			bool laserSelect = false;
			for (auto quad : vBuffer.second) {
				if (getMouseOverlap(quad)) {
					laserSelect = true;
					laserHover = std::make_pair(l, vBuffer.first);
				}
				if (vBuffer.first == selectedLaser.second && selectedLaser.first == l) {
					laserSelect = true;
				}
			}

			
			for (auto quad : vBuffer.second) {
				window->draw(quad);

				if (laserSelect) {
					for (int i = 0; i < 4; i++) {
						quad[i].color = sf::Color(255, 255, 255, 100);
					}
					sf::Vector2f v = getNoteLocation(-1, vBuffer.first->pos);
					sf::Vertex l[] = {
						sf::Vertex(sf::Vector2f(v.x, v.y), sf::Color(0, 255, 0)),
						sf::Vertex(sf::Vector2f(v.x - laneWidth, v.y), sf::Color(0, 255, 0))
					};
					window->draw(l, 2, sf::Lines);
					window->draw(quad);

				}
			}
		}
		
		vertexBuffer.clear();
	}
	

	sf::Vector2f pos1 = getNoteLocation(-1, selectStart);
	sf::Vector2f pos2 = getNoteLocation(5, selectStart);

	sf::Vertex line[] = {
		sf::Vertex(pos1, sf::Color(255, 0, 0)),
		sf::Vertex(pos2, sf::Color(255, 0, 0))
	};
	window->draw(line, 2, sf::Lines);



	pos1 = getNoteLocation(-1, selectEnd);
	pos2 = getNoteLocation(5, selectEnd);

	sf::Vertex line2[] = {
		sf::Vertex(pos1, sf::Color(255, 0, 0)),
		sf::Vertex(pos2, sf::Color(255, 0, 0))
	};
	window->draw(line2, 2, sf::Lines);

	
	//draw the play bar
	if (player.isPlaying()) {
		for (int i = 1; i < editorMeasure + measuresPerColumn * columns; i++) {
			if (i >= chart.measures.size()) break;
			if (chart.measures[i].msStart > player.getMs()) {

				float startMs = chart.measures[i - 1].msStart;
				float endMs = chart.measures[i].msStart;

				sf::Vector2f pos1 = getNoteLocation(-1, chart.measures[i-1].pos);
				sf::Vector2f pos2 = getNoteLocation(5, chart.measures[i-1].pos);

				pos1 = getNoteLocation(-1, chart.measures[i - 1].pos + chart.measures[i - 1].pulses * (player.getMs() - startMs) / (endMs - startMs));
				pos2 = getNoteLocation(5, chart.measures[i - 1].pos + chart.measures[i - 1].pulses * (player.getMs() - startMs) / (endMs - startMs));
				//pos2.y -= measureHeight * (player.getMs() - startMs) / (endMs - startMs);

				sf::Vertex line[] = {
					sf::Vertex(pos1, sf::Color(255, 0, 0)),
					sf::Vertex(pos2, sf::Color(255, 0, 0))
				};
				window->draw(line, 2, sf::Lines);
				break;
			}
		}
	}
}

sf::Vector2f EditWindow::getMeasureStart(int measure) {
	int columnNum = measure / measuresPerColumn;
	return sf::Vector2f((4 * laneWidth) + (columnWidth * columnNum), topPadding + measureHeight * (measuresPerColumn - (measure % measuresPerColumn) - 1));
}

sf::Vector2f EditWindow::getSnappedPos(ToolType type) {
	//first check if we have a valid position
	int measure = getMouseMeasure() - editorMeasure;
	sf::Vector2f start = getMeasureStart(measure);
	float snapSize = measureHeight / snapGridSize;
	if (measure != -1) {
		switch (type){
		case ToolType::BT:
			return sf::Vector2f(start.x + getMouseLane() * laneWidth, start.y + snapSize * (snapGridSize - getMouseLine()));
		case ToolType::FX:
			return sf::Vector2f(start.x + (getMouseLane() / 2) * (laneWidth * 2), start.y + snapSize * (snapGridSize - getMouseLine()));
		case ToolType::LASER_L:
			return sf::Vector2f(-1,-1);
			//placeholder
			//TODO
		}
	}
	return sf::Vector2f(-1, -1);
}


void EditWindow::handleEvent(sf::Event event) {
	if (event.type == sf::Event::MouseButtonPressed) {

		if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) return;

		if (event.mouseButton.button == sf::Mouse::Left) {

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
				if (getMouseGlobalLine() != -1) {
					selectStart = getMouseGlobalLine();
					selectEnd = getMouseGlobalLine();
				}
			}
			else if (getMouseLane() != -1) {
				ChartLine* newLine = new ChartLine;
				switch (tool) {
				case ToolType::BT:
					newLine->btVal[getMouseLane()] = 1;
					break;
				case ToolType::FX:
					newLine->fxVal[getMouseLane() / 2] = 2;
					break;
				};
				
				if (getMouseMeasure() != -1 && !select) {
					chart.clearRedoStack();
					chart.insertChartLine((getMouseGlobalLine() / (192 / snapGridSize)) * (192 / snapGridSize), newLine);
				}
			}
			if (select && laserHover.second != nullptr) {
				selectedLaser = laserHover;
			}
		}
		if (event.mouseButton.button == sf::Mouse::Right) {
			if (getMouseMeasure() != -1 && !select) {
				chart.removeChartLine((getMouseGlobalLine() / (192 / snapGridSize)) * (192 / snapGridSize), getMouseLane(), tool);
				chart.clearRedoStack();
			}
			
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Space) {
			if (player.isPlaying()) {
				player.stop();
			}
			else {
				player.playFrom(chart.getMs(selectStart));
			}
			controlPtr->paused = true;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){ 
			if (event.key.code == sf::Keyboard::Z) {
				chart.undo();
			}
			if (event.key.code == sf::Keyboard::Y) {
				chart.redo();
			}
		}
	}
	if (event.type == sf::Event::MouseWheelScrolled)
	{
		if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
			if (editorMeasure + event.mouseWheelScroll.delta * measuresPerColumn >= 0) {
				editorMeasure += event.mouseWheelScroll.delta * measuresPerColumn;
			}
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
		sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) &&
		getMouseGlobalLine() != -1) {
		if (event.type == sf::Event::MouseButtonReleased) {
			selectEnd = getMouseGlobalLine();
		}
	}
	
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && 
		sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
		sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) &&
		getMouseGlobalLine() != -1){

		if (event.type == sf::Event::MouseMoved) {
			selectEnd = getMouseGlobalLine();
		}
	}
}
/*
sf::Vector2f EditWindow::getNoteLocation(int measure, int lane, int line) {
	sf::Vector2f startPos = getMeasureStart(measure);
	float snapHieght = measureHeight / 192;
	return sf::Vector2f(startPos.x + lane * laneWidth, startPos.y + (192 - line) * snapHieght);
}
*/
sf::Vector2f EditWindow::getNoteLocation(int lane, int line) {
	line -= editorLineStart;
	//48 is the number of lines per beat
	int totalLines = beatsPerColumn * 48;
	int columnNum = line / totalLines;
	float startX = (4 * laneWidth) + (columnWidth * columnNum);
	float y = topPadding + height - height * (static_cast<float>(line % totalLines) / static_cast<float>(totalLines));
	return sf::Vector2f(startX + lane * laneWidth, y);
}

sf::Vector2f EditWindow::getNoteLocation(int line) {
	return getNoteLocation(0, line);
}

float EditWindow::triArea(sf::Vector2f A, sf::Vector2f B, sf::Vector2f C) {
	return std::abs((A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)) / 2);
}

//thanks to this stackoverflow question: https://stackoverflow.com/questions/5922027/how-to-determine-if-a-point-is-within-a-quadrilateral
bool EditWindow::getMouseOverlap(const sf::VertexArray quad) {
	float leniency = 0.1;
	float area = triArea(quad[0].position, quad[1].position, quad[2].position) + triArea(quad[2].position, quad[3].position, quad[0].position);
	float areaSum = 0;
	for (int i = 0; i < 4; i++) {
		areaSum += triArea(quad[i % 4].position, quad[(i + 1) % 4].position, sf::Vector2f(mouseX, mouseY));
	}

	if (areaSum < area + leniency && areaSum > area - leniency) {
		return true;
	}
	else {
		return false;
	}

}

void EditWindow::update() {
	laserHover.second = nullptr;

	editorLineStart = editorMeasure * 192;
	beatsPerColumn = measuresPerColumn * 4;
	sf::Clock deltaClock;
	//controlPtr->seekPos = 50;
	sf::Vector2i position = sf::Mouse::getPosition(*window);
	mouseX = position.x * (width / float(window->getSize().x));
	mouseY = position.y * ((height + topPadding + bottomPadding) / float(window->getSize().y));

	
	if (!select) {
		switch (tool) {
		case ToolType::BT:
			toolSprite = btSprite;
			break;
		case ToolType::FX:
			toolSprite = fxSprite;
			break;
		}
		if (getMouseLane() != -1) {
			toolSprite.setPosition(getSnappedPos(tool));
			toolSprite.setColor(sf::Color(255, 255, 255, 128));
			window->draw(toolSprite);
		}
	}
	
	

	ImGui::Begin("Debug");
	
	ImGui::Text(std::to_string(mouseX).c_str());
	ImGui::Text(std::to_string(mouseY).c_str());
	std::string s3 = "measure: " + std::to_string(getMouseMeasure());
	ImGui::Text(s3.c_str());
	std::string s4 = "lane: " + std::to_string(getMouseLane());
	ImGui::Text(s4.c_str());
	std::string s5 = "line: " + std::to_string(getMouseLine());
	ImGui::Text(s5.c_str());
	std::string s6 = "Mouse Line: " + std::to_string(getMouseGlobalLine());
	ImGui::Text(s6.c_str());
	std::string s7 = "Mouse Line Snapped: " + std::to_string((getMouseGlobalLine() / (192 / snapGridSize)) * (192 / snapGridSize));
	ImGui::Text(s7.c_str());
	//std::string s8 = "note start x: " + std::to_string(getNoteLocation(getMouseGlobalLine()).x);
	//ImGui::Text(s8.c_str());
	//std::string s9 = "note start y: " + std::to_string(getNoteLocation(getMouseGlobalLine()).y);
	//ImGui::Text(s9.c_str());
	std::string s10 = "hoverWindow: " + std::to_string(ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow));
	ImGui::Text(s10.c_str());
	std::string s11 = "bufferSize: " + std::to_string(player.track.buffSize);
	ImGui::Text(s11.c_str());
	std::string s12 = "audioSamples: " + std::to_string(player.track.lastSampleLength);
	ImGui::Text(s12.c_str());
	
	ImGui::End();
	
	//drawMap();
	drawChart();
}

