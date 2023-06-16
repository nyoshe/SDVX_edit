#include "editWindow.h"
void editWindow::loadFile(std::string fileName) {
	parser p;
	chart = p.loadFile(fileName);

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

	p.saveFile(chart, "test.ksh");
}

void editWindow::updateVars() {
	//calculate lane Width, there is 11 lanes allocate per column
	laneWidth = width / (11 * columns);
	columnWidth = width / columns;
	measureHeight = float(height) / measuresPerColumn;
}

void editWindow::setWindow(sf::RenderWindow* _window) {
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
int editWindow::getMouseLane() {
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
int editWindow::getMouseMeasure() {
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
int editWindow::getMouseLine() {
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

int editWindow::getMouseLine(int snapSize) {
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
int editWindow::getMouseGlobalLine() {
	if (getMouseMeasure() != -1) {
		unsigned int measure = chart.measures[getMouseMeasure()].pos;
		return getMouseLine(192) + measure;
	}
	return -1;
}

//probably slow as shit but it's fiiiiiine
unsigned int editWindow::getMeasureFromGlobal(unsigned int loc) {
	int n = 0;
	for (auto m : chart.measures) {
		if (m.pos > loc) {
			return n - 1;
		}
		n++;
	}
	return 0;
}

ChartLine* editWindow::insertChartLine(unsigned int measure, unsigned int line, ChartLine* cLine) {
	line = line * 192 / snapGridSize;
	unsigned int absPos = chart.measures[measure].pos + line;
	auto it = chart.lines.find(absPos);

	std::vector<std::pair<ChartLine*, ChartLine*>> actionList;

	if (it == chart.lines.end()) {
		//add to the undo list, the new pointer we created
		actionList.push_back(std::make_pair(nullptr, cLine));

		chart.measures[measure].lines[line] = cLine;
		chart.lines[absPos] = cLine;

		cLine->pos = absPos;
		cLine->measurePos = measure;

		it = chart.lines.find(absPos);

		if (it == chart.lines.begin()) {
			it++;
			//check if we are the ONLY object
			if (it == chart.lines.end()) {
				//do nothing
			}
			//we are first object
			else {
				ChartLine* newPtr = new ChartLine(*it->second);
				actionList.push_back(std::make_pair(it->second, newPtr));

				it->second->prev = cLine;
				cLine->next = it->second;

			}
		}
		else {
			it++;
			//we are last object
			if (it == chart.lines.end()) {
				it--;
				it--;
				ChartLine* newPtr = new ChartLine(*it->second);
				actionList.push_back(std::make_pair(it->second, newPtr));

				it->second->next = cLine;
				cLine->prev = it->second;

			}
			//we are middle object
			else {
				ChartLine* newPtr = new ChartLine(*it->second);
				actionList.push_back(std::make_pair(it->second, newPtr));

				it->second->prev = cLine;
				cLine->next = it->second;
				it--;
				it--;

				ChartLine* newPtr2 = new ChartLine(*it->second);
				actionList.push_back(std::make_pair(it->second, newPtr2));

				it->second->next = cLine;
				cLine->prev = it->second;

				for (int i = 0; i < 2; i++) {
					if (cLine->prev->laserPos[i] == -2 || cLine->next->laserPos[i] == -2) {
						cLine->laserPos[i] = -2;
					}
					//extend hold notes
					if (cLine->prev->fxVal[i] == 1) {
						cLine->fxVal[i] = 1;
					}
				}
				for (int i = 0; i < 4; i++) {
					//extend hold notes
					if (cLine->prev->btVal[i] == 2) {
						cLine->btVal[i] = 2;
					}
				}
			}
		}
		
	}

	//if the object already exists, merge them
	else {
		ChartLine* existingLine = chart.lines[absPos];

		actionList.push_back(std::make_pair(existingLine, new ChartLine(*chart.lines[absPos])));

		*existingLine += *cLine;

		delete cLine;
	}

	undoStack.push(actionList);

	return chart.lines[absPos];
}

ChartLine* editWindow::removeChartLine(unsigned int measure, unsigned int line, ToolType type) {
	line = line * 192 / snapGridSize;
	unsigned int absPos = chart.measures[measure].pos + line;
	auto it = chart.lines.find(absPos);

	std::vector<std::pair<ChartLine*, ChartLine*>> actionList;

	if (it != chart.lines.end()) {
		

		if (type == ToolType::BT) {
			//traverse list forward and back
			if (it->second->btVal[getMouseLane() / 2] == 2) {
				ChartLine* cLine = it->second;
				while (cLine != nullptr && cLine->btVal[getMouseLane() / 2] == 2) {
					actionList.push_back(std::make_pair(cLine, new ChartLine(*cLine)));
					cLine->btVal[getMouseLane() / 2] = 0;
					cLine = cLine->prev;
				}
				cLine = it->second->next;
				while (cLine != nullptr && cLine->btVal[getMouseLane() / 2] == 2) {
					actionList.push_back(std::make_pair(cLine, new ChartLine(*cLine)));
					cLine->btVal[getMouseLane() / 2] = 0;
					cLine = cLine->next;
				}
			}
			//remove chip
			if (it->second->btVal[getMouseLane()] == 1) {
				actionList.push_back(std::make_pair(it->second, new ChartLine(*it->second)));
				it->second->btVal[getMouseLane()] = 0;
			}
		}
		if (type == ToolType::FX) {
			//traverse list forward and back
			if (it->second->fxVal[getMouseLane() / 2] == 1) {
				ChartLine* cLine = it->second;
				while (cLine != nullptr && cLine->fxVal[getMouseLane() / 2] == 1) {
					actionList.push_back(std::make_pair(cLine, new ChartLine(*cLine)));
					cLine->fxVal[getMouseLane() / 2] = 0;
					cLine = cLine->prev;
				}
				cLine = it->second->next;
				while (cLine != nullptr && cLine->fxVal[getMouseLane() / 2] == 1) {
					actionList.push_back(std::make_pair(cLine, new ChartLine(*cLine)));
					cLine->fxVal[getMouseLane() / 2] = 0;
					cLine = cLine->next;
				}
			}
			//remove chip
			if (it->second->fxVal[getMouseLane()/2] == 2) {
				actionList.push_back(std::make_pair(it->second, new ChartLine(*it->second)));
				it->second->fxVal[getMouseLane()/2] = 0;
			}
		}

		undoStack.push(actionList);
	}
	else {
		return nullptr;
	}
	
}
//gets left x position of leaser start
std::vector <float> editWindow::getLaserX(ChartLine* line) {
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
void editWindow::drawMap() {	
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



void editWindow::drawChart() {

	for (int i = 0; i < (measuresPerColumn * columns); i++) {
		if ((i + editorMeasure) >= chart.measures.size())
			break;
		for (auto line : chart.measures[i + editorMeasure].lines) {
			for (int lane = 0; lane < 2; lane += 1) {
				//fx chips
				if (line.second->fxVal[lane] == 2) {
					fxSprite.setPosition(getNoteLocation(i, lane * 2, line.first));
					window->draw(fxSprite);
				}
				//fx hold
				else if (line.second->fxVal[lane] == 1) {
					fxHoldSprite.setPosition(getNoteLocation(i, lane * 2, line.first));
					fxHoldSprite.setScale(fxHoldSprite.getScale().x, (measureHeight / 192) * (line.second->next->pos - line.second->pos));
					window->draw(fxHoldSprite);
				}
			}

			
			for (int lane = 0; lane < 4; lane++) {
				//bt chips
				if (line.second->btVal[lane] == 1) {
					auto test = getNoteLocation(i, lane, line.first);
					btSprite.setPosition(getNoteLocation(i, lane, line.first));
					window->draw(btSprite);
				}
				//bt hold
				else if (line.second->btVal[lane] == 2) {
					btHoldSprite.setPosition(getNoteLocation(i, lane, line.first));
					btHoldSprite.setScale(btHoldSprite.getScale().x, (measureHeight / 192) * (line.second->next->pos - line.second->pos));
					window->draw(btHoldSprite);
				}
			}
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
}


sf::Vector2f editWindow::getMeasureStart(int measure) {
	int columnNum = measure / measuresPerColumn;
	return sf::Vector2f((4 * laneWidth) + (columnWidth * columnNum), topPadding + measureHeight * (measuresPerColumn - (measure % measuresPerColumn) - 1));
}

sf::Vector2f editWindow::getSnappedPos(ToolType type) {
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

void editWindow::clearRedoStack() {
	while (!redoStack.empty()) {
		for (auto it : redoStack.top()) {
			delete it.second;
		}
		redoStack.pop();
	}
}

void editWindow::handleEvent(sf::Event event) {
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
				clearRedoStack();
				insertChartLine(getMouseMeasure(), getMouseLine(), newLine);
			}
		}
		if (event.mouseButton.button == sf::Mouse::Right) {
			removeChartLine(getMouseMeasure(), getMouseLine(), tool);
			clearRedoStack();
			
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Space) {
			std::cout << "paused!" << std::endl;
			controlPtr->paused = true;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){ 
			if (event.key.code == sf::Keyboard::Z) {
				undo();
			}
			if (event.key.code == sf::Keyboard::Y) {
				redo();
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
	if (event.type == sf::Event::MouseMoved) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && 
			sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
			sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) &&
			getMouseGlobalLine() != -1){
				selectEnd = getMouseGlobalLine();
		}
	}

	if (event.type == sf::Event::MouseButtonReleased) {
		if (getMouseGlobalLine() != -1) {
			selectEnd = getMouseGlobalLine();
		}
	}
}

void editWindow::undo() {
	if (undoStack.empty()) return;
	std::vector<std::pair<ChartLine*, ChartLine*>> redoBuffer;
	for (auto it : undoStack.top()){
		if (it.first == nullptr) {
			redoBuffer.push_back(it);
			chart.lines.erase(it.second->pos);
			chart.measures[it.second->measurePos].lines.erase(it.second->pos - chart.measures[it.second->measurePos].pos);
		}
		else {
			redoBuffer.push_back(std::make_pair(it.first, new ChartLine(*it.first)));
			*(it.first) = *(it.second);
			delete it.second;
		}
	}
	redoStack.push(redoBuffer);
	undoStack.pop();
}

void editWindow::redo() {
	if (redoStack.empty()) return;
	std::vector<std::pair<ChartLine*, ChartLine*>> undoBuffer;
	for (auto it : redoStack.top()) {
		if (it.first == nullptr) {
			undoBuffer.push_back(it);
			chart.lines[it.second->pos] = it.second;
			chart.measures[it.second->measurePos].lines[it.second->pos - chart.measures[it.second->measurePos].pos] = it.second;
		}
		else {
			undoBuffer.push_back(std::make_pair(it.first, new ChartLine(*it.first)));
			*(it.first) = *(it.second);
			delete it.second;
		}
	}
	undoStack.push(undoBuffer);
	redoStack.pop();
}

sf::Vector2f editWindow::getNoteLocation(int measure, int lane, int line) {
	sf::Vector2f startPos = getMeasureStart(measure);
	float snapHieght = measureHeight / 192;
	return sf::Vector2f(startPos.x + lane * laneWidth, startPos.y + (192 - line) * snapHieght);
}

void editWindow::update() {
	sf::Clock deltaClock;
	controlPtr->seekPos = 50;
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
	
	ImGui::End();
	
	drawMap();
	drawChart();
}

