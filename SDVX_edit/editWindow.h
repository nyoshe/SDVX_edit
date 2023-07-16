#pragma once
#include <string>
#include <tinyfsm.hpp>
#include <unordered_map>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "AudioManager.h"
#include "ChartDisplay.h"
#include "FontManager.h"
#include "Input.h"
#include "Parser.h"
#include "Structs.h"
#include "Unique.h"
#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"
#include "imgui_internal.h"

class EditWindow;

class EditController;
struct EditEvent : tinyfsm::Event
{
	int lane = -1;
	int line = -1;
	int startLine = 0;
	int endLine = 0;
	float laserPos = L_NONE;
	sf::RenderTarget* window;
	EditTool tool;
};

#include "fsmlist.h"
using event_list = tinyfsm::FsmList<EditEvent>;

struct MouseEvent : EditEvent { };
struct UpdateEvent : EditEvent { };
struct ToolChangeEvent : EditEvent { };
struct RightCLickEvent : EditEvent { };
struct MouseDown : EditEvent { };
struct MouseUp : EditEvent { };
struct MouseMove : EditEvent { };
struct KeyEvent : EditEvent { };
struct SelectEvent : EditEvent { };
struct KeyUpEvent : EditEvent { };
struct PlayEvent : EditEvent { };
struct UpdateEndEvent : EditEvent { };

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
	virtual void react(tinyfsm::Event const&) { };
	
	virtual void react(MouseDown const&);
	virtual void react(MouseUp const&);
	virtual void react(UpdateEvent const&);
	virtual void react(MouseMove const&);
	virtual void react(ToolChangeEvent const& e);
	virtual void react(RightCLickEvent const& e);
	virtual void react(SelectEvent const& e);
	virtual void react(KeyUpEvent const& e);
	virtual void react(PlayEvent const& e);
	virtual void react(UpdateEndEvent const& e);

	virtual void entry(void) { };  /* entry actions in some states */
	virtual void exit(void) { };  /* no exit actions at all */

protected:
	static EditEvent iState;
	static EditWindow* editor;
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


	std::pair<int, ChartLine*> laserHover;
	
	//std::pair<int, ChartLine*> selectedLaserEnd;

	std::vector<Measure> measures;
	sf::Font font;

public:
	float mouseX = 0;
	float mouseY = 0;
	int selectStart = 0;
	int selectEnd = 0;

	EditWindow();
	~EditWindow() = default;
	std::pair<int, ChartLine*> selectedLaser;
	
	void setWindow(sf::RenderWindow* _window);
	void setWindow(sf::RenderTarget* _window);
	void update();
	void drawDebug();
	void updateVars();
	void changeTool(ToolType type);
	void changeTool(EditTool tool);

	void drawChart(unsigned int start, unsigned int end);
	std::vector<sf::VertexArray> generateSlamQuads(int lineNum, float start, float end, int laser, bool isWide);

	int getMouseLane();
	//int getMouseMeasure();
	int getMouseLine();
	int getSnappedLine(int line);
	float getMouseLaserPos(bool isWide) const;

	int getMeasureFromLine(unsigned int loc);

	void handleEvent(sf::Event event);
	//get the note location from line (global) and lane
	sf::Vector2f getNoteLocation(int lane, int line);
	//get the note location from line (global)
	sf::Vector2f getNoteLocation(int line);

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

	void play(sf::Event event);

	void moveLaserLeft(sf::Event event);
	void moveLaserRight(sf::Event event);
	void moveLaserDown(sf::Event event);
	void moveLaserUp(sf::Event event);

	void mouseScroll(sf::Event event);

	void mousePressedLeft(sf::Event event);

	template <typename T>
	T createEvent(void)
	{
		T out;
		out.lane = getMouseLane();
		out.line = getSnappedLine(getMouseLine());
		out.laserPos = getMouseLaserPos(true);
		out.window = this->window;
		out.tool = this->tool;
		return out;
	}

	template <typename T>
	T createEvent(int start, int end)
	{
		T out = createEvent<T>();
		out.startLine = start;
		out.endLine = end;
		return out;
	}


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
	sf::RenderTexture tex;
	sf::RenderWindow* appWindow = nullptr;

	std::map<unsigned int, ChartLine*> selectedLines;

	std::map<unsigned int, ChartLine*> clipboard;

	ChartDisplay display = ChartDisplay();

	Chart chart;
	//functions
};
