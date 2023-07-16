#include "EditWindow.h"

#include "bezier.h"
using namespace std::placeholders;

class Idle;
class DragPlacing;
class AwaitLaserSelect;
class EditingLaser;

class AwaitPlacement
	: public EditController
{
	void react(UpdateEvent const& e) override
	{
		if (e.lane != -1 && e.line != -1) {
			for (auto [key, val] : placingMap) {
				delete val;
			}
			placingMap.clear();
			ChartLine* line = new ChartLine();
			switch (e.tool.type) {
			case BT:
				line->btVal[e.lane] = 1;
				break;
			case FX:
				line->fxVal[e.lane/2] = 2;
				break;
			}
			
			line->pos = e.line;
			placingMap[e.line] = line;
			editor->display.drawAsColor(*e.window, placingMap,sf::Color(255,255,255,100),
				editor->editorLineStart, e.startLine, e.endLine, Mask::ALL);
		}
	}

	void react(RightCLickEvent const& e) override {
		LineMask mask;
		if (e.lane == -1) return;
		switch (e.tool.type) {
		case BT:
			mask.bt[e.lane] = true;
			break;
		case FX:
			mask.fx[e.lane / 2] = true;
			break;
		}
		editor->chart.removeChartLine(e.line, mask);
		editor->chart.pushUndoBuffer();
		editor->chart.clearRedoStack();
	}


	void react(MouseDown const& e) override {
		if (!ImGui::GetIO().WantCaptureMouse && e.lane != -1 && e.line != -1) {
			iState = e;
			auto glambda = []() ->bool {return !ImGui::GetIO().WantCaptureMouse && iState.lane != -1 && iState.line != -1; };
			transit<DragPlacing>([]() { }, glambda);
			return;
		}
	};
protected:
	inline static std::map<unsigned int, ChartLine*> placingMap = {};
};

class DragPlacing
	: public AwaitPlacement
{
	void entry() override {

	}

	void react(MouseUp const& e) override {
		editor->chart.clearRedoStack();
		editor->chart.insertChartLine(std::min(e.line, iState.line), placingMap);
		editor->chart.pushUndoBuffer();
		transit<AwaitPlacement>();
	};
	void react(UpdateEvent const& e) override
	{
		if(placingMap.size() > 0) {
			editor->display.drawAsColor(*e.window, placingMap, sf::Color(255, 255, 255, 100),
				editor->editorLineStart, e.startLine, e.endLine, Mask::ALL);
		}
		
	}

	//slow but meh, whatever
	void react(MouseMove const& e) override
	{
		
		int mouseLine = e.line;

		if (mouseLine != iState.line &&
			mouseLine != -1 &&
			!placingMap.empty() &&
			std::prev(placingMap.end())->second->pos != e.line){
			
			for (auto [key, val] : placingMap) {
				delete val;
			}
			placingMap.clear();
			LineMask mask;
			for (int i = std::min(e.line, iState.line); i < std::max(e.line, iState.line); i += (192 /
				editor->snapGridSize)) {
				ChartLine* newLine = new ChartLine();
				switch (e.tool.type) {
				case BT:
					newLine->btVal[iState.lane] = 2;
					mask.bt[iState.lane] = true;
					break;
				case FX:
					newLine->fxVal[iState.lane / 2] = 1;
					mask.fx[iState.lane / 2] = true;
					break;
				}
				newLine->pos = i;
				placingMap[i] = newLine;
			}
			placingMap[std::max(e.line, iState.line)] = new ChartLine();
			placingMap[std::max(e.line, iState.line)]->pos = std::max(e.line, iState.line);
			editor->chart.connectLines(placingMap);
		}
	}

	void exit() override
	{
		for (auto [key, val] : placingMap) {
			delete val;
		}
		placingMap.clear();
	}

};

class SelectArea
	: public EditController
{
	void react(MouseUp const& e) override {
		if (!ImGui::GetIO().WantCaptureMouse && e.line != -1) {
			editor->selectedLines.clear();
			editor->selectedLines = editor->chart.getSelection(iState.line, e.line, Mask::ALL);
		}
		iState = e;
		transit<Idle>();
	};

	void react(MouseMove const& e)
	{
		if (!ImGui::GetIO().WantCaptureMouse && e.line != -1) {
			editor->selectEnd = e.line;
		}
	}
	void react(KeyUpEvent const& e) override {
		//do nothing
	};
};

class AwaitSelection
	: public EditController
{
	void react(MouseDown const& e) override {
		if (!ImGui::GetIO().WantCaptureMouse && e.line != -1) {
			editor->selectEnd = e.line;
			editor->selectStart = e.line;
			iState = e;
			transit<SelectArea>();
		}
	};

	void react(KeyUpEvent const& e) override {
		transit<Idle>();
	};

	void react(SelectEvent const& e) override {};
};

class Idle
	: public EditController
{
	//automatically return to state that we should be in
	void entry() override
	{
		switch (iState.tool.type) {
		case BT:
		case FX:
			if (iState.tool.select) 
				transit<AwaitSelection>();
			else
				transit<AwaitPlacement>();
			return;
			break;
		case LASER_L:
		case LASER_R:
			if (iState.tool.select)
				transit<AwaitLaserSelect>();
			return;
			break;
		}
	}
};

class Playing
	: public EditController
{
	void entry() override
	{
		if (editor->player.trackValid) {
			editor->player.playFrom(editor->chart.getMs(editor->selectStart));
		}
	}

	void react(PlayEvent const& e) override
	{
		if (editor->player.isPlaying()) {
			editor->player.stop();
			transit<Idle>();
		}
		else if (editor->player.trackValid) {
			editor->player.playFrom(editor->chart.getMs(editor->selectStart));
		}
	}

	void exit() override
	{
		editor->player.stop();
	}

	void react(UpdateEvent const& e) override
	{
		if (editor->chart.lines.lower_bound(e.startLine) == editor->chart.lines.end()) return;
		if (editor->chart.getLineBefore(e.endLine) == editor->chart.lines.end()) return;

		for (int i = editor->chart.lines.lower_bound(e.startLine)->second->measurePos; i <= editor->chart.getLineBefore(e.endLine)->second->measurePos; i++) {
			if (i + 1 >= editor->chart.measures.size()) {
				break;
			}
			if (editor->chart.measures[i + 1].msStart > editor->player.getMs()) {
				float startMs = editor->chart.measures[i].msStart;
				float endMs = editor->chart.measures[i + 1].msStart;

				sf::Vector2f pos1 = editor->getNoteLocation(-1, editor->chart.measures[i].pos);
				sf::Vector2f pos2 = editor->getNoteLocation(5, editor->chart.measures[i].pos);

				pos1 = editor->getNoteLocation(
					-1, editor->chart.measures[i].pos + editor->chart.measures[i].pulses * (editor->player.getMs() - startMs) / (endMs -
						startMs));
				pos2 = editor->getNoteLocation(
					5, editor->chart.measures[i].pos + editor->chart.measures[i].pulses * (editor->player.getMs() - startMs) / (endMs -
						startMs));

				sf::Vertex line[] = {
					sf::Vertex(pos1, sf::Color(255, 0, 0)),
					sf::Vertex(pos2, sf::Color(255, 0, 0))
				};
				e.window->draw(line, 2, sf::Lines);
				break;
			}
		}
	}
	void react(UpdateEndEvent const& e) override
	{
	}
};

class AwaitLaserSelect
	: public EditController
{
	void react(MouseDown const& e) override
	{
		if (e.tool.select && hover) {
			int laserBeginPos = hover->pos;
			int laserEndPos = hover->getNextLaser(e.tool.type)->pos;
			int mousePos = e.line;
			if (mousePos - laserBeginPos < laserEndPos - mousePos) {
				editor->selectedLaser = std::make_pair(e.tool.type, hover);
			}
			else {
				editor->selectedLaser = std::make_pair(e.tool.type, hover->getNextLaser(e.tool.type));
			}
			editor->chart.pushUndoBuffer();
			iState = e;
			transit<EditingLaser>();
		}
		else {
			//deselect
			editor->chart.pushUndoBuffer();
			editor->selectedLaser = std::make_pair(0, nullptr);
		}
	}

	void react(UpdateEvent const& e) override
	{
		int laser = e.tool.type;
		ChartLine* newHover = editor->display.getLaserHover(laser, editor->mouseX, editor->mouseY);
		if (newHover) {
			isHovered = true;
			hover = newHover;
			editor->display.drawAsColor(*e.window, editor->chart.lines,
				sf::Color(255, 255, 255, 100), editor->editorLineStart, 
				hover->pos, hover->getNextLaser(laser)->pos, laser ? Mask::LASER_R : Mask::LASER_L);
		}

		if(hover) {
			std::vector<sf::Vector2f> points(3);
			points[0] = { editor->display.getLaserX(hover, e.tool.type), editor->getNoteLocation(hover->pos).y };
			points[1] = {
				editor->mouseX, editor->mouseY
			};
			points[2] = {
				editor->display.getLaserX(hover->getNextLaser(e.tool.type), e.tool.type),
				editor->getNoteLocation(hover->getNextLaser(e.tool.type)->pos).y
			};
			int numPoints = 100;

			std::vector<sf::Vector2f> bezierCurve = calculateBezierCurve(points, numPoints);

			sf::VertexArray curve(sf::LineStrip);

			sf::VertexArray ctrlPoints(sf::LineStrip);

			for (auto point : bezierCurve) {
				curve.append(sf::Vertex(point, sf::Color::Red));
			}
			for (auto point : points) {
				ctrlPoints.append(sf::Vertex(point, sf::Color::Green));
			}

			//curve.append(sf::Vertex({0,0}, sf::Color::Red));
			e.window->draw(curve);

			e.window->draw(ctrlPoints);
		}
		
	}

	void react(UpdateEndEvent const& e) override
	{
		if (!isHovered) {
			hover = nullptr;
		}

		if (isHovered) {
			if (ImGui::BeginPopupContextVoid("curvePopup", ImGuiPopupFlags_MouseButtonRight))
			{
				isHovered = true;
				if (ImGui::MenuItem("make curve"))
				{
				}
				ImGui::EndPopup();
			} else {
				isHovered = false;
			}
		}
	}

	bool isHovered = false;
	ChartLine* hover = nullptr;
};

class EditingLaser
	: public AwaitLaserSelect
{

};
/*
template<typename... T>
void EditController::react(EditEvent<T> const& e) {}
*/

void EditController::react(MouseDown const&) {
	PLOG_INFO << "mouse down ignored";
}

void EditController::react(MouseUp const&) {
	PLOG_INFO << "mouse up ignored";
}

void EditController::react(UpdateEvent const&) { }

void EditController::react(MouseMove const&) { }

void EditController::react(ToolChangeEvent const& e) {
	iState.tool = e.tool;
	transit<Idle>();
}

void EditController::react(RightCLickEvent const& e) {
	iState = e;
	transit<Idle>();
}

void EditController::react(SelectEvent const& e) { transit<AwaitSelection>(); }

void EditController::react(KeyUpEvent const& e) { }

void EditController::react(PlayEvent const& e) { transit<Playing>(); }

void EditController::react(UpdateEndEvent const& e) { }

EditEvent EditController::iState = EditEvent();
EditWindow* EditController::editor = &EditWindow::instance();

FSM_INITIAL_STATE(EditController, AwaitPlacement)


EditWindow::EditWindow()
{
	Input::instance().addActionKey(sf::Event::KeyPressed, {sf::Keyboard::LControl, sf::Keyboard::Z},
	                               std::bind(&EditWindow::undo, this, _1), "undo");
	Input::instance().addActionKey(sf::Event::KeyPressed, {sf::Keyboard::LControl, sf::Keyboard::Y},
	                               std::bind(&EditWindow::redo, this, _1), "redo");
	Input::instance().addActionKey(sf::Event::KeyPressed, {sf::Keyboard::LControl, sf::Keyboard::C},
	                               std::bind(&EditWindow::copy, this, _1), "copy");
	Input::instance().addActionKey(sf::Event::KeyPressed, {sf::Keyboard::LControl, sf::Keyboard::V},
	                               std::bind(&EditWindow::paste, this, _1), "paste");
	Input::instance().addActionKey(sf::Event::KeyPressed, {sf::Keyboard::LControl, sf::Keyboard::X},
	                               std::bind(&EditWindow::cut, this, _1), "cut");
	Input::instance().addActionKey(sf::Event::KeyPressed, {sf::Keyboard::LControl, sf::Keyboard::S},
	                               std::bind(&EditWindow::saveEvent, this, _1), "save");
	Input::instance().addActionKey(sf::Event::KeyPressed, {sf::Keyboard::Space,},
	                               std::bind(&EditWindow::play, this, _1), "play");
	Input::instance().addActionKey(sf::Event::KeyPressed, {sf::Keyboard::Left},
	                               std::bind(&EditWindow::moveLaserLeft, this, _1), "moveLaserLeft");
	Input::instance().addActionKey(sf::Event::KeyPressed, {sf::Keyboard::Right},
	                               std::bind(&EditWindow::moveLaserRight, this, _1), "moveLaserRight");
	Input::instance().addActionKey(sf::Event::KeyPressed, {sf::Keyboard::Down},
	                               std::bind(&EditWindow::moveLaserDown, this, _1), "moveLaserDown");
	Input::instance().addActionKey(sf::Event::KeyPressed, {sf::Keyboard::Up},
	                               std::bind(&EditWindow::moveLaserUp, this, _1), "moveLaserUp");
	Input::instance().addEventAction(sf::Event::MouseWheelScrolled, std::bind(&EditWindow::mouseScroll, this, _1),
	                                 "moveEditWindow");
	Input::instance().addActionKey(sf::Event::KeyPressed, {sf::Keyboard::LAlt, sf::Keyboard::LControl},
		std::bind(&EditWindow::startSelect, this, _1), "selectStart");
	Input::instance().addActionMouse(sf::Event::MouseButtonPressed, {sf::Mouse::Left},
	                                 std::bind(&EditWindow::mousePressedLeft, this, _1), "mousePress");

	fsm_list::start();

	font.loadFromFile("Fonts/CONSOLA.TTF");
}

void EditWindow::loadFile(const std::string& mapFilePath, const std::string& mapFileName)
{
	Parser parser;
	Chart newChart;
	chart = newChart;
	parser.loadFile(mapFileName, chart);
	chart.calcTimings();
	chart.metadata.mapFileName = mapFileName;
	chart.metadata.mapFilePath = mapFilePath;

	if (player.loadFile(mapFilePath + "\\" + chart.metadata.songFile)) {
		PLOG_INFO << "chart File \"" << chart.metadata.mapFileName << "\" successfully loaded";
	}
	else
		PLOG_ERROR << "chart File \"" << chart.metadata.mapFileName << "\" unable to be loaded";

	int linesBefore = chart.lines.size();

	chart.minimize();

	chart.validateChart();

	PLOG_INFO << "minimized lines (" << linesBefore << "->" << chart.lines.size() << ")";
}

void EditWindow::saveFile(const std::string& fileName)
{
	Parser p;
	p.saveFile(chart, fileName);
}

void EditWindow::saveFile()
{
	saveFile(chart.metadata.mapFileName);
}

void EditWindow::saveEvent(sf::Event event)
{
	saveFile();
}

void EditWindow::updateVars()
{
	//calculate lane Width, there is 11 lanes allocate per column
	height = window->getSize().y * columns;
	width = window->getSize().x / columns;
	laneWidth = static_cast<float>(window->getSize().x) / (10.0 * columns);
	columnWidth = static_cast<float>(window->getSize().x) / columns;
	measureHeight = height / static_cast<float>(measuresPerColumn * columns);
	viewLines = pulsesPerColumn * columns;

	editorLineStart = editorMeasure * 192;
	pulsesPerColumn = measuresPerColumn * 192;

}

void EditWindow::changeTool(ToolType type)
{
	tool.type = type;
	send_event(createEvent<ToolChangeEvent>());
}

void EditWindow::changeTool(EditTool _tool)
{
	tool = _tool;

	send_event(createEvent<ToolChangeEvent>());
}

void EditWindow::setWindow(sf::RenderTarget* _window)
{
	window = _window;
	updateVars();
}

void EditWindow::setWindow(sf::RenderWindow* _window)
{
	font.loadFromFile("Fonts/CONSOLA.TTF");
	window = _window;
	appWindow = _window;
	updateVars();
}

//returns the lane # the mouse is in, -1 if not found
int EditWindow::getMouseLane()
{
	for (int j = 0; j < 4; j++) {
		if (
			mouseX >= (3 * laneWidth) + (laneWidth * j) &&
			mouseX <= (3 * laneWidth) + (laneWidth * (j + 1))) {
			return j;
		}
	}

	return -1;
}

//gives is the global location in 1/192 snapping
int EditWindow::getMouseLine()
{
	if (mouseX >= 0 &&
		mouseX <= (10 * laneWidth)) {
		return editorLineStart - (mouseY / height) * viewLines;
	}
	return editorLineStart;
	return -1;
}

int EditWindow::getSnappedLine(int line)
{
	if (line == -1) {
		return -1;
	}
	return (line / (192 / snapGridSize) * (192 / snapGridSize));
}

float EditWindow::getMouseLaserPos(bool isWide) const
{
	if (state == HOVERED_GUI) {
		return -1;
	}
	float selectAreaWidth = isWide ? (8 * laneWidth) : (4 * laneWidth);
	float mouseLocal = (mouseX - (5 * laneWidth));
	return std::clamp(static_cast<float>((mouseLocal / selectAreaWidth) + 0.5), 0.f, 1.f);
}

int EditWindow::getMeasureFromLine(unsigned int loc)
{
	int n = 0;
	if (chart.measures.empty()) {
		return -1;
	}
	auto nearest = chart.lines.lower_bound(loc);
	if (nearest == chart.lines.end()) {
		nearest = std::prev(nearest, 1);
	}
	return nearest->second->measurePos;
}


void EditWindow::drawChart(unsigned int start, unsigned int end)
{

	display.drawSelection(*window, selectedLines, editorLineStart, start, end);

	display.draw(*window, chart, editorLineStart, start, end);

	//ChartLine* hover = display.getLaserHover(0, mouseX, mouseY);

	if (selectedLaser.second) {
		sf::Vector2f v = getNoteLocation(0, selectedLaser.second->pos);
		sf::RectangleShape rect(sf::Vector2f(laneWidth + 8, 8));
		rect.setPosition(sf::Vector2f(display.getLaserX(selectedLaser.second, selectedLaser.first) - 4, v.y - 4));
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
}



std::vector<sf::VertexArray> EditWindow::generateSlamQuads(int lineNum, float start, float end, int laser, bool isWide)
{
	std::vector<sf::VertexArray> drawVec;
	float xStart = getNoteLocation(lineNum).x;
	float xEnd = getNoteLocation(lineNum).x;
	float y = getNoteLocation(lineNum).y;

	if (isWide) {
		xStart += start * (9 * laneWidth) - 3 * laneWidth;
		xEnd += end * (9 * laneWidth) - 3 * laneWidth;
	}
	else {
		xStart += start * (5 * laneWidth) - laneWidth;
		xEnd += end * (5 * laneWidth) - laneWidth;
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

void EditWindow::handleEvent(sf::Event event)
{
	if (event.mouseButton.button == sf::Mouse::Right && event.type == sf::Event::MouseButtonReleased) {
		send_event(createEvent<RightCLickEvent>());
	}

	if (event.type == sf::Event::KeyReleased) {
		send_event(createEvent<KeyUpEvent>());
	}

	if (event.type == sf::Event::MouseMoved) {
		send_event(createEvent<MouseMove>());
	}

	if (event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonPressed) {
		send_event(createEvent<MouseDown>());
	}

	if (event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonReleased) {
		send_event(createEvent<MouseUp>());
	}
}

sf::Vector2f EditWindow::getNoteLocation(int lane, int line)
{
	line -= editorLineStart;
	//48 is the number of lines per beat
	float startX = (3 * laneWidth);
	float y = -height * (static_cast<float>(line) / static_cast<float>(viewLines));
	return sf::Vector2f(startX + lane * laneWidth, y);
}

sf::Vector2f EditWindow::getNoteLocation(int line)
{
	return getNoteLocation(0, line);
}

void EditWindow::update()
{
	updateVars();
	laserHover.second = nullptr;

	sf::Clock deltaClock;
	//controlPtr->seekPos = 50;
	

	if (DEBUG) {
		drawDebug();
	}
	
	tex.clear();
	tex.create(window->getSize().x, window->getSize().y);
	sf::View prevView = window->getView();
	window = &tex;
	for (int i = 0; i < columns; i++) {
		sf::View view;
		view.setViewport(sf::FloatRect((1.f / columns) * i, topPadding, (1.f / columns) - (0.1 / columns),
		                               1.0 - bottomPadding * FontManager::instance().getAbsoluteScale() - topPadding));
		view.setSize(window->getSize().x / columns, measureHeight * measuresPerColumn);
		view.setCenter((window->getSize().x / columns) * 0.5,
		               (measureHeight * measuresPerColumn) * 0.5 - (i + 1) * (measureHeight * measuresPerColumn));
		window->setView(view);
		sf::Vector2i position = sf::Mouse::getPosition(*appWindow);

		if (view.getViewport().contains(static_cast<float>(position.x) / window->getSize().x,
		                                static_cast<float>(position.y) / window->getSize().y)) {
			// convert it to world coordinates
			sf::Vector2f worldPos = window->mapPixelToCoords(position);
			mouseX = worldPos.x;
			mouseY = worldPos.y;
		}
		int start = editorLineStart + pulsesPerColumn * i;
		int end = editorLineStart + pulsesPerColumn * (i + 1);
		drawChart(start, end);

		send_event(createEvent<UpdateEvent>(start, end));


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

	window = appWindow;
	send_event(createEvent<UpdateEndEvent>());

	sf::Sprite spr(tex.getTexture());
	spr.setScale(sf::Vector2f(1.f, -1.f));
	spr.setPosition(sf::Vector2f(0, appWindow->getSize().y));
	window->draw(spr);
	

	window->setView(prevView);
}
template<typename E>
void printType(E const& fsmType)
{
	
}

void EditWindow::drawDebug()
{

	
	static sf::Clock deltaClock;
	/**/
	ImGui::Begin("Debug");
	
	ImGui::Text(std::string("fps: " + std::to_string(1.f / deltaClock.getElapsedTime().asSeconds())).c_str());
	ImGui::Text(std::string("X: " + std::to_string(mouseX) + " Y: " + std::to_string(mouseY).c_str()).c_str());
	//ImGui::Text(std::string("measure: " + std::to_string(getMouseMeasure())).c_str());
	ImGui::Text(std::string("Mouse Line: " + std::to_string(getMouseLine())).c_str());
	ImGui::Text(std::string("Mouse Line Snapped: " + std::to_string(getSnappedLine(getMouseLine()))).c_str());
	ImGui::Text(std::string("state: " + tinyfsm::Fsm<EditController>::current_state_ptr->last).c_str());
	ImGui::Text(std::string("mouse laser pos: " + std::to_string(getMouseLaserPos(tool.wideLaser))).c_str());
	ImGui::Text(std::string("hovered: " + std::to_string(ImGui::GetIO().WantCaptureMouse)).c_str());
	ImGui::End();
	//appWindow
	//drawMap();
	deltaClock.restart();
}

void EditWindow::undo(sf::Event event)
{
	chart.pushUndoBuffer();
	selectedLaser = std::make_pair(0, nullptr);
	chart.undo();
}

void EditWindow::redo(sf::Event event)
{
	chart.pushRedoBuffer();
	selectedLaser = std::make_pair(0, nullptr);
	chart.redo();
}

void EditWindow::copy(sf::Event event)
{
	for (auto [key, val] : clipboard) {
		delete val;
	}
	clipboard.clear();
	clipboard = chart.getSelection(selectStart, selectEnd, Mask::ALL);
}

void EditWindow::cut(sf::Event event)
{
	for (auto [key, val] : selectedLines) {
		delete val;
	}
	selectedLines.clear();

	for (auto [key, val] : clipboard) {
		delete val;
	}

	clipboard = chart.getSelection(selectStart, selectEnd, Mask::ALL);

	for (auto line : clipboard) {
		chart.removeChartLine(line.first, (*line.second).makeMask());
	}
}

void EditWindow::paste(sf::Event event)
{
	chart.clearRedoStack();
	unsigned int snapPos = getSnappedLine(selectStart);
	chart.insertChartLine(snapPos, clipboard);
	chart.validateChart();
	chart.pushUndoBuffer();
}


void EditWindow::startSelect(sf::Event event)
{
	send_event(createEvent<SelectEvent>());
}



void EditWindow::play(sf::Event event)
{
	send_event(createEvent<PlayEvent>());
}

void EditWindow::moveLaserLeft(sf::Event event)
{
	if (selectedLaser.second != nullptr) {
		ChartLine* laserToEdit = selectedLaser.second;
		chart.addUndoBuffer(laserToEdit);
		//should probably change modifyLaserPos to chart
		laserToEdit->modifyLaserPos(selectedLaser.first, -laserMoveSize);
	}
}

void EditWindow::moveLaserRight(sf::Event event)
{
	if (selectedLaser.second != nullptr) {
		ChartLine* laserToEdit = selectedLaser.second;
		chart.addUndoBuffer(laserToEdit);
		laserToEdit->modifyLaserPos(selectedLaser.first, laserMoveSize);
	}
}

void EditWindow::moveLaserDown(sf::Event event)
{
	if (selectedLaser.second != nullptr) {
		LineMask moveMask;
		moveMask.laser[selectedLaser.first] = true;
		//chart.addUndoBuffer(selectedLaser.second);
		int moveDiff = getSnappedLine(selectedLaser.second->pos) - selectedLaser.second->pos;
		selectedLaser.second = chart.moveChartLine(selectedLaser.second->pos, moveMask, -192 / snapGridSize - moveDiff);
	}
}

void EditWindow::moveLaserUp(sf::Event event)
{
	if (selectedLaser.second != nullptr) {
		LineMask moveMask;
		moveMask.laser[selectedLaser.first] = true;
		//chart.addUndoBuffer(selectedLaser.second);
		int moveDiff = getSnappedLine(selectedLaser.second->pos) - selectedLaser.second->pos;
		selectedLaser.second = chart.moveChartLine(selectedLaser.second->pos, moveMask, 192 / snapGridSize + moveDiff);
	}
}

void EditWindow::mouseScroll(sf::Event event)
{
	//done for trackpads, should work fine with normal mice too
	static float scrollAccumulate = 0;
	scrollAccumulate += event.mouseWheelScroll.delta;
	int mouseDelta = 0;
	if (scrollAccumulate >= 1) {
		mouseDelta = 1;
		scrollAccumulate = 0;
	} else if (scrollAccumulate <= -1) {
		mouseDelta = -1;
		scrollAccumulate = 0;
	}

	if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
		if (editorMeasure + mouseDelta * measuresPerColumn >= 0) {
			editorMeasure += mouseDelta * measuresPerColumn;
		}
	}
}

void EditWindow::mousePressedLeft(sf::Event event)
{

	
}
