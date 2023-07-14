#pragma once
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "AudioManager.h"
#include "FontManager.h"
#include "Input.h"
#include "Parser.h"
#include "Structs.h"
#include "Unique.h"
#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"

#include <tinyfsm.hpp>

#include "ChartDisplay.h"


class EditController;
struct MouseEvent : tinyfsm::Event
{
	int lane = -1;
	int line = -1;
	float laserPos = L_NONE;
	EditTool tool;
};
struct UpdateEvent : MouseEvent
{
	sf::RenderTarget* window;
};
struct ToolChangeEvent : tinyfsm::Event
{
	ToolChangeEvent() = default;
	ToolChangeEvent(ToolType _tool) : tool(_tool) {};
	EditTool tool;
};



#include "fsmlist.h"

struct DeleteEvent : MouseEvent { };
struct MouseDown : MouseEvent { };
struct MouseUp : MouseEvent { };
struct MouseMove : MouseEvent { };
struct KeyEvent : tinyfsm::Event { };

class EditController
	: public tinyfsm::Fsm<EditController>
{
	/* NOTE: react(), entry() and exit() functions need to be accessible
	 * from tinyfsm::Fsm class. You might as well declare friendship to
	 * tinyfsm::Fsm, and make these functions private:
	 *
	 * friend class Fsm;
	 */
public:

	/* default reaction for unhandled events */
	void react(tinyfsm::Event const&) { };

	virtual void react(MouseDown const&);
	virtual void react(MouseUp const&);
	virtual void react(UpdateEvent const&);
	virtual void react(MouseMove const&);
	virtual void react(ToolChangeEvent const& e);
	virtual void react(DeleteEvent const& e);

	virtual void entry(void);  /* entry actions in some states */
	virtual void exit(void) { };  /* no exit actions at all */

protected:
	static MouseEvent iState;
};


enum EditorState
{
	IDLE,
	PLACING_BT,
	PLACING_FX,
	PLACING_LASER,
	SELECTING,
	MODIFYING_LASER,
	PLAYING,
	HOVERED_GUI,
};

typedef std::vector<std::pair<ChartLine*, std::vector<sf::VertexArray>>> QuadArray;

class EditWindow final : public Unique<EditWindow>
{
private:
	int selectStart = 0;
	int selectEnd = 0;

	int mouseDownLine = 0;
	int mouseDownLane = 0;

	std::pair<int, ChartLine*> laserHover;
	std::pair<int, ChartLine*> selectedLaser;
	//std::pair<int, ChartLine*> selectedLaserEnd;

	std::vector<Measure> measures;
	float mouseX = 0;
	float mouseY = 0;

	sf::Font font;

public:
	EditWindow();
	~EditWindow() = default;

	void setWindow(sf::RenderWindow* _window);
	void setWindow(sf::RenderTarget* _window);
	void update();
	void drawDebug();
	void updateVars();
	void changeTool(EditTool tool);

	void drawChart(unsigned int start, unsigned int end);
	void drawPlayBar();
	std::vector<sf::VertexArray> generateSlamQuads(int lineNum, float start, float end, int laser, bool isWide);

	int getMouseLane();
	//int getMouseMeasure();
	int getMouseLine();
	int getSnappedLine(int line);
	float getMouseLaserPos(bool isWide) const;

	int getMeasureFromLine(unsigned int loc);

	void handleEvent(sf::Event event);
	float getLaserX(ChartLine* line, int laser);

	sf::Vector2f getSnappedPos(ToolType type);

	//get the note location from line (global) and lane
	sf::Vector2f getNoteLocation(int lane, int line);
	//get the note location from line (global)
	sf::Vector2f getNoteLocation(int line);

	void connectLines(std::map<unsigned int, ChartLine*> input);

	//QuadArray wrapLaserQuads(QuadArray& arr);

	void loadFile(const std::string& mapFilePath, const std::string& mapFileName);
	void saveFile(const std::string& fileName);
	void saveFile();
	void saveEvent(sf::Event event);

	//actions
	void undo(sf::Event event);
	void redo(sf::Event event);
	void copy(sf::Event event);
	void cut(sf::Event event);
	void paste(sf::Event event);

	void startSelect(sf::Event event);
	void endSelect(sf::Event event);

	void play(sf::Event event);

	void moveLaserLeft(sf::Event event);
	void moveLaserRight(sf::Event event);
	void moveLaserDown(sf::Event event);
	void moveLaserUp(sf::Event event);

	void mouseScroll(sf::Event event);

	void mousePressedLeft(sf::Event event);
	void mouseReleasedLeft(sf::Event event);

	int editorMeasure = 0;
	int editorLineStart = 0;
	int columns = 6;

	//this is relative to 4/4, if we have say 6/4 it would only draw 3 columns
	int measuresPerColumn = 4;

	//derived from 4/4 with 4 measures per column
	int pulsesPerColumn = 192 * 4;
	int snapGridSize = 16;
	float laserMoveSize = 0.1;
	int viewLines = 0;
	float playbackSpeed = 1.0;
	AudioManager player;

	EditTool tool;


	float width = 0;
	float height = 0;
	float laneWidth = 0;
	float columnWidth = 0;
	float measureHeight = 0;
	float topPadding = 0.05;
	float bottomPadding = 0.1;

	bool DEBUG = true;

	EditorState state = IDLE;

	sf::RenderTarget* window = nullptr;
	sf::RenderWindow* appWindow = nullptr;

	std::map<unsigned int, ChartLine> selectedLines;

	std::map<unsigned int, ChartLine> clipboard;

	ChartDisplay display = ChartDisplay();

	Chart chart;
	//functions
};
