#include "ChartDisplay.h"

#include "EditWindow.h"

void ChartDisplay::setVariables(sf::RenderTarget& window)
{
	width = static_cast<float>(window.getView().getSize().x);
	height = static_cast<float>(window.getView().getSize().y);
	laneWidth = static_cast<float>(window.getView().getSize().x) / (10.0);

	entrySprite.setScale(laneWidth / entrySprite.getTexture()->getSize().x,
		laneWidth / entrySprite.getTexture()->getSize().x);
	entrySprite.setOrigin(0, entrySprite.getTexture()->getSize().y);

	btSprite.setScale(laneWidth / btSprite.getTexture()->getSize().x, laneWidth / btSprite.getTexture()->getSize().x);
	btSprite.setOrigin(0, btSprite.getTexture()->getSize().y);

	btHoldSprite.setScale(laneWidth / btHoldSprite.getTexture()->getSize().x, 1);
	btHoldSprite.setOrigin(0, 1);

	fxSprite.setScale(2 * laneWidth / fxSprite.getTexture()->getSize().x,
		2 * laneWidth / fxSprite.getTexture()->getSize().x);
	fxSprite.setOrigin(0, fxSprite.getTexture()->getSize().y);

	fxHoldSprite.setScale(2 * laneWidth / fxHoldSprite.getTexture()->getSize().x, 1);
	fxHoldSprite.setOrigin(0, 1);
}

ChartDisplay::ChartDisplay()
{
	//bt button
	if (!entryTex.loadFromFile("textures/entryTex.png")) {
		PLOG_WARNING << "failed to load entry sprite!";
	}
	entrySprite.setTexture(entryTex);

	//bt button
	if (!btTex.loadFromFile("textures/button.png")) {
		PLOG_WARNING << "failed to load bt sprite!";
	}
	btSprite.setTexture(btTex);

	sf::Texture testTex;
	//btHold
	if (!testTex.loadFromFile("textures/buttonhold.png")) {
		PLOG_WARNING << "failed to load fx sprite!";
	}

	if (!btHoldTex.loadFromFile("textures/buttonhold.png",
		sf::IntRect(0, testTex.getSize().y - 1, testTex.getSize().x, testTex.getSize().y))) {
		PLOG_WARNING << "failed to load fx sprite!";
	}
	btHoldSprite.setTexture(btHoldTex);
	btHoldSprite.setTextureRect(sf::IntRect(0, btHoldTex.getSize().y - 1, btHoldTex.getSize().x,
		btHoldTex.getSize().y));

	//fx button
	if (!fxTex.loadFromFile("textures/fxbutton.png")) {
		PLOG_WARNING << "failed to load fx sprite!";
	}
	fxSprite.setTexture(fxTex);

	//fx hold
	if (!testTex.loadFromFile("textures/fxbuttonhold.png")) {
		PLOG_WARNING << "failed to load fx sprite!";
	}

	if (!fxHoldTex.loadFromFile("textures/fxbuttonhold.png",
		sf::IntRect(0, testTex.getSize().y - 1, testTex.getSize().x, testTex.getSize().y))) {
		PLOG_WARNING << "failed to load fx sprite!";
	}
	fxHoldSprite.setTexture(fxHoldTex);
	fxHoldSprite.setTextureRect(sf::IntRect(0, fxHoldTex.getSize().y - 1, fxHoldTex.getSize().x,
		fxHoldTex.getSize().y));
	font.loadFromFile("Fonts/CONSOLA.TTF");

}

void ChartDisplay::draw(sf::RenderTarget& window, const Chart& chart, int _offset, int start, int end)
{
	offset = _offset;
	visibleLines = end - start;
	setVariables(window);

	//draw vertical separators
	for (int i = 0; i < 5; i++) {
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f((3 * laneWidth) + (laneWidth * i), getNoteLocation(start).y),
					   sf::Color(100, 100, 100)),
			sf::Vertex(sf::Vector2f((3 * laneWidth) + (laneWidth * i), getNoteLocation(end).y),
					   sf::Color(100, 100, 100))
		};
		window.draw(line, 2, sf::Lines);
	}

	//draw beat lines
	for (int i = start; i < end; i += beatSeparation) {
		sf::Vector2f v = getNoteLocation(i);
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(v.x, v.y), sf::Color(50, 50, 50)),
			sf::Vertex(sf::Vector2f(v.x + laneWidth * 4, v.y), sf::Color(50, 50, 50))
		};
		window.draw(line, 2, sf::Lines);
	}

	//drawing buttons
	auto lineIt = chart.lines.lower_bound(start);
	if (lineIt != chart.lines.end()) {
		ChartLine* line = lineIt->second;
		while (line && line->pos <= end) {

			//draw measure separators
			if (!(line->prev) || line->prev->measurePos != line->measurePos) {
				sf::Vector2f v = getNoteLocation(line->pos);
				sf::Vertex line[] = {
					sf::Vertex(sf::Vector2f(v.x, v.y), sf::Color(255, 255, 0)),
					sf::Vertex(sf::Vector2f(v.x + laneWidth * 4, v.y), sf::Color(255, 255, 0))
				};
				window.draw(line, 2, sf::Lines);
			} else if (!line->prev) {
				sf::Vector2f v = getNoteLocation(0);
				sf::Vertex line[] = {
					sf::Vertex(sf::Vector2f(v.x, v.y), sf::Color(255, 255, 0)),
					sf::Vertex(sf::Vector2f(v.x + laneWidth * 4, v.y), sf::Color(255, 255, 0))
				};
				window.draw(line, 2, sf::Lines);
			}
			

			drawLineButtons(window, line, false);
			line = line->next;
		}
	}

	for (int l = 0; l < 2; l++) {
		
		QuadArray vertexBuffer =
			generateLaserQuads(l, chart.lines, chart.getLineBefore(start), chart.getLineAfter(end), {laserLColor, laserRColor});
		drawLaserQuads(window, vertexBuffer);
		qArray[l] = vertexBuffer;
	}
}

void ChartDisplay::drawAsColor(sf::RenderTarget& window, std::map<unsigned int, ChartLine*>& objects, sf::Color col, int _offset, int start, int end, LineMask mask)
{
	if (objects.empty()) return;
	for (int l = 0; l < 2; l++) {
		if (mask.laser[l]) {
			QuadArray vertexBuffer =
				generateLaserQuads(l, objects, objects.lower_bound(start), objects.lower_bound(end),
					{ col, col });
			drawLaserQuads(window, vertexBuffer);
			//qArray[l] = vertexBuffer;
		}
	}
	auto lineIt = objects.lower_bound(start);
	if (lineIt != objects.end()) {
		ChartLine* line = lineIt->second;
		while (line && line->pos <= end) {
			ChartLine* maskedLine = (line->extractMask(mask));
			drawLineButtons(window, maskedLine, false, col);
			delete maskedLine;
			line = line->next;
		}
	}
}

sf::Vector2f ChartDisplay::getNoteLocation(int lane, int line)
{
	line -= offset;
	//48 is the number of lines per beat
	float startX = (3 * laneWidth);
	float y = -height * (static_cast<float>(line) / static_cast<float>(visibleLines));
	return sf::Vector2f(startX + lane * laneWidth, y);
}

sf::Vector2f ChartDisplay::getNoteLocation(int line)
{
	return getNoteLocation(0, line);
}

void ChartDisplay::drawLineButtons(sf::RenderTarget& window, const ChartLine* line, bool selected, sf::Color col)
{
	int pos = line->pos;
	for (int lane = 0; lane < 2; lane++) {
		//fx chips
		if (line->fxVal[lane] == 2) {
			fxSprite.setPosition(getNoteLocation(lane * 2, pos));
			fxSprite.setColor(col);
			if (selected) {
				drawSelected(window, line, fxSprite);
			}
			else {
				window.draw(fxSprite);
			}
		}
		//fx hold
		else if (line->fxVal[lane] == 1) {
			fxHoldSprite.setPosition(getNoteLocation(lane * 2, pos));
			fxHoldSprite.setColor(col);
			if (line->next) {
				fxHoldSprite.setScale(fxHoldSprite.getScale().x,
					(height / visibleLines) * (line->next->pos - line->pos));
			}
			if (selected) {
				drawSelected(window, line, fxHoldSprite);
			}
			else {
				window.draw(fxHoldSprite);
			}
		}
	}

	if (DEBUG) {
		sf::Vector2f v = getNoteLocation(0, pos);
		sf::Color col;
		if (line->isMeasureStart) {
			col = sf::Color(255, 80, 80, 100);
		} else {
			col = sf::Color(0, 255, 0, 100);
		}
		sf::Vertex l[] = {
			sf::Vertex(sf::Vector2f(v.x - laneWidth * 1.0 - line->isMeasureStart * laneWidth * 0.5, v.y), col),
			sf::Vertex(sf::Vector2f(v.x , v.y), col),
			sf::Vertex(sf::Vector2f(v.x + laneWidth * 4.0, v.y), col),
			sf::Vertex(sf::Vector2f(v.x + laneWidth * 5.0 + line->isMeasureStart * laneWidth * 0.5 , v.y), col)
		};

		for (int i = 0; i < 2; i++) {
			if (line->type[i] != LASER_NONE) {
				sf::Text text("N", font);
				if (line->type[i] == LASER_HEAD) {
					text.setString("H");
					text.setFillColor(sf::Color::Green);
				}
				if (line->type[i] == LASER_TAIL) {
					text.setString("T");
					text.setFillColor(sf::Color::Red);
				}
				if (line->type[i] == LASER_BODY) {
					text.setString("B");
					text.setFillColor(sf::Color::Blue);
				}

				text.setCharacterSize(20);
				text.setPosition(v.x - laneWidth * 1.6 + i * laneWidth * 7, v.y - 10);
				window.draw(text);
			}
			
		}
		

		
		window.draw(l, 4, sf::Lines);
	}

	for (int lane = 0; lane < 4; lane++) {
		//bt chips
		if (line->btVal[lane] == 1) {
			btSprite.setPosition(getNoteLocation(lane, pos));
			btSprite.setColor(col);
			if (selected) {
				drawSelected(window, line, btSprite);
			}
			else {
				window.draw(btSprite);
			}
		}
		//bt hold
		else if (line->btVal[lane] == 2) {
			btHoldSprite.setPosition(getNoteLocation(lane, pos));
			btHoldSprite.setColor(col);
			if (line->next) {
				btHoldSprite.setScale(btHoldSprite.getScale().x,
					(height / visibleLines) * (line->next->pos - line->pos));
			}
			if (selected) {
				drawSelected(window, line, btHoldSprite);
			}
			else {
				window.draw(btHoldSprite);
			}
		}
	}
}

void ChartDisplay::drawLineButtons(sf::RenderTarget& window, const ChartLine* line, bool selected)
{
	drawLineButtons(window, line, selected, sf::Color(255, 255, 255));
}

void ChartDisplay::drawSelected(sf::RenderTarget& window, const ChartLine* line, const sf::Sprite& sprite)
{
	//static sf::RectangleShape rectangle;
	//rectangle.setSize(sf::Vector2f(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height));

	//rectangle.setPosition(sf::Vector2f(sprite.getPosition().x, sprite.getPosition().y - sprite.getGlobalBounds().height));
	float x = sprite.getPosition().x;
	float y = sprite.getPosition().y - sprite.getGlobalBounds().height;

	//rectangle.setOutlineThickness(3.0);
	//rectangle.setFillColor(sf::Color::Red);
	//rectangle.setOutlineColor(sf::Color(255, 0, 0));
	float outlineSize = 4 * FontManager::instance().getScale();
	sf::VertexArray quad(sf::Quads, 4);
	quad[0] = (sf::Vertex(sf::Vector2f(x - outlineSize, y - outlineSize), sf::Color::Red));
	quad[1] = (sf::Vertex(sf::Vector2f(x + outlineSize + sprite.getGlobalBounds().width, y - outlineSize),
		sf::Color::Red));
	quad[2] = (sf::Vertex(sf::Vector2f(x + outlineSize + sprite.getGlobalBounds().width,
		y + outlineSize + sprite.getGlobalBounds().height), sf::Color::Red));
	quad[3] = (sf::Vertex(sf::Vector2f(x - outlineSize, y + outlineSize + sprite.getGlobalBounds().height),
		sf::Color::Red));
	window.draw(quad);
}

void ChartDisplay::drawSelection(sf::RenderTarget& window, std::map<unsigned, ChartLine*>& selectedLines, int offset,
	int start, int end)
{
	setVariables(window);

	drawAsColor(window, selectedLines, sf::Color(255, 255, 255, 100), offset, start, end, Mask::LASER_ALL);

	auto lineIt = selectedLines.lower_bound(start);
	if (lineIt != selectedLines.end()) {
		//ChartLine* line = &lineIt->second;
		while (lineIt != selectedLines.end() && lineIt->second->pos <= end) {
			drawLineButtons(window, lineIt->second, true);
			++lineIt;
		}
	}
}


QuadArray ChartDisplay::generateLaserQuads(int laser, const std::map<unsigned int, ChartLine*>& objects,
	LineIterator startIter, LineIterator endIter, std::vector<sf::Color> cols)
{
	std::vector<sf::VertexArray> laserBuffer;
	QuadArray vertexBuffer;
	if (startIter == objects.end() ||
		endIter == objects.end()) {
		return vertexBuffer;
	}


	ChartLine* line = startIter->second;
	if (line->laserPos[laser] == L_CONNECTOR) {
		line = line->getPrevLaser(laser);
	}
	ChartLine* start = line;

	bool isSelected = false;
	while (line && line->pos < (endIter->second->pos)) {
		//int measureNum = line->measurePos - editorMeasure;
		sf::Color c = cols[laser];

		if (!(line->laserPos[laser] >= 0) ||
			!line->next ||
			!line->getNextLaser(laser)) {
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
		if (line->type[laser] == LASER_HEAD) {
			x = getLaserX(line, laser);
			y = getNoteLocation(lineNum).y;

			quad[0] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);
			quad[1] = sf::Vertex(sf::Vector2f(x, y), c);
			quad[2] = sf::Vertex(sf::Vector2f(x, y + laneWidth), c);
			quad[3] = sf::Vertex(sf::Vector2f(x + laneWidth, y + laneWidth), c);
			laserBuffer.push_back(quad);

			quad[0].color = sf::Color(255, 255, 255, 255);
			quad[1].color = sf::Color(255, 255, 255, 255);
			quad[2].color = sf::Color(255, 255, 255, 255);
			quad[3].color = sf::Color(255, 255, 255, 255);
			quad[0].texCoords = sf::Vector2f(0.f, 0.f);
			quad[1].texCoords = sf::Vector2f(128.f, 0.f);
			quad[2].texCoords = sf::Vector2f(128.f, 128.f);
			quad[3].texCoords = sf::Vector2f(0.f, 128.f);

			laserBuffer.push_back(quad);
		}

		ChartLine* nextLaser = line->getNextLaser(laser);
		//check for slams, this is not kson compliant and checks based on timing
		if ((nextLaser->pos - line->pos) <= (192 / 32) &&
			//line->laserPos[laser] != L_CONNECTOR &&
			line->laserPos[laser] != nextLaser->laserPos[laser]) {
			x = std::max(getLaserX(line, laser), getLaserX(nextLaser, laser));
			y = getNoteLocation(lineNum).y;
			// build laser quad

			float dif = getNoteLocation(lineNum).y - getNoteLocation(nextLaser->pos).y;
			quad[0] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);
			quad[1] = sf::Vertex(sf::Vector2f(x + laneWidth, y - dif), c);

			x = std::min(getLaserX(line, laser), getLaserX(nextLaser, laser));
			quad[2] = sf::Vertex(sf::Vector2f(x, y - dif), c);
			quad[3] = sf::Vertex(sf::Vector2f(x, y), c);

			//window->draw(quad);
			laserBuffer.push_back(quad);

			//draw tail if we need it
			if (nextLaser->type[laser] == LASER_TAIL) {
				x = getLaserX(nextLaser, laser);
				quad[0] = sf::Vertex(sf::Vector2f(x, y - dif), c);
				quad[1] = sf::Vertex(sf::Vector2f(x + laneWidth, y - dif), c);
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

		x = getLaserX(line, laser);
		y = getNoteLocation(lineNum).y;
		quad[0] = sf::Vertex(sf::Vector2f(x, y), c);
		quad[1] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);

		int nextLineNum = line->getNextLaser(laser)->pos;
		ChartLine* nextLine = line->getNextLaser(laser);

		x = getLaserX(nextLine, laser);
		y = getNoteLocation(nextLineNum).y;

		quad[2] = sf::Vertex(sf::Vector2f(x + laneWidth, y), c);
		quad[3] = sf::Vertex(sf::Vector2f(x, y), c);

		laserBuffer.push_back(quad);

		if (nextLine->laserPos[laser] >= 0 ||
			nextLine->pos > (offset + visibleLines)) {
			vertexBuffer.push_back(std::make_pair(line, laserBuffer));
			laserBuffer.clear();
		}

		line = nextLine;
	}

	return vertexBuffer;
}

float ChartDisplay::getLaserX(ChartLine* line, int laser)
{
	float ourPos = 0;
	if (line->type[laser] == LASER_NONE) {
		ChartLine* prevLaser = line->getPrevLaser(laser);
		ChartLine* nextLaser = line->getNextLaser(laser);
		float startPos = prevLaser->laserPos[laser];
		float endPos = nextLaser->laserPos[laser];
		int diff = nextLaser->pos - prevLaser->pos;
		ourPos = startPos - (startPos - endPos) * (static_cast<float>(line->pos - prevLaser->pos) / diff);
	}
	else {
		ourPos = line->laserPos[laser];
	}

	if (line->isWide[laser]) {
		return ourPos * (9 * laneWidth);
	}
	return ourPos * (5 * laneWidth) + 2 * laneWidth;
}


void ChartDisplay::drawLaserQuads(sf::RenderTarget& window, const QuadArray& arr)
{
	for (auto& vBuffer : arr) {
		for (auto quad : vBuffer.second) {
			//hackey but it works
			if (quad[0].color == sf::Color(255, 255, 255, 255)) {
				window.draw(quad, &entryTex);
			}
			else {
				window.draw(quad);
			}
		}
	}
}

ChartLine* ChartDisplay::getLaserHover(int laser, float mouseX, float mouseY)
{
	//run over the buffer and then check for collision
	for (auto& [chartLine, array] : qArray[laser]) {

		for (auto& quad : array) {
			if (getMouseOverlap(quad, mouseX, mouseY)) {
				return chartLine;
			}
		}
	}
	return nullptr;
}

float ChartDisplay::triArea(sf::Vector2f A, sf::Vector2f B, sf::Vector2f C)
{
	return std::abs((A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)) / 2);
}

//thanks to this stackoverflow question: https://stackoverflow.com/questions/5922027/how-to-determine-if-a-point-is-within-a-quadrilateral
bool ChartDisplay::getMouseOverlap(const sf::VertexArray quad, float mouseX, float mouseY)
{
	float leniency = 0.1;
	float area = triArea(quad[0].position, quad[1].position, quad[2].position) + triArea(
		quad[2].position, quad[3].position, quad[0].position);
	float areaSum = 0;
	for (int i = 0; i < 4; i++) {
		areaSum += triArea(quad[i % 4].position, quad[(i + 1) % 4].position, sf::Vector2f(mouseX, mouseY));
	}

	return areaSum < area + leniency && areaSum > area - leniency;
}