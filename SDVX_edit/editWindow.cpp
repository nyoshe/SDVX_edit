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
		unsigned int measure = chart.measures[getMouseMeasure()].pos;
		return getMouseLine(192) + measure;
	}
	return -1;
}

//probably slow as shit but it's fiiiiiine
int EditWindow::getMeasureFromGlobal(unsigned int loc) {
	int n = 0;
	if (chart.measures.empty()) return -1;
	for (auto m : chart.measures) {
		if (m.pos > loc) {
			return n - 1;
		}
		n++;
	}
	return 0;
}



//gets left x position of leaser start
std::vector <float> EditWindow::getLaserX(ChartLine* line) {
	std::vector <float> xVec = { 0, 0 };

	
	for (int i = 0; i < 2; i++) {
		float ourPos = 0;
		if (line->laserPos[i] == -2) {
			float startPos = line->prevLaser[i]->laserPos[i];
			float endPos = line->nextLaser[i]->laserPos[i];
			float diff = line->nextLaser[i]->pos - line->prevLaser[i]->pos;
			ourPos = startPos - (startPos - endPos) * (float(line->pos - line->prevLaser[i]->pos) / diff);
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

//TODO: add proper support for nunstandard time signatures eg. 20/4
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
	int pos = line->pos - chart.measures[line->measurePos].pos;
	int measureNum = line->measurePos - editorMeasure;
	for (int lane = 0; lane < 2; lane += 1) {
		//fx chips
		if (line->fxVal[lane] == 2) {
			fxSprite.setPosition(getNoteLocation(measureNum, lane * 2, pos));
			window->draw(fxSprite);
		}
		//fx hold
		else if (line->fxVal[lane] == 1) {
			fxHoldSprite.setPosition(getNoteLocation(measureNum, lane * 2, pos));
			fxHoldSprite.setScale(fxHoldSprite.getScale().x, (measureHeight / 192) * (line->next->pos - line->pos));
			window->draw(fxHoldSprite);
		}
	}


	for (int lane = 0; lane < 4; lane++) {
		//bt chips
		if (line->btVal[lane] == 1) {
			btSprite.setPosition(getNoteLocation(measureNum, lane, pos));
			window->draw(btSprite);
		}
		//bt hold
		else if (line->btVal[lane] == 2) {
			btHoldSprite.setPosition(getNoteLocation(measureNum, lane, pos));
			btHoldSprite.setScale(btHoldSprite.getScale().x, (measureHeight / 192) * (line->next->pos - line->pos));
			window->draw(btHoldSprite);
		}
	}
}

void EditWindow::drawChart() {

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
		if (editorMeasure >= chart.measures.size())
			break;
		ChartLine* line = chart.measures[editorMeasure].lines.begin()->second;
		ChartLine* start = line;
		while (line != nullptr && line->measurePos <= (editorMeasure + columns * measuresPerColumn)) {
		
			int measureNum = line->measurePos - editorMeasure;
			sf::Color c;
			if (l == 1) {
				c = sf::Color(255, 0, 200, 150);
			}
			else {
				c = sf::Color(0, 160, 255, 150);
			}

			if (line->laserPos[l] == -1 ||
				line->next == nullptr ||
				line->next->laserPos[l] == -1 ||
				line->nextLaser[l] == nullptr) {

				line = line->next;
				continue;
			}

			sf::VertexArray quad(sf::Quads, 4);
			int lineNum = line->pos - chart.measures[line->measurePos].pos;
			float x = 0;
			float y = 0;
			//draw entry point
			if (line->prevLaser[l] == nullptr && line->laserPos[l] >= 0) {
				//check wrapping
				if (line == chart.measures[measureNum + editorMeasure].lines.begin()->second) {
					x = getMeasureStart(measureNum - 1).x + getLaserX(line)[l];
					//TODO update this for weird time signatures
					y = getNoteLocation(measureNum - 1, 0, 192).y;
				}
				else {
					x = getMeasureStart(measureNum).x + getLaserX(line)[l];
					//TODO update this for weird time signatures
					y = getNoteLocation(measureNum, 0, lineNum).y;
				}

				quad[0] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);
				quad[1] = sf::Vertex(sf::Vector2f(x, y), c);
				quad[2] = sf::Vertex(sf::Vector2f(x, y + laneWidth), c);
				quad[3] = sf::Vertex(sf::Vector2f(x + laneWidth, y + laneWidth), c);
				window->draw(quad);

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

			//check for slams, this is not kson compliant and checks based on timing
			if ((line->nextLaser[l]->pos - line->pos) <= (192 / 32) &&
				 line->laserPos[l] != -2 &&
				 line->laserPos[l] != line->nextLaser[l]->laserPos[l]) {

				x = std::max(getLaserX(line)[l], getLaserX(line->nextLaser[l])[l]) + getMeasureStart(measureNum).x;
				y = getNoteLocation(measureNum, 0, lineNum).y;
				// build laser quad
				quad[0] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);
				quad[1] = sf::Vertex(sf::Vector2f(x + laneWidth, y - laneWidth / 2.0), c);
					
				x = std::min(getLaserX(line)[l], getLaserX(line->nextLaser[l])[l]) + getMeasureStart(measureNum).x;
				quad[2] = sf::Vertex(sf::Vector2f(x, y - laneWidth / 2.0), c);
				quad[3] = sf::Vertex(sf::Vector2f(x, y), c);

				window->draw(quad);
					
				line = line->nextLaser[l];

				//draw tail if we need it
				if (line->nextLaser[l] == nullptr) {
					x = getLaserX(line)[l] + getMeasureStart(measureNum).x;
					quad[0] = sf::Vertex(sf::Vector2f(x, y - laneWidth / 2.0), c);
					quad[1] = sf::Vertex(sf::Vector2f(x + laneWidth, y - laneWidth / 2.0), c);
					quad[2] = sf::Vertex(sf::Vector2f(x + laneWidth, y - laneWidth * 1.5), c);
					quad[3] = sf::Vertex(sf::Vector2f(x, y - laneWidth * 1.5), c);
					window->draw(quad);
				}
				line = line->next;
			}

			else {
				
				x = getLaserX(line)[l] + getMeasureStart(measureNum).x;
				y = getNoteLocation(measureNum, 0, lineNum).y;
				quad[0] = sf::Vertex(sf::Vector2f(x, y), c);
				quad[1] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);

				int nextLineNum = line->nextLaser[l]->pos - chart.measures[line->measurePos].pos;
				ChartLine* nextLine = line->nextLaser[l];
				//check wraping, cutoff laser early if we wrap the measure
				if (line->measurePos != line->nextLaser[l]->measurePos) {
					nextLine = chart.measures[line->measurePos + 1].lines.begin()->second;
					nextLineNum = nextLine->pos - chart.measures[line->measurePos].pos;
				}

				x = getLaserX(nextLine)[l] + getMeasureStart(measureNum).x;
				y = getNoteLocation(measureNum, 0, nextLineNum).y;
				// build laser quad
				quad[2] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);
				quad[3] = sf::Vertex(sf::Vector2f(x, y), c);
				line = nextLine;
				window->draw(quad);
			}
		}
	}
	
	int m = getMeasureFromGlobal(selectStart) - editorMeasure;
	if (m >= 0) {
		sf::Vector2f pos1 = getNoteLocation(m, -1, selectStart - chart.measures[m + editorMeasure].pos);
		sf::Vector2f pos2 = getNoteLocation(m, 5, selectStart - chart.measures[m + editorMeasure].pos);

		sf::Vertex line[] = {
			sf::Vertex(pos1, sf::Color(255, 0, 0)),
			sf::Vertex(pos2, sf::Color(255, 0, 0))
		};
		window->draw(line, 2, sf::Lines);
	}

	m = getMeasureFromGlobal(selectEnd) - editorMeasure;
	if (m >= 0) {
		sf::Vector2f pos1 = getNoteLocation(m, -1, selectEnd - chart.measures[m + editorMeasure].pos);
		sf::Vector2f pos2 = getNoteLocation(m, 5, selectEnd - chart.measures[m + editorMeasure].pos);

		sf::Vertex line[] = {
			sf::Vertex(pos1, sf::Color(255, 0, 0)),
			sf::Vertex(pos2, sf::Color(255, 0, 0))
		};
		window->draw(line, 2, sf::Lines);
	}
	
	//draw the play bar
	if (player.isPlaying()) {
		for (int i = 0; i < editorMeasure + measuresPerColumn * columns; i++) {
			if (i >= chart.measures.size()) break;
			if (chart.measures[i].msStart > player.getMs()) {

				float startMs = chart.measures[i - 1].msStart;
				float endMs = chart.measures[i].msStart;

				sf::Vector2f pos1 = getNoteLocation(i - 1 - editorMeasure, -1, 0);
				sf::Vector2f pos2 = getNoteLocation(i - 1 - editorMeasure, 5, 0);

				pos1.y -= measureHeight * (player.getMs() - startMs) / (endMs - startMs);
				pos2.y -= measureHeight * (player.getMs() - startMs) / (endMs - startMs);

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
				
				if (getMouseMeasure() != -1) {
					chart.clearRedoStack();
					chart.insertChartLine(getMouseMeasure(), getMouseLine() * 192 / snapGridSize, newLine);
				}
			}
		}
		if (event.mouseButton.button == sf::Mouse::Right) {
			if (getMouseMeasure() != -1) {
				chart.removeChartLine(getMouseMeasure(), getMouseLine() * 192 / snapGridSize, getMouseLane(), tool);
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


sf::Vector2f EditWindow::getNoteLocation(int measure, int lane, int line) {
	sf::Vector2f startPos = getMeasureStart(measure);
	float snapHieght = measureHeight / 192;
	return sf::Vector2f(startPos.x + lane * laneWidth, startPos.y + (192 - line) * snapHieght);
}

void EditWindow::update() {
	sf::Clock deltaClock;
	//controlPtr->seekPos = 50;
	sf::Vector2i position = sf::Mouse::getPosition(*window);
	mouseX = position.x * (width / float(window->getSize().x));
	mouseY = position.y * ((height + topPadding + bottomPadding) / float(window->getSize().y));

	

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
	

	ImGui::Begin("Debug");
	
	ImGui::Text(std::to_string(mouseX).c_str());
	ImGui::Text(std::to_string(mouseY).c_str());
	std::string s3 = "measure: " + std::to_string(getMouseMeasure());
	ImGui::Text(s3.c_str());
	std::string s4 = "lane: " + std::to_string(getMouseLane());
	ImGui::Text(s4.c_str());
	std::string s5 = "line: " + std::to_string(getMouseLine());
	ImGui::Text(s5.c_str());
	std::string s6 = "measure start x: " + std::to_string(getMeasureStart(getMouseMeasure()).x);
	ImGui::Text(s6.c_str());
	std::string s7 = "measure start y: " + std::to_string(getMeasureStart(getMouseMeasure()).y);
	ImGui::Text(s7.c_str());
	std::string s8 = "note start x: " + std::to_string(getNoteLocation(getMouseMeasure(), getMouseLane(), getMouseLine()).x);
	ImGui::Text(s8.c_str());
	std::string s9 = "note start y: " + std::to_string(getNoteLocation(getMouseMeasure(), getMouseLane(), getMouseLine()).y);
	ImGui::Text(s9.c_str());
	std::string s10 = "hoverWindow: " + std::to_string(ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow));
	ImGui::Text(s10.c_str());
	std::string s11 = "bufferSize: " + std::to_string(player.track.buffSize);
	ImGui::Text(s11.c_str());
	std::string s12 = "audioSamples: " + std::to_string(player.track.lastSampleLength);
	ImGui::Text(s12.c_str());
	
	ImGui::End();
	
	drawMap();
	drawChart();
}

