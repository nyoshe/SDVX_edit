#include "editWindow.h"
using namespace std::placeholders;

EditWindow::EditWindow() {
	Input::instance().addActionKey(sf::Event::KeyPressed, { sf::Keyboard::LControl, sf::Keyboard::Z }, std::bind(&EditWindow::undo, this, _1), "undo");
	Input::instance().addActionKey(sf::Event::KeyPressed, { sf::Keyboard::LControl, sf::Keyboard::Y }, std::bind(&EditWindow::redo, this, _1), "redo");
	Input::instance().addActionKey(sf::Event::KeyPressed, { sf::Keyboard::LControl, sf::Keyboard::C }, std::bind(&EditWindow::copy, this, _1), "copy");
	Input::instance().addActionKey(sf::Event::KeyPressed, { sf::Keyboard::LControl, sf::Keyboard::V }, std::bind(&EditWindow::paste, this, _1), "paste");
	Input::instance().addActionKey(sf::Event::KeyPressed, { sf::Keyboard::LControl, sf::Keyboard::X }, std::bind(&EditWindow::cut, this, _1), "cat");
	Input::instance().addActionKey(sf::Event::KeyPressed, { sf::Keyboard::Space, }, std::bind(&EditWindow::play, this, _1), "play");
	Input::instance().addActionKey(sf::Event::KeyPressed, { sf::Keyboard::Left }, std::bind(&EditWindow::moveLaserLeft, this, _1), "moveLaserLeft");
	Input::instance().addActionKey(sf::Event::KeyPressed, { sf::Keyboard::Right }, std::bind(&EditWindow::moveLaserRight, this, _1), "moveLaserRight");
	Input::instance().addActionKey(sf::Event::KeyPressed, { sf::Keyboard::Down }, std::bind(&EditWindow::moveLaserDown, this, _1), "moveLaserDown");
	Input::instance().addActionKey(sf::Event::KeyPressed, { sf::Keyboard::Up }, std::bind(&EditWindow::moveLaserUp, this, _1), "moveLaserUp");
	Input::instance().addEventAction(sf::Event::MouseWheelScrolled, std::bind(&EditWindow::mouseScroll, this, _1), "moveEditWindow");
	Input::instance().addAction(sf::Event::MouseButtonPressed, { sf::Keyboard::LAlt, sf::Keyboard::LControl }, { sf::Mouse::Left }, std::bind(&EditWindow::startSelect, this, _1), "selectStart");
	Input::instance().addAction(sf::Event::MouseButtonReleased, { sf::Keyboard::LAlt, sf::Keyboard::LControl }, { }, std::bind(&EditWindow::endSelect, this, _1), "endSelect");
	Input::instance().addActionMouse(sf::Event::MouseButtonPressed, { sf::Mouse::Left }, std::bind(&EditWindow::mousePressedLeft, this, _1), "mousePress");
	Input::instance().addActionMouse(sf::Event::MouseButtonReleased, { }, std::bind(&EditWindow::mouseReleasedLeft, this, _1), "mouseRelease");

	//bt button
	if (!entryTex.loadFromFile("textures/entryTex.png"))
		PLOG_WARNING << "failed to load entry sprite!";
	entrySprite.setTexture(entryTex);

	//bt button
	if (!btTex.loadFromFile("textures/button.png"))
		PLOG_WARNING << "failed to load bt sprite!";
	btSprite.setTexture(btTex);

	sf::Texture testTex;
	//btHold
	if (!testTex.loadFromFile("textures/buttonhold.png"))
		PLOG_WARNING << "failed to load fx sprite!";

	if (!btHoldTex.loadFromFile("textures/buttonhold.png", sf::IntRect(0, testTex.getSize().y - 1, testTex.getSize().x, testTex.getSize().y)))
		PLOG_WARNING << "failed to load fx sprite!";
	btHoldSprite.setTexture(btHoldTex);
	btHoldSprite.setTextureRect(sf::IntRect(0, btHoldTex.getSize().y - 1, btHoldTex.getSize().x, btHoldTex.getSize().y));

	//fx button
	if (!fxTex.loadFromFile("textures/fxbutton.png"))
		PLOG_WARNING << "failed to load fx sprite!";
	fxSprite.setTexture(fxTex);

	//fx hold
	if (!testTex.loadFromFile("textures/fxbuttonhold.png"))
		PLOG_WARNING << "failed to load fx sprite!";

	if (!fxHoldTex.loadFromFile("textures/fxbuttonhold.png", sf::IntRect(0, testTex.getSize().y - 1, testTex.getSize().x, testTex.getSize().y)))
		PLOG_WARNING << "failed to load fx sprite!";
	fxHoldSprite.setTexture(fxHoldTex);
	fxHoldSprite.setTextureRect(sf::IntRect(0, fxHoldTex.getSize().y - 1, fxHoldTex.getSize().x, fxHoldTex.getSize().y));

	font.loadFromFile("Fonts/CONSOLA.TTF");
}

void EditWindow::loadFile(std::string mapFilePath, std::string mapFileName) {
	Parser parser;
	Chart newChart;
	chart = newChart;
	parser.loadFile(mapFileName, chart);
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
	controlPtr->speed = 1.0;
	controlPtr->seekPos = 50;

	//An handle from the base address can identify any byte of the shared
	//memory segment even if it is mapped in different base addresses
	boost::interprocess::managed_shared_memory::handle_t handle = memSegment.get_handle_from_address(controlPtr);

	std::stringstream s;
	s << "start C:/Users/niayo/source/repos/unnamed-sdvx-clone/bin/usc-game_Debug.exe" << " " << handle << " C:/Users/niayo/source/repos/SDVX_edit/SDVX_edit/exh.ksh" << " -autoplay";
	//Launch child process
	//std::system(s.str().c_str());

	if(player.loadFile(mapFilePath + "\\" + chart.metadata.songFile))
		PLOG_INFO << "chart File \"" << chart.metadata.mapFileName << "\" successfully loaded";
	else
		PLOG_ERROR << "chart File \"" << chart.metadata.mapFileName << "\" unable to be loaded";

	int linesBefore = chart.lines.size();

	chart.minimize();

	chart.validateChart();

	PLOG_INFO << "minimized lines (" << linesBefore << "->" << chart.lines.size() << ")";
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
	height = window->getSize().y * columns;
	width = window->getSize().x / columns;
	laneWidth = static_cast<float>(window->getSize().x) / (10.0 * columns);
	columnWidth = static_cast<float>(window->getSize().x) / columns;
	measureHeight = float(height) / float(measuresPerColumn * columns);
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
	updateVars();
}

void EditWindow::setWindow(sf::RenderWindow* _window) {
	font.loadFromFile("Fonts/CONSOLA.TTF");
	window = _window;
	appWindow = _window;
	updateVars();
}

//returns the lane # the mouse is in, -1 if not found
int EditWindow::getMouseLane() {
	
	for (int j = 0; j < 4; j++) {
		if (
			mouseX >= (3 * laneWidth) + (laneWidth * j) &&
			mouseX <= (3 * laneWidth) + (laneWidth * (j + 1)))
		{
			return j;
		}
	}

	return -1;
}

//gives is the global location in 1/192 snapping
int EditWindow::getMouseLine() {

	if (mouseX >= 0 &&
		mouseX <= (10 * laneWidth)) {
		return editorLineStart - (mouseY / height) * viewLines;
	}
	return editorLineStart;
	return -1;
}

int EditWindow::getSnappedLine(int line) {
	if (line == -1) return -1;
	return (line / (192 / snapGridSize) * (192 / snapGridSize));
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

int EditWindow::getMeasureFromLine(unsigned int loc) {
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
			sf::Vertex(sf::Vector2f(v.x, v.y - 1), sf::Color(255, 255, 0)),
			sf::Vertex(sf::Vector2f(v.x + laneWidth * 4, v.y - 1), sf::Color(255, 255, 0))
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

void EditWindow::drawSelected(ChartLine* line, const sf::Sprite& sprite) {
	sf::RectangleShape rectangle(sf::Vector2f(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height));
	//rectangle.setOrigin(sprite.getOrigin());
	rectangle.setPosition(sf::Vector2f(sprite.getPosition().x, sprite.getPosition().y - sprite.getGlobalBounds().height));

	//rectangle.setScale(sprite.getScale());
	rectangle.setOutlineThickness(3.0);
	rectangle.setFillColor(sf::Color::Red);
	rectangle.setOutlineColor(sf::Color(255, 0, 0));

	window->draw(rectangle);
}

void EditWindow::drawLineButtons(ChartLine* line, bool selected) {
	int pos = line->pos;
	for (int lane = 0; lane < 2; lane++) {
		//fx chips
		if (line->fxVal[lane] == 2) {
			fxSprite.setPosition(getNoteLocation(lane * 2, pos));
			if (selected) drawSelected(line, fxSprite);
			window->draw(fxSprite);
		}
		//fx hold
		else if (line->fxVal[lane] == 1) {
			fxHoldSprite.setPosition(getNoteLocation(lane * 2, pos));
			if (line->next) fxHoldSprite.setScale(fxHoldSprite.getScale().x, (measureHeight / 192) * (line->next->pos - line->pos));
			if (selected) drawSelected(line, fxHoldSprite);
			window->draw(fxHoldSprite);
		}
	}

	if (DEBUG) {
		sf::Vector2f v = getNoteLocation(0, pos);
		sf::Vertex l[] = {
			sf::Vertex(sf::Vector2f(v.x - laneWidth*0.5, v.y), sf::Color(0, 255, 0,60)),
			sf::Vertex(sf::Vector2f(v.x + laneWidth*4.5, v.y), sf::Color(0, 255, 0,60))
		};
		window->draw(l, 2, sf::Lines);
	}

	for (int lane = 0; lane < 4; lane++) {
		//bt chips
		if (line->btVal[lane] == 1) {
			btSprite.setPosition(getNoteLocation(lane, pos));
			if (selected) drawSelected(line, btSprite);
			window->draw(btSprite);
		}
		//bt hold
		else if (line->btVal[lane] == 2) {
			btHoldSprite.setPosition(getNoteLocation(lane, pos));
			if (line->next) btHoldSprite.setScale(btHoldSprite.getScale().x, (measureHeight / 192) * (line->next->pos - line->pos));
			if (selected) drawSelected(line, btHoldSprite);
			window->draw(btHoldSprite);
		}
	}
}

QuadArray EditWindow::generateLaserQuads(int l, const std::map<unsigned int, ChartLine*>& objects, LineIterator startIter, LineIterator endIter) {
	std::vector<sf::VertexArray> laserBuffer;
	QuadArray vertexBuffer;
	if (editorMeasure >= chart.measures.size())
		return vertexBuffer;
	if (startIter == objects.end()) {
		return vertexBuffer;
	}
	if (endIter == objects.end()) {
		return vertexBuffer;
	}
	ChartLine* line = startIter->second;
	ChartLine* start = line;

	bool isSelected = false;
	while (line && line->pos <= (endIter->second->pos)) {
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
			!line->next ||
			line->next->laserPos[l] == -1 ||
			!line->getNextLaser(l)) {
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
			x = getNoteLocation(lineNum).x + getLaserX(line)[l];
			y = getNoteLocation(lineNum).y;

			quad[0] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);
			quad[1] = sf::Vertex(sf::Vector2f(x, y), c);
			quad[2] = sf::Vertex(sf::Vector2f(x, y + laneWidth), c);
			quad[3] = sf::Vertex(sf::Vector2f(x + laneWidth, y + laneWidth), c);
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
				quad[2] = sf::Vertex(sf::Vector2f(x + laneWidth, y - laneWidth * 1.0), c);
				quad[3] = sf::Vertex(sf::Vector2f(x, y - laneWidth * 1.0), c);
				//window->draw(quad);
				laserBuffer.push_back(quad);
			}

			vertexBuffer.push_back(std::make_pair(line, laserBuffer));
			laserBuffer.clear();

			line = nextLaser;
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

		quad[2] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);
		quad[3] = sf::Vertex(sf::Vector2f(x, y), c);

		laserBuffer.push_back(quad);

		if (nextLine->laserPos[l] >= 0 ||
			line->measurePos == nextLine->measurePos ||
			nextLine->measurePos > (editorMeasure * measuresPerColumn)) {
			vertexBuffer.push_back(std::make_pair(line, laserBuffer));
			laserBuffer.clear();
		}

		line = nextLine;
	}

	return vertexBuffer;
}

void EditWindow::drawChart(unsigned int start, unsigned int end) {
	int drawLine = start;
	while (drawLine <= end) {
		int m = getMeasureFromLine(drawLine);
		int add = drawMeasure(m, drawLine);
		drawLine += add;
	}

	for (int j = 0; j < 5; j++) {
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f((3 * laneWidth) + (laneWidth * j), getNoteLocation(start).y), sf::Color(100, 100, 100)),
			sf::Vertex(sf::Vector2f((3 * laneWidth) + (laneWidth * j), getNoteLocation(end).y), sf::Color(100, 100, 100))
		};
		window->draw(line, 2, sf::Lines);
	}


	if (selectedLines.size() > 1) {
		for (int l = 0; l < 2; l++) {
			QuadArray vertexBuffer = generateLaserQuads(l, selectedLines, selectedLines.begin(), std::prev(selectedLines.end(), 1));
			drawLaserQuads(vertexBuffer);
		}
	}


	for (auto const&  [key, val] : selectedLines) {
		drawLineButtons(val, true);
	}

	auto lineIt = chart.lines.lower_bound(start);
	if (lineIt != chart.lines.end()) {
		ChartLine* line = lineIt->second;
		while (line && line->pos <= end) {
			drawLineButtons(line, false);
			line = line->next;
		}
	}

	for (int l = 0; l < 2; l++) {
		QuadArray vertexBuffer = generateLaserQuads(l, chart.lines, chart.getLineBefore(start), chart.getLineAfter(end));
		checkLaserSelect(vertexBuffer, l);
		drawLaserQuads(vertexBuffer);
	}

	if (selectedLaser.second) {
		sf::Vector2f v = getNoteLocation(0, selectedLaser.second->pos);
		sf::RectangleShape rect(sf::Vector2f(laneWidth + 8, 8));
		rect.setPosition(sf::Vector2f(v.x + getLaserX(selectedLaser.second)[selectedLaser.first] - 4, v.y - 4));
		rect.setOutlineColor(sf::Color::Green);
		rect.setFillColor(sf::Color(0, 255, 0, 125));
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

	drawPlayBar();
}

void EditWindow::drawPlayBar()
{
	//draw the play bar
	if (player.isPlaying()) {
		for (int i = editorMeasure; i < editorMeasure + columns * measuresPerColumn + 1; i++) {
			if (i >= chart.measures.size()) break;
			if (chart.measures[i].msStart > player.getMs()) {
				float startMs = chart.measures[i - 1].msStart;
				float endMs = chart.measures[i].msStart;

				sf::Vector2f pos1 = getNoteLocation(-1, chart.measures[i - 1].pos);
				sf::Vector2f pos2 = getNoteLocation(5, chart.measures[i - 1].pos);

				pos1 = getNoteLocation(-1, chart.measures[i - 1].pos + chart.measures[i - 1].pulses * (player.getMs() - startMs) / (endMs - startMs));
				pos2 = getNoteLocation(5, chart.measures[i - 1].pos + chart.measures[i - 1].pulses * (player.getMs() - startMs) / (endMs - startMs));

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
	quad[2] = sf::Vertex(sf::Vector2f(xEnd + laneWidth, y - laneWidth * 1.0), c);
	quad[3] = sf::Vertex(sf::Vector2f(xEnd, y - laneWidth * 1.0), c);
	drawVec.push_back(quad);

	return drawVec;
}

void EditWindow::checkLaserSelect(QuadArray& arr, int laser) {
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
			if (vBuffer.first == laserHover.second) {
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

sf::Vector2f EditWindow::getMeasurePos(int measure) {
	int columnNum = measure / measuresPerColumn;
	return sf::Vector2f((4 * laneWidth) + (columnWidth * columnNum), topPadding + measureHeight * (measuresPerColumn - (measure % measuresPerColumn) - 1));
}

sf::Vector2f EditWindow::getSnappedPos(ToolType type) {
	//first check if we have a valid position
	int snappedLine = (getMouseLine() / (192 / snapGridSize)) * (192 / snapGridSize);
	sf::Vector2f start(laneWidth * 3, 0);
	float snapSize = measureHeight / snapGridSize;
	if (mouseValid) {
		switch (type) {
		case ToolType::BT:
			return sf::Vector2f(getNoteLocation(getMouseLane(), snappedLine));
		case ToolType::FX:
			return sf::Vector2f(getNoteLocation((getMouseLane() / 2) * 2, snappedLine));
		case ToolType::LASER_L:
			return sf::Vector2f(-1, -1);
			//placeholder
			//TODO
		}
	}
	return sf::Vector2f(-1, -1);
}

void EditWindow::handleEvent(sf::Event event) {
	if (event.mouseButton.button == sf::Mouse::Right && mouseValid && event.type == sf::Event::MouseButtonReleased) {
		if (mouseValid && !select && getMouseLane() != -1) {
			LineMask mask;
			switch (tool) {
			case ToolType::BT:
				mask.bt[getMouseLane()] = 1;
				break;
			case ToolType::FX:
				mask.fx[getMouseLane() / 2] = 1;
				break;
			}
			chart.removeChartLine(getSnappedLine(getMouseLine()), mask);
			chart.pushUndoBuffer();
			chart.clearRedoStack();
		}
	}

	if (event.type == sf::Event::MouseMoved) {
		if (mouseValid && state == EditorState::SELECTING) {
			selectEnd = getSnappedLine(getMouseLine());
		}
		if (mouseValid && state == EditorState::IDLE) {
			mouseDownLine = getSnappedLine(getMouseLine());
			mouseDownLane = getMouseLane();
		}
	}
}

sf::Vector2f EditWindow::getNoteLocation(int lane, int line) {
	line -= editorLineStart;
	//48 is the number of lines per beat
	float startX = (3 * laneWidth);
	float y = - height * (static_cast<float>(line) / static_cast<float>(viewLines));
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

	return areaSum < area + leniency && areaSum > area - leniency;
}

void EditWindow::update() {
	updateVars();
	laserHover.second = nullptr;

	sf::Clock deltaClock;
	//controlPtr->seekPos = 50;

	std::vector<sf::VertexArray> arr = generateSlamQuads(getSnappedLine(getMouseLine()), 25, (getMouseLaserPos(true) / laserMoveSize) * laserMoveSize, 1, true);

	for (auto quad : arr) {
		//window->draw(quad);
	}

	if (DEBUG) {
		drawDebug();
	}

	mouseValid = true;
	for (int i = 0; i < columns; i++) {
		sf::View view;
		view.setViewport(sf::FloatRect((1.f / columns) * i, 0.1, (1.f / columns) - (0.1/columns), 0.8));
		view.setSize(window->getSize().x /columns, measureHeight * measuresPerColumn);
		view.setCenter((window->getSize().x / columns) * 0.5, (measureHeight * measuresPerColumn) * 0.5 - (i+1) * (measureHeight * measuresPerColumn));
		window->setView(view);

		sf::Vector2i position = sf::Mouse::getPosition(*appWindow);

		if (view.getViewport().contains(static_cast<float>(position.x) / window->getSize().x ,static_cast<float>( position.y) / window->getSize().y)) {
			// convert it to world coordinates
			sf::Vector2f worldPos = appWindow->mapPixelToCoords(position);
			mouseX = worldPos.x;
			mouseY = worldPos.y;
			
		} else {
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
				mouseValid = false;
			}
		}
		drawChart(editorLineStart + pulsesPerColumn * i, editorLineStart + pulsesPerColumn * (i + 1));
		drawPlacementGuides();

		if (DEBUG) {
			sf::RectangleShape rect(sf::Vector2f(view.getSize().x - 4.0, view.getSize().y - 2.0));
			rect.setPosition(0 + 1.0, -(i + 1) * (measureHeight * measuresPerColumn) + 1.0);
			rect.setFillColor(sf::Color(124, 255, 0, 20));
			rect.setOutlineColor(sf::Color(0, 255, 0, 255));
			rect.setOutlineThickness(5.0);

			sf::Text txt;
			txt.setString(std::to_string(i));
			txt.setFont(font);
			txt.setPosition(rect.getPosition());
			window->draw(rect);
			window->draw(txt);
		}
	}
}

void EditWindow::drawPlacementGuides()
{
	if (!select && mouseValid) {
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
		if (getMouseLane() != -1 &&
			mouseDownLane != -1 &&
			mouseDownLine != getSnappedLine(getMouseLine()) &&
			getMouseLine() != -1){
			switch (state) {
			case EditorState::PLACING_BT:
				toolSprite = btHoldSprite;
				toolSprite.setColor(sf::Color(255, 255, 255, 128));
				toolSprite.setPosition(getNoteLocation(mouseDownLane, mouseDownLine));
				toolSprite.setScale(toolSprite.getScale().x, (measureHeight / 192) * (getSnappedLine(getMouseLine()) - mouseDownLine));
				window->draw(toolSprite);

				break;
			case EditorState::PLACING_FX:
				toolSprite = fxHoldSprite;
				toolSprite.setColor(sf::Color(255, 255, 255, 128));
				toolSprite.setPosition(getNoteLocation((mouseDownLane / 2 ) * 2, mouseDownLine));
				toolSprite.setScale(toolSprite.getScale().x, (measureHeight / 192) * (getSnappedLine(getMouseLine()) - mouseDownLine));
				window->draw(toolSprite);
				break;
			}
		}
	}

}

void EditWindow::drawDebug()
{
	/**/
	ImGui::Begin("Debug");

	ImGui::Text(std::string("X: " + std::to_string(mouseX) + " Y: " + std::to_string(mouseY).c_str()).c_str());
	//ImGui::Text(std::string("measure: " + std::to_string(getMouseMeasure())).c_str());
	ImGui::Text(std::string("Mouse Line: " + std::to_string(getMouseLine())).c_str());
	ImGui::Text(std::string("Mouse Line Snapped: " + std::to_string(getSnappedLine(getMouseLine()))).c_str());
	ImGui::Text(std::string("mouse diff: " + std::to_string((getSnappedLine(getMouseLine()) - mouseDownLine))).c_str());
	std::string stateStr = "";
	switch (state) {
	case EditorState::IDLE:
			stateStr = "idle";
			break;
	case EditorState::PLACING_BT:
		stateStr = "placing bt";
		break;
	case EditorState::PLACING_FX:
		stateStr = "placing fx";
		break;
	case EditorState::PLACING_LASER:
		stateStr = "placing laser";
		break;
	case EditorState::SELECTING:
		stateStr = "selecting";
		break;
	case EditorState::MODIFYING_LASER:
		stateStr = "modifying laser";
		break;
	case EditorState::PLAYING:
		stateStr = "playing";
		break;
	}
	ImGui::Text(std::string("state: " + stateStr).c_str());
	ImGui::Text(std::string("mouse valid? : " + std::to_string(mouseValid)).c_str());
	ImGui::Text(std::string("mouse down line: " + std::to_string(mouseDownLine)).c_str());
	ImGui::Text(std::string("mouse down lane: " + std::to_string(mouseDownLane)).c_str());
	ImGui::End();
	//appWindow
	//drawMap();
}

void EditWindow::undo(sf::Event event) {
	chart.pushUndoBuffer();
	selectedLaser = std::make_pair(0, nullptr);
	chart.undo();
}

void EditWindow::redo(sf::Event event) {
	chart.pushRedoBuffer();
	selectedLaser = std::make_pair(0, nullptr);
	chart.redo();
}

void EditWindow::copy(sf::Event event) {
	clipboard = chart.getSelection(selectStart, selectEnd, Mask::ALL);
}

void EditWindow::cut(sf::Event event)
{
	selectedLines.clear();
	clipboard = chart.getSelection(selectStart, selectEnd, Mask::ALL);
	for (auto line : clipboard) {
		chart.removeChartLine(line.first, line.second.makeMask());
	}
}

void EditWindow::paste(sf::Event event) {
	chart.clearRedoStack();
	unsigned int snapPos = getSnappedLine(selectStart);
	chart.insertChartLine(snapPos, clipboard);
	/*
	unsigned int lineBegin = 0;
	if (clipboard.begin() != clipboard.end()) lineBegin = clipboard.begin()->second.pos;
	for (auto line : clipboard) {
		chart.insertChartLine(snapPos + (line.second.pos - lineBegin), line.second);
	}
	*/
	chart.validateChart();
	chart.pushUndoBuffer();
}


void EditWindow::startSelect(sf::Event event) {
	if (getMouseLine() != -1) {
		selectStart = getSnappedLine(getMouseLine());
		selectEnd = getSnappedLine(getMouseLine());
		state = EditorState::SELECTING;
	}
}

void EditWindow::conenctLines(std::map<unsigned int, ChartLine*> input) {
	if (input.size() <= 1) return;

	auto i = input.begin();
	i->second->prev = nullptr;
	i++;
	for (; i != input.end(); i++) {
		i->second->prev = std::prev(i, 1)->second;
		std::prev(i, 1)->second->next = i->second;
	}
	input.begin()->second->prev = nullptr;
	std::prev(input.end(), 1)->second->next = nullptr;
}

void EditWindow::endSelect(sf::Event event) {
	if (getMouseLine() != -1 && state == EditorState::SELECTING) {
		selectEnd = getSnappedLine(getMouseLine());
		selectedLines.clear();

		std::map<unsigned int, ChartLine> out = chart.getSelection(selectStart, selectEnd, Mask::ALL);
		for (auto line : out) {
			selectedLines[line.second.pos] = new ChartLine(line.second);
		}
		conenctLines(selectedLines);
	}
	state = IDLE;
}

void EditWindow::play(sf::Event event) {
	if (player.isPlaying()) {
		player.stop();
		state = EditorState::IDLE;
	}
	else if (player.trackValid) {
		player.playFrom(chart.getMs(selectStart));
		state = EditorState::PLAYING;
	}
}

void EditWindow::moveLaserLeft(sf::Event event) {
	if (selectedLaser.second != nullptr) {
		ChartLine* laserToEdit = selectedLaser.second;
		chart.addUndoBuffer(laserToEdit);
		//should probably change modifyLaserPos to chart
		laserToEdit->modifyLaserPos(selectedLaser.first, -laserMoveSize);
	}
}

void EditWindow::moveLaserRight(sf::Event event) {
	if (selectedLaser.second != nullptr) {
		ChartLine* laserToEdit = selectedLaser.second;
		chart.addUndoBuffer(laserToEdit);
		laserToEdit->modifyLaserPos(selectedLaser.first, laserMoveSize);
	}
}

void EditWindow::moveLaserDown(sf::Event event) {
	if (selectedLaser.second != nullptr) {
		LineMask moveMask;
		moveMask.laser[selectedLaser.first] = 1;
		//chart.addUndoBuffer(selectedLaser.second);
		int moveDiff = getSnappedLine(selectedLaser.second->pos) - selectedLaser.second->pos;
		selectedLaser.second = chart.moveChartLine(selectedLaser.second->pos, moveMask, -192 / snapGridSize - moveDiff);
	}
}

void EditWindow::moveLaserUp(sf::Event event) {
	if (selectedLaser.second != nullptr) {
		LineMask moveMask;
		moveMask.laser[selectedLaser.first] = 1;
		//chart.addUndoBuffer(selectedLaser.second);
		int moveDiff = getSnappedLine(selectedLaser.second->pos) - selectedLaser.second->pos;
		selectedLaser.second = chart.moveChartLine(selectedLaser.second->pos, moveMask, 192 / snapGridSize + moveDiff);
	}
}

void EditWindow::mouseScroll(sf::Event event) {
	if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
		if (editorMeasure + event.mouseWheelScroll.delta * measuresPerColumn >= 0) {
			editorMeasure += event.mouseWheelScroll.delta * measuresPerColumn;
		}
	}
}

void EditWindow::mousePressedLeft(sf::Event event) {
	if (!mouseValid) return;
	if (select && laserHover.second && state == EditorState::IDLE) {
		int laserBeginPos = laserHover.second->pos;
		int laserEndPos = laserHover.second->getNextLaser(laserHover.first)->pos;
		int mousePos = getMouseLine();
		if (mousePos - laserBeginPos < laserEndPos - mousePos) {
			selectedLaser = laserHover;
		}
		else {
			selectedLaser = std::make_pair(laserHover.first, laserHover.second->getNextLaser(laserHover.first));
		}
		chart.pushUndoBuffer();
	}
	else {
		//deselect
		chart.pushUndoBuffer();
		selectedLaser = std::make_pair(0, nullptr);
	}
	if (!select && state == EditorState::IDLE) {
		switch (tool) {
		case ToolType::BT:
			state = EditorState::PLACING_BT;
			break;
		case ToolType::FX:
			state = EditorState::PLACING_FX;
			break;
		case ToolType::LASER_L:
		case ToolType::LASER_R:
			state = EditorState::PLACING_LASER;
			break;
		}
	}
}

void EditWindow::mouseReleasedLeft(sf::Event event) {
	if (state != EditorState::MODIFYING_LASER) {
		state = EditorState::IDLE;
	}
	if (!mouseValid) return;
	if (event.mouseButton.button != sf::Mouse::Left) return;

	if (mouseDownLane != -1) {
		int mouseLine = getSnappedLine(getMouseLine());
		if (mouseLine != mouseDownLine && mouseLine != -1) {
			chart.clearRedoStack();
			LineMask mask;

			std::map<unsigned int, ChartLine> holdMap;
			for (int i = mouseDownLine; i < mouseLine; i += (192 / snapGridSize)) {
				ChartLine newLine;
				switch (tool) {
				case ToolType::BT:
					newLine.btVal[mouseDownLane] = 2;
					mask.bt[mouseDownLane] = 1;
					break;
				case ToolType::FX:
					newLine.fxVal[mouseDownLane / 2] = 1;
					mask.fx[mouseDownLane / 2] = 1;
					break;
				};
				newLine.pos = i;
				holdMap[i] = newLine;
			}
			holdMap[mouseLine] = ChartLine();
			holdMap[mouseLine].pos = mouseLine;
			chart.insertChartLine(mouseDownLine, holdMap);
			chart.pushUndoBuffer();
		}
		else {
			ChartLine newLine;
			switch (tool) {
			case ToolType::BT:
				newLine.btVal[mouseDownLane] = 1;
				break;
			case ToolType::FX:
				newLine.fxVal[mouseDownLane / 2] = 2;
				break;
			};

			if (mouseValid && !select) {
				chart.clearRedoStack();
				chart.insertChartLine(getSnappedLine(getMouseLine()), newLine, newLine.makeMask());
				chart.pushUndoBuffer();
			}
		}
		
	}
}