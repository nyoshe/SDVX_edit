#include "editWindow.h"
EditWindow::EditWindow() {
	//bt button
	if (!entryTex.loadFromFile("textures/entryTex.png"))
		std::cout << "failed to load entry sprite!";
	entrySprite.setTexture(entryTex);


	//bt button
	if (!btTex.loadFromFile("textures/button.png"))
		std::cout << "failed to load bt sprite!";
	btSprite.setTexture(btTex);



	sf::Texture testTex;
	//btHold
	if (!testTex.loadFromFile("textures/buttonhold.png"))
		std::cout << "failed to load fx sprite!";

	if (!btHoldTex.loadFromFile("textures/buttonhold.png", sf::IntRect(0, testTex.getSize().y - 1, testTex.getSize().x, testTex.getSize().y)))
		std::cout << "failed to load fx sprite!";
	btHoldSprite.setTexture(btHoldTex);
	btHoldSprite.setTextureRect(sf::IntRect(0, btHoldTex.getSize().y - 1, btHoldTex.getSize().x, btHoldTex.getSize().y));

	//fx button
	if (!fxTex.loadFromFile("textures/fxbutton.png"))
		std::cout << "failed to load fx sprite!";
	fxSprite.setTexture(fxTex);



	//fx hold
	if (!testTex.loadFromFile("textures/fxbuttonhold.png"))
		std::cout << "failed to load fx sprite!";

	if (!fxHoldTex.loadFromFile("textures/fxbuttonhold.png", sf::IntRect(0, testTex.getSize().y - 1, testTex.getSize().x, testTex.getSize().y)))
		std::cout << "failed to load fx sprite!";
	fxHoldSprite.setTexture(fxHoldTex);
	fxHoldSprite.setTextureRect(sf::IntRect(0, fxHoldTex.getSize().y - 1, fxHoldTex.getSize().x, fxHoldTex.getSize().y));
}

void EditWindow::loadFile(std::string mapFilePath, std::string mapFileName) {
	
	Parser p;
	Chart newChart;
	chart = newChart;
	p.loadFile(mapFileName, chart);
	chart.calcTimings();
	chart.metadata.mapFileName = mapFileName;
	chart.metadata.mapFilePath = mapFilePath;

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

	player.loadFile(mapFilePath + "\\" + chart.metadata.songFile);

	PLOG_INFO << "chart File \"" << chart.metadata.mapFileName << "\" successfully loaded";

	int linesBefore = chart.lines.size();

	chart.minimize();

	chart.validateChart();

	PLOG_INFO << "minimized lines (" << linesBefore << "->" << chart.lines.size() << ")";

	p.saveFile(chart, "test.ksh");
}

void EditWindow::saveFile(std::string fileName) {
	Parser p;
	p.saveFile(chart, fileName);
}

void EditWindow::saveFile() {
	saveFile(chart.metadata.mapFileName);
}

void EditWindow::updateVars() {
	//calculate lane Width, there is 11 lanes allocate per column
	laneWidth = width / (11 * columns);
	columnWidth = width / float(columns);
	measureHeight = float(height) / float(measuresPerColumn);
	viewLines = pulsesPerColumn * columns;

	editorLineStart = editorMeasure * 192;
	pulsesPerColumn = measuresPerColumn * 192;


	entrySprite.setScale(laneWidth / entrySprite.getTexture()->getSize().x, laneWidth / entrySprite.getTexture()->getSize().x);
	entrySprite.setOrigin(0, entrySprite.getTexture()->getSize().y);

	btSprite.setScale(laneWidth / btSprite.getTexture()->getSize().x, laneWidth / btSprite.getTexture()->getSize().x);
	btSprite.setOrigin(0, btSprite.getTexture()->getSize().y);

	btHoldSprite.setScale(laneWidth / btHoldSprite.getTexture()->getSize().x, 1);
	btHoldSprite.setOrigin(0, 1);

	fxSprite.setScale(2 * laneWidth / fxSprite.getTexture()->getSize().x, 2 * laneWidth / fxSprite.getTexture()->getSize().x);
	fxSprite.setOrigin(0, fxSprite.getTexture()->getSize().y);

	fxHoldSprite.setScale(2 * laneWidth / fxHoldSprite.getTexture()->getSize().x, 1);
	fxHoldSprite.setOrigin(0, 1);
}

void EditWindow::setWindow(sf::RenderTarget* _window) {
	window = _window;
	width = window->getSize().x;
	bottomPadding = 0;
	topPadding = 0;

	height = window->getSize().y;
	updateVars();
}

void EditWindow::setWindow(sf::RenderWindow* _window) {
	font.loadFromFile("Fonts/CONSOLA.TTF");
	window = _window;
	appWindow = _window;

	width = window->getSize().x;
	bottomPadding = window->getSize().y * 0.1;
	topPadding = window->getSize().y * 0.05;
	height = window->getSize().y - topPadding - bottomPadding;
	updateVars();
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

//gives is the global location in 1/192 snapping
int EditWindow::getMouseLine() {
	if (getMouseMeasure() != -1) {
		//unsigned int measure = chart.measures[getMouseMeasure()].pos;
		for (int i = 0; i < columns; i++) {
			if (mouseX >= (1 * laneWidth) + (columnWidth * i) &&
				mouseX <= (11 * laneWidth) + (columnWidth * i)) {
				return editorLineStart + (i + 1) * pulsesPerColumn  - ((mouseY - topPadding) / height) * pulsesPerColumn;
			}
		}
		return editorLineStart;
	}
	return -1;
}

int EditWindow::getMouseSnappedLine() {
	return (getMouseLine() / (192 / snapGridSize) * (192 / snapGridSize));
}

int EditWindow::getMouseLaserPos(bool isWide) {
	if (mouseY > (height + topPadding) || mouseY < topPadding) return -1;
	for (int i = 0; i < columns; i++) {
		if (mouseX >= (1 * laneWidth) + (columnWidth * i) &&
			mouseX <= (11 * laneWidth) + (columnWidth * i)) {
			int selectAreaWidth = isWide ? (9 * laneWidth) : (5 * laneWidth);
			int mouseLocal = (mouseX - ((6 * laneWidth) + (columnWidth * i)));
			return std::clamp((50 * mouseLocal / selectAreaWidth) + 25, 0, 50);
		}
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
	for (int i = startLine + 48; i < startLine + mPulses; i += 48) {
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
	if (editorLineStart + viewLines >= startLine + mPulses) {
		sf::Vector2f v = getNoteLocation(startLine + mPulses);
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(v.x, v.y), sf::Color(255, 255, 0)),
			sf::Vertex(sf::Vector2f(v.x + laneWidth * 4, v.y), sf::Color(255, 255, 0))
		};
		window->draw(line, 2, sf::Lines);
	}


	return mPulses;
}

void EditWindow::drawLineButtons(ChartLine* line) {
	int pos = line->pos;
	for (int lane = 0; lane < 2; lane ++) {
		//fx chips
		if (line->fxVal[lane] == 2) {
			fxSprite.setPosition(getNoteLocation(lane * 2, pos));
			sf::RectangleShape rectangle(sf::Vector2f(fxSprite.getTexture()->getSize()));
			rectangle.setPosition(fxSprite.getPosition());
			rectangle.setScale(fxSprite.getScale());
			rectangle.setOutlineThickness(2.0);
			rectangle.setOutlineColor(sf::Color(0, 255, 0));
			rectangle.setOrigin(fxSprite.getOrigin());
			//window->draw(rectangle);
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

QuadArray EditWindow::generateLaserQuads(int l, const std::map<unsigned int, ChartLine*> &objects, LineIterator startIter, LineIterator endIter){
	std::vector<sf::VertexArray> laserBuffer;
	QuadArray vertexBuffer;
	if (editorMeasure >= chart.measures.size())
		return vertexBuffer;
	if (startIter == objects.end()) {
		return vertexBuffer;
	}
	ChartLine* line = startIter->second;
	ChartLine* start = line;
	
	bool isSelected = false;
	while (line != nullptr && line->pos <= (endIter->second->pos)) {

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
			y = getNoteLocation(nextLineNum).y - height / pulsesPerColumn;
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

void EditWindow::drawChart(sf::RenderTarget* target) {
	window = target;
	drawChart();
}

void EditWindow::drawChart() {

	int drawLine = editorLineStart;
	while (drawLine <= editorLineStart + viewLines) {
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

	//draw note lines

	auto lineIt = chart.lines.lower_bound(editorLineStart);
	if (lineIt != chart.lines.end()) {
		ChartLine* line = lineIt->second;
		while (line != nullptr && line->pos <= editorLineStart + viewLines) {
			if (line->pos >= std::min(selectStart, selectEnd) &&
				line->pos <= std::max(selectStart, selectEnd))
			{
				fxSprite.setColor(sf::Color(255, 0, 0));
				btSprite.setColor(sf::Color(255, 0, 0));
			}
			drawLineButtons(line);

			fxSprite.setColor(sf::Color(255, 255, 255));
			btSprite.setColor(sf::Color(255, 255, 255));
			line = line->next;
		}
	}
	
	for (int l = 0; l < 2; l++) {
		QuadArray vertexBuffer = generateLaserQuads(l, chart.lines, chart.lines.lower_bound(editorLineStart), chart.getLineBefore(editorLineStart + viewLines + 1));
		checkLaserSelect(vertexBuffer, l);
		drawLaserQuads(vertexBuffer);
	}
	
	if (selectedLaserEnd.second != nullptr){
		sf::Vector2f v = getNoteLocation(0, selectedLaserEnd.second->pos);
		sf::RectangleShape rect(sf::Vector2f(laneWidth + 8, 8));
		rect.setPosition(sf::Vector2f(v.x + getLaserX(selectedLaserEnd.second)[selectedLaserEnd.first] - 4, v.y - 4));
		rect.setOutlineColor(sf::Color::Red);
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineThickness(2);
		window->draw(rect);
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

std::vector<sf::VertexArray> EditWindow::generateSlamQuads(int lineNum, int start, int end, int laser, bool isWide) {
	std::vector<sf::VertexArray> drawVec;
	float xStart = getNoteLocation(lineNum).x;
	float xEnd = getNoteLocation(lineNum).x;
	float y = getNoteLocation(lineNum).y;

	if (isWide) {
		xStart += start * (9 * laneWidth) / 50.0 - 3 * laneWidth;
		xEnd += end * (9 * laneWidth) / 50.0 - 3 * laneWidth;
	}
	else {
		xStart += start * (5 * laneWidth) / 50.0 - laneWidth;
		xEnd += end * (5 * laneWidth) / 50.0 - laneWidth;
	}

	sf::Color c;
	if (laser == 1) {
		//pink
		c = sf::Color(255, 0, 200, 150);
	}
	else {
		//blue
		c = sf::Color(0, 160, 255, 150);
	}

	sf::VertexArray quad(sf::Quads, 4);

	quad[0] = sf::Vertex(sf::Vector2f(xStart + laneWidth, y), c);
	quad[1] = sf::Vertex(sf::Vector2f(xStart, y), c);
	quad[2] = sf::Vertex(sf::Vector2f(xStart, y + laneWidth), c);
	quad[3] = sf::Vertex(sf::Vector2f(xStart + laneWidth, y + laneWidth), c);
	//window->draw(quad);
	drawVec.push_back(quad);

	if (xStart < xEnd) {
		quad[0] = sf::Vertex(sf::Vector2f(xStart, y), c);
		quad[1] = sf::Vertex(sf::Vector2f(xStart, y - laneWidth / 2.0), c);
		quad[2] = sf::Vertex(sf::Vector2f(xEnd + laneWidth, y - laneWidth / 2.0), c);
		quad[3] = sf::Vertex(sf::Vector2f(xEnd + laneWidth, y), c);
	}
	else {
		quad[0] = sf::Vertex(sf::Vector2f(xStart + laneWidth, y), c);
		quad[1] = sf::Vertex(sf::Vector2f(xStart + laneWidth, y - laneWidth / 2.0), c);
		quad[2] = sf::Vertex(sf::Vector2f(xEnd, y - laneWidth / 2.0), c);
		quad[3] = sf::Vertex(sf::Vector2f(xEnd, y), c);
	}

	drawVec.push_back(quad);

	quad[0] = sf::Vertex(sf::Vector2f(xEnd, y - laneWidth / 2.0), c);
	quad[1] = sf::Vertex(sf::Vector2f(xEnd + laneWidth, y - laneWidth / 2.0), c);
	quad[2] = sf::Vertex(sf::Vector2f(xEnd + laneWidth, y - laneWidth * 1.5), c);
	quad[3] = sf::Vertex(sf::Vector2f(xEnd, y - laneWidth * 1.5), c);
	//window->draw(quad);
	drawVec.push_back(quad);

	return drawVec;
}

void EditWindow::checkLaserSelect(QuadArray &arr, int laser) {
	
	//run over the buffer and then check for collision
	for (auto& vBuffer : arr) {
		if (vBuffer.first->laserPos[laser] == -2 || vBuffer.first->next->laserPos[laser] == -1) {
			vBuffer.first = vBuffer.first->getPrevLaser(laser);
		}
		bool laserSelect = false;
		for (auto quad : vBuffer.second) {
			if (getMouseOverlap(quad)) {
				laserSelect = true;
				laserHover = std::make_pair(laser, vBuffer.first);
			}
			if (vBuffer.first == selectedLaser.second && selectedLaser.first == laser) {
				laserSelect = true;
			}
		}
	}
}

void EditWindow::drawLaserQuads(const QuadArray& arr) {
	for (auto& vBuffer : arr) {
		for (auto quad : vBuffer.second) {
			window->draw(quad);

			if (vBuffer.first == laserHover.second || vBuffer.first == selectedLaser.second) {
				for (int i = 0; i < 4; i++) {
					quad[i].color = sf::Color(255, 255, 255, 100);
				}
				sf::Vector2f v = getNoteLocation(-1, vBuffer.first->pos);
				sf::Vertex l[] = {
					sf::Vertex(sf::Vector2f(v.x + getLaserX(vBuffer.first)[laserHover.first], v.y), sf::Color(0, 255, 0)),
					sf::Vertex(sf::Vector2f(v.x + getLaserX(vBuffer.first)[laserHover.first] - laneWidth, v.y), sf::Color(255, 0, 0))
				};
				//window->draw(l, 2, sf::Lines);
				window->draw(quad);

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
	int snappedLine = (getMouseLine() / (192 / snapGridSize)) * (192 / snapGridSize);
	sf::Vector2f start = getMeasureStart(measure);
	float snapSize = measureHeight / snapGridSize;
	if (measure != -1) {
		switch (type){
		case ToolType::BT:
			return sf::Vector2f(getNoteLocation(getMouseLane(), snappedLine));
		case ToolType::FX:
			return sf::Vector2f(getNoteLocation((getMouseLane() / 2) * 2, snappedLine));
		case ToolType::LASER_L:
			return sf::Vector2f(-1,-1);
			//placeholder
			//TODO
		}
	}
	return sf::Vector2f(-1, -1);
}


void EditWindow::handleEvent(sf::Event event) {
	if (event.type == sf::Event::MouseButtonPressed && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {

		if (event.mouseButton.button == sf::Mouse::Left) {

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
				if (getMouseLine() != -1) {
					selectStart = getMouseLine();
					selectEnd = getMouseLine();
				}
			}

			else if (getMouseLane() != -1) {
				ChartLine newLine;
				switch (tool) {
				case ToolType::BT:
					newLine.btVal[getMouseLane()] = 1;
					break;
				case ToolType::FX:
					newLine.fxVal[getMouseLane() / 2] = 2;
					break;
				};
				
				if (getMouseMeasure() != -1 && !select) {
					chart.clearRedoStack();
					chart.insertChartLine(getMouseSnappedLine(), newLine);
					chart.pushUndoBuffer();
				}
			}
			if (select && laserHover.second != nullptr) {
				selectedLaser = laserHover;
				selectedLaserEnd = std::make_pair(selectedLaser.first, selectedLaser.second->getNextLaser(selectedLaser.first));
				chart.pushUndoBuffer();
			}
			else {
				//deselect
				chart.pushUndoBuffer();
				selectedLaser = std::make_pair(0, nullptr);
				selectedLaserEnd = std::make_pair(0, nullptr);
			}
		}
		if (event.mouseButton.button == sf::Mouse::Right) {
			if (getMouseMeasure() != -1 && !select) {
				chart.removeChartLine(getMouseSnappedLine(), getMouseLane(), tool);
				chart.pushUndoBuffer();
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
				chart.pushUndoBuffer();
				selectedLaser = std::make_pair(0, nullptr);
				selectedLaserEnd = std::make_pair(0, nullptr);
				chart.undo();
			}
			if (event.key.code == sf::Keyboard::Y) {
				chart.pushRedoBuffer();
				selectedLaser = std::make_pair(0, nullptr);
				selectedLaserEnd = std::make_pair(0, nullptr);
				chart.redo();
			}
		}
		if (selectedLaser.second != nullptr) {
			if (event.key.code == sf::Keyboard::Left) {
				ChartLine* laserToEdit = selectedLaserEnd.second;
				chart.addUndoBuffer(laserToEdit);
				//should probably change modifyLaserPos to chart
				laserToEdit->modifyLaserPos(selectedLaser.first, -laserMoveSize);
			}
			if (event.key.code == sf::Keyboard::Right) {
				ChartLine* laserToEdit = selectedLaserEnd.second;
				chart.addUndoBuffer(laserToEdit);
				laserToEdit->modifyLaserPos(selectedLaser.first, laserMoveSize);
			}
			if (event.key.code == sf::Keyboard::Up) {
				LineMask moveMask;
				moveMask.laser[selectedLaser.first] = 1;
				chart.addUndoBuffer(selectedLaserEnd.second);
				selectedLaserEnd.second = chart.moveChartLine(selectedLaserEnd.second->pos, moveMask, 192 / snapGridSize);
			}
			if (event.key.code == sf::Keyboard::Down) {
				LineMask moveMask;
				moveMask.laser[selectedLaser.first] = 1;
				chart.addUndoBuffer(selectedLaserEnd.second);
				selectedLaserEnd.second = chart.moveChartLine(selectedLaserEnd.second->pos, moveMask, -192 / snapGridSize);
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
		getMouseLine() != -1) {
		if (event.type == sf::Event::MouseButtonReleased) {
			selectEnd = getMouseLine();
		}
	}
	
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && 
		sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
		sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) &&
		getMouseLine() != -1){

		if (event.type == sf::Event::MouseMoved) {
			selectEnd = getMouseLine();
		}
	}
}

sf::Vector2f EditWindow::getNoteLocation(int lane, int line) {
	line -= editorLineStart;
	//48 is the number of lines per beat
	int totalLines = pulsesPerColumn;
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
	updateVars();
	laserHover.second = nullptr;

	sf::Clock deltaClock;
	//controlPtr->seekPos = 50;
	sf::Vector2i position = sf::Mouse::getPosition(*appWindow);
	mouseX = position.x * (width / float(window->getSize().x));
	mouseY = position.y * ((height + topPadding + bottomPadding) / float(window->getSize().y));

	std::vector<sf::VertexArray> arr = generateSlamQuads(getMouseSnappedLine(), 25, (getMouseLaserPos(true) / laserMoveSize) * laserMoveSize, 1, true);

	for (auto quad : arr) {
		//window->draw(quad);
	}
	/**/
	ImGui::Begin("Debug");
	
	ImGui::Text(std::to_string(mouseX).c_str());
	ImGui::Text(std::to_string(mouseY).c_str());
	std::string s3 = "measure: " + std::to_string(getMouseMeasure());
	ImGui::Text(s3.c_str());
	std::string s4 = "lane: " + std::to_string(getMouseLane());
	ImGui::Text(s4.c_str());
	std::string s6 = "Mouse Line: " + std::to_string(getMouseLine());
	ImGui::Text(s6.c_str());
	std::string s7 = "Mouse Line Snapped: " + std::to_string(getMouseSnappedLine());
	ImGui::Text(s7.c_str());
	//std::string s8 = "note start x: " + std::to_string(getNoteLocation(getMouseGlobalLine()).x);
	//ImGui::Text(s8.c_str());
	//std::string s9 = "note start y: " + std::to_string(getNoteLocation(getMouseGlobalLine()).y);
	//ImGui::Text(s9.c_str());
	std::string s10 = "laserPos: " + std::to_string(getMouseLaserPos(false));
	ImGui::Text(s10.c_str());
	std::string s11 = "bufferSize: " + std::to_string(player.track.buffSize);
	ImGui::Text(s11.c_str());
	std::string s12 = "audioSamples: " + std::to_string(player.track.lastSampleLength);
	ImGui::Text(s12.c_str());
	
	ImGui::End();
	
	//drawMap();
	drawChart();


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

}

