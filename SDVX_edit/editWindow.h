#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include "parser.h"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "structs.h"
#include "audioManager.h"
#include "Unique.h"
#include "Input.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <stack>
#include <functional>
#include <memory>
#include <functional>
#include <memory>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <cstdlib> //std::system
#include <sstream>

enum EditorState {
	IDLE,
	PLACING_LASER,
	SELECTING,
	MODIFYING_LASER,
	CREATING_LASER,
	PLAYING,
};

struct gameControl
{
	bool paused = true;
	bool seek = false;
	uint32_t seekPos = 0;
	float speed = 1.0;
};
/*
struct MouseInfo {
	int mouseStartLine = 0;
	int mouseCurrentLine = 0;
	int mouseEndLine = 0;
	int mouseDownLane = 0;
	int mouseUpLane = 0;
	int mouseDownLaserPos = 0;

	bool isValid = false;
	void update() {
		hoveredGui = (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow));
	}
};
*/
typedef std::vector<std::pair<ChartLine*, std::vector<sf::VertexArray>>> QuadArray;
typedef std::map<unsigned int, ChartLine*>::iterator LineIterator;

class EditWindow final : public Unique <EditWindow>
{
private:

	void drawLineButtons(ChartLine* line, bool Selected);
	void drawSelected(ChartLine* line, const sf::Sprite& sprite);

	unsigned int selectStart = 0;
	unsigned int selectEnd = 0;

	unsigned int mouseDownLine = 0;

	unsigned int mouseDownLane = 0;

	std::pair<int, ChartLine*> laserHover;
	std::pair<int, ChartLine*> selectedLaser;
	//std::pair<int, ChartLine*> selectedLaserEnd;

	std::vector<Measure> measures;
	float mouseX = 0;
	float mouseY = 0;
	sf::Texture btTex;
	sf::Sprite btSprite;

	sf::Texture fxTex;
	sf::Sprite fxSprite;

	sf::Texture btHoldTex;
	sf::Sprite btHoldSprite;

	sf::Texture fxHoldTex;
	sf::Sprite fxHoldSprite;

	sf::Texture entryTex;
	sf::Sprite entrySprite;

	sf::Sprite toolSprite;

	sf::Font font;
public:

	EditWindow();
	~EditWindow() = default;

	void setWindow(sf::RenderWindow* _window);
	void setWindow(sf::RenderTarget* _window);
	void update();
	void drawPlacementGuides();
	void drawDebug();
	void updateVars();

	//the line specifies where we want to start drawing the measure, it returns the end location
	int drawMeasure(unsigned int measure, unsigned int startLine);

	//hackey but this function essentially generates the laser vertices
	QuadArray generateLaserQuads(int l, const std::map<unsigned int, ChartLine*>& objects, LineIterator startIter, LineIterator endIter);
	void drawChart();
	void drawPlayBar();
	void drawChart(sf::RenderTarget* target);
	void checkLaserSelect(QuadArray& arr, int laser);
	void drawLaserQuads(const QuadArray& arr);
	std::vector<sf::VertexArray> generateSlamQuads(int llineNumine, int start, int end, int laser, bool isWide);

	int getMouseLane();
	int getMouseMeasure();
	int getMouseLine();
	int getSnappedLine(int line);
	int getMouseLaserPos(bool isWide);

	int getMeasureFromLine(unsigned int loc);

	void handleEvent(sf::Event event);
	std::vector <float> getLaserX(ChartLine* line);
	sf::Vector2f getMeasurePos(int measure);

	sf::Vector2f getSnappedPos(ToolType type);
	//get the note location from measure, lane, and line positio
	//sf::Vector2f getNoteLocation(int measure, int lane, int line);

	//get the note location from line (global) and lane
	sf::Vector2f getNoteLocation(int lane, int line);
	//get the note location from line (global)
	sf::Vector2f getNoteLocation(int line);

	void conenctLines(std::map<unsigned int, ChartLine*> input);

	float triArea(sf::Vector2f A, sf::Vector2f B, sf::Vector2f C);
	bool getMouseOverlap(const sf::VertexArray quad);
	//QuadArray wrapLaserQuads(QuadArray& arr);

	void loadFile(std::string mapFilePath, std::string mapFileName);
	void saveFile(std::string fileName);
	void saveFile();

	//actions
	void undo(sf::Event event);
	void redo(sf::Event event);
	void copy(sf::Event event);
	void cut(sf::Event event);
	void paste(sf::Event event);

	void updateSelect(sf::Event event);
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
	//vars
	boost::interprocess::managed_shared_memory memSegment;
	int editorMeasure = 0;
	int editorLineStart = 0;
	int columns = 6;

	//this is relative to 4/4, if we have say 6/4 it would only draw 3 columns
	int measuresPerColumn = 4;

	//derived from 4/4 with 4 measures per column
	int pulsesPerColumn = 192 * 4;
	int snapGridSize = 16;
	int laserMoveSize = 5;
	int viewLines = 0;
	float playbackSpeed = 1.0;
	AudioManager player;

	ToolType tool = ToolType::BT;

	gameControl* controlPtr;

	float width = 0;
	float height = 0;
	float laneWidth = 0;
	float columnWidth = 0;
	float measureHeight = 0;
	float topPadding = 50;
	float bottomPadding = 50;

	bool DEBUG = true;

	EditorState state = IDLE;
	bool select;

	sf::RenderTarget* window = nullptr;
	sf::RenderWindow* appWindow = nullptr;

	std::map<unsigned int, ChartLine*> selectedLines;

	std::map<unsigned int, ChartLine>  clipboard;

	Chart chart;
	//functions
};