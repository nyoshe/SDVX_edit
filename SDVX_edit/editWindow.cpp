#include "editWindow.h"
void editWindow::loadFile(std::string fileName) {
	parser p;
	chart = p.loadFile(fileName);

	//set up intermediate laser positions
	std::vector<float> connectionStartX = { 0, 0 };
	std::vector<int> connectionLength = { 0, 0 };
	std::vector<bool> laserActive = { false, false };
	std::vector<int> startMeasure = { 0, 0 };
	std::vector<int> startLine = { 0, 0 };

	for (int m = 0; m < chart.measures.size(); m++) {
		for (int l = 0; l < chart.measures[m].lines.size(); l++) {
			for (int i = 0; i < 2; i++) {
				switch (chart.measures[m].lines[l].laserPos[i]) {
				case -1:
					connectionLength[i] = 0;
					laserActive[i] = false;
					break;
				case -2:
					connectionLength[i] += 192 / chart.measures[m].division;
					break;
				default:
					if (laserActive[i]) {
						//make connection points
						int n = 0;

						for (int m2 = startMeasure[i]; m2 < m; m2++) {
							int l2 = 0;
							if (m2 == startMeasure[i]) l2 = startLine[i];

							for (; l2 < chart.measures[m2].division; l2++) {
								chart.measures[m2].lines[l2].laserConnectionPos[i] = connectionStartX[i] - (connectionStartX[i] - chart.measures[m].lines[l].laserPos[i]) * float(float(n) / float(connectionLength[i]));
								n += 192 / chart.measures[m2].division;
							}
						}

						int l2 = 0;
						if (m == startMeasure[i]) l2 = startLine[i];
						for (; l2 < l; l2++) {
							chart.measures[m].lines[l2].laserConnectionPos[i] = connectionStartX[i] - (connectionStartX[i] - chart.measures[m].lines[l].laserPos[i]) * float(float(n) / float(connectionLength[i]));
							n += 192 / chart.measures[m].division;
						}
						chart.measures[m].lines[l].laserConnectionPos[i] = chart.measures[m].lines[l].laserPos[i];
						connectionLength[i] = 0;
					}
					else {
						chart.measures[m].lines[l].laserConnectionPos[i] = chart.measures[m].lines[l].laserPos[i];
						laserActive[i] = true;
					}
					
					startMeasure[i] = m;
					startLine[i] = l;
					connectionStartX[i] = chart.measures[m].lines[l].laserPos[i];
					connectionLength[i] += 192 / chart.measures[m].division;
					break;
				}
			}
		}
	}

	p.saveFile(chart, "test.ksh");
}

void editWindow::setWindow(sf::RenderWindow* _window) {
	window = _window;
	width = window->getSize().x;
	height = window->getSize().y - topPadding - bottomPadding;

	//calculate lane Width, there is 11 lanes allocate per column
	laneWidth = width / (11 * columns);
	columnWidth = width / columns;
	measureHeight = float(height) / measuresPerColumn;
	

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
				mouseX >= (4 * laneWidth) + (columnWidth * i) &&
				mouseX <= (8 * laneWidth) + (columnWidth * i) &&
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

//gets left x position of leaser start
float editWindow::getLaserX(float pos, bool wide) {
	if (wide) {
		return pos * (9 * laneWidth) / 50.0 - 3 * laneWidth;
	}
	else {
		return pos * (5 * laneWidth) / 50.0 - laneWidth;
	}
	
}

void editWindow::drawMap() {
	//draw the columns
	//draw measure lines
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < (measuresPerColumn + 1); j++) {
			sf::Vertex line[] = {
				sf::Vertex(sf::Vector2f((4 * laneWidth) + (columnWidth * i) , topPadding + measureHeight * j), sf::Color(255, 0, 0)),
				sf::Vertex(sf::Vector2f((8 * laneWidth) + (columnWidth * i) , topPadding + measureHeight * j), sf::Color(255, 0, 0))
			};
			window->draw(line, 2, sf::Lines);
		}
	}
	//draw horizontal lines
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < measuresPerColumn; j++) {
			for (int k = 1; k < 4; k++) {
				sf::Vertex line[] = {
					sf::Vertex(sf::Vector2f((4 * laneWidth) + (columnWidth * i) , topPadding + (measureHeight * j) + ((measureHeight / 4) * k)), sf::Color(50, 50, 50)),
					sf::Vertex(sf::Vector2f((8 * laneWidth) + (columnWidth * i) , topPadding + (measureHeight * j) + ((measureHeight / 4) * k)), sf::Color(50, 50, 50))
				};
				window->draw(line, 2, sf::Lines);
			}
		}
	}

	//draw vertical lines
	for (int i = 0; i < columns; i++) {
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
	std::vector<float> laserStartX = { 0, 0 };
	std::vector<float> laserStartY = { 0, 0 };

	std::vector<bool> laserActive = { false, false };
	std::vector<int> laserLength = { 0, 0 };

	for (int i = 0; i < (measuresPerColumn * columns); i++) {
		Measure cMeasure = chart.measures[i + editorMeasure];
		for (int line = 0; line < chart.measures[i + editorMeasure].lines.size(); line++) {
			ChartLine cLine = cMeasure.lines[line];
			for (int lane = 0; lane < 2; lane += 1) {
				//fx chips
				if (cLine.fxVal[lane] == 2) {
					fxSprite.setPosition(getNoteLocation(i, lane * 2, line, cMeasure.division));
					window->draw(fxSprite);
				}
				//fx hold
				else if (cLine.fxVal[lane] == 1) {
					fxHoldSprite.setPosition(getNoteLocation(i, lane * 2, line, cMeasure.division));
					fxHoldSprite.setScale(fxHoldSprite.getScale().x, measureHeight / cMeasure.division);
					window->draw(fxHoldSprite);
				}
			}

			
			for (int lane = 0; lane < 4; lane++) {
				//bt chips
				if (cLine.btVal[lane] == 1) {
					auto test = getNoteLocation(i, lane, line, cMeasure.division);
					btSprite.setPosition(getNoteLocation(i, lane, line, cMeasure.division));
					window->draw(btSprite);
				}
				//bt hold
				else if (cLine.btVal[lane] == 2) {
					btHoldSprite.setPosition(getNoteLocation(i, lane, line, cMeasure.division));
					btHoldSprite.setScale(btHoldSprite.getScale().x, measureHeight / cMeasure.division);
					window->draw(btHoldSprite);
				}
			}

			
			for (int l = 0; l < 2; l++) {
				

				sf::Color c;
				if (l == 1) {
					c = sf::Color(255, 0, 200, 150);
				}
				else {
					c = sf::Color(0, 160, 255, 150);
				}
				
				
				if (cLine.laserPos[l] != -1) {
					sf::VertexArray quad(sf::Quads, 4);
					//sf::Vertex quad[2];
					
					// build laser quad
					quad[0] = sf::Vertex(sf::Vector2f(getLaserX(cLine.laserConnectionPos[l], cLine.isWide[l]) + getMeasureStart(i).x, getNoteLocation(i, 0, line, cMeasure.division).y), c);
					quad[1] = sf::Vertex(sf::Vector2f(getLaserX(cLine.laserConnectionPos[l], cLine.isWide[l]) + getMeasureStart(i).x + laneWidth, getNoteLocation(i, 0, line, cMeasure.division).y), c);
					
					if (line == chart.measures[i + editorMeasure].lines.size() - 1) {
						quad[3] = sf::Vertex(sf::Vector2f(getLaserX(chart.measures[i + editorMeasure + 1].lines[0].laserConnectionPos[l], cLine.isWide[l]) + getMeasureStart(i).x, getNoteLocation(i, 0, line + 1, cMeasure.division).y), c);
						quad[2] = sf::Vertex(sf::Vector2f(getLaserX(chart.measures[i + editorMeasure + 1].lines[0].laserConnectionPos[l], cLine.isWide[l]) + getMeasureStart(i).x + laneWidth, getNoteLocation(i, 0, line + 1, cMeasure.division).y), c);
						if (chart.measures[i + editorMeasure + 1].lines[0].laserPos[l] != -1) {
							window->draw(quad);
						}
					}
					else {
						quad[3] = sf::Vertex(sf::Vector2f(getLaserX(cMeasure.lines[line + 1].laserConnectionPos[l], cLine.isWide[l]) + getMeasureStart(i).x, getNoteLocation(i, 0, line + 1, cMeasure.division).y), c);
						quad[2] = sf::Vertex(sf::Vector2f(getLaserX(cMeasure.lines[line + 1].laserConnectionPos[l], cLine.isWide[l]) + getMeasureStart(i).x + laneWidth, getNoteLocation(i, 0, line + 1, cMeasure.division).y), c);
						if (chart.measures[i + editorMeasure].lines[line + 1].laserPos[l] != -1) {
							window->draw(quad);
						}
					}
					
					
				}
			}
		}
	}
}

sf::Vector2f editWindow::getMeasureStart(int measure) {
	int columnNum = measure / measuresPerColumn;
	return sf::Vector2f((4 * laneWidth) + (columnWidth * columnNum), topPadding + measureHeight * (measuresPerColumn - (measure % measuresPerColumn) - 1));
}

sf::Vector2f editWindow::getSnappedPos(NoteType type) {
	//first check if we have a valid position
	int measure = getMouseMeasure();
	sf::Vector2f start = getMeasureStart(measure);
	float snapSize = measureHeight / snapGridSize;
	if (measure != -1) {
		switch (type){
		case NoteType::BT:
			return sf::Vector2f(start.x + getMouseLane() * laneWidth, start.y + snapSize * (snapGridSize - getMouseLine()));
		case NoteType::FX:
			return sf::Vector2f(start.x + (getMouseLane() / 2) * (laneWidth * 2), start.y + snapSize * (snapGridSize - getMouseLine()));
		case NoteType::LASER:
			return sf::Vector2f(-1,-1);
			//placeholder
			//TODO
		}
	}
	return sf::Vector2f(-1, -1);
}

sf::Vector2f editWindow::getNoteLocation(int measure, int lane, int line, int snapSize) {
	sf::Vector2f startPos = getMeasureStart(measure);
	float snapHieght = measureHeight / snapSize;
	return sf::Vector2f(startPos.x + lane * laneWidth, startPos.y + (snapSize - line) * snapHieght);
}

void editWindow::update() {


	sf::Vector2i position = sf::Mouse::getPosition(*window);
	mouseX = position.x * (width / float(window->getSize().x));
	mouseY = position.y * ((height + topPadding + bottomPadding) / float(window->getSize().y));

	btSprite.setPosition(getSnappedPos(NoteType::BT));

	window->draw(fxSprite);

	

	ImGui::Begin("laneWidth");
	
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
	std::string s8 = "note start x: " + std::to_string(getNoteLocation(getMouseMeasure(), getMouseLane(), getMouseLine(), snapGridSize).x);
	ImGui::Text(s8.c_str());
	std::string s9 = "note start y: " + std::to_string(getNoteLocation(getMouseMeasure(), getMouseLane(), getMouseLine(), snapGridSize).y);
	ImGui::Text(s9.c_str());
	ImGui::End();
	
	drawMap();
	drawChart();
}