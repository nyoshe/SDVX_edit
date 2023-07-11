#pragma once
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <stack>
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

#include <cstdlib> //std::system
#include <sstream>
#include <boost/interprocess/managed_shared_memory.hpp>

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
using QuadArray = std::vector<std::pair<ChartLine*, std::vector<sf::VertexArray>>>;
using LineIterator = std::map<unsigned int, ChartLine*>::iterator;

class EditWindow final : public Unique<EditWindow>
{
private:
	void drawLineButtons(ChartLine* line, bool Selected);
	void drawSelected(ChartLine* line, const sf::Sprite& sprite);

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
	QuadArray generateLaserQuads(int l, const std::map<unsigned int, ChartLine*>& objects, LineIterator startIter,
	                             LineIterator endIter);
	void drawChart(unsigned int start, unsigned int end);
	void drawPlayBar();
	void checkLaserSelect(QuadArray& arr, int laser);
	void drawLaserQuads(const QuadArray& arr);
	std::vector<sf::VertexArray> generateSlamQuads(int llineNumine, float start, float end, int laser, bool isWide);

	int getMouseLane();
	//int getMouseMeasure();
	int getMouseLine();
	int getSnappedLine(int line);
	float getMouseLaserPos(bool isWide);

	int getMeasureFromLine(unsigned int loc);

	void handleEvent(sf::Event event);
	float getLaserX(ChartLine* line, int laser);

	sf::Vector2f getSnappedPos(ToolType type);
	//get the note location from measure, lane, and line positio
	//sf::Vector2f getNoteLocation(int measure, int lane, int line);

	//get the note location from line (global) and lane
	sf::Vector2f getNoteLocation(int lane, int line);
	//get the note location from line (global)
	sf::Vector2f getNoteLocation(int line);

	void conenctLines(std::map<unsigned int, ChartLine*> input);

	float triArea(sf::Vector2f A, sf::Vector2f B, sf::Vector2f C);
	bool getMouseOverlap(sf::VertexArray quad);
	//QuadArray wrapLaserQuads(QuadArray& arr);

	void loadFile(std::string mapFilePath, std::string mapFileName);
	void saveFile(std::string fileName);
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

	std::map<unsigned int, ChartLine*> selectedLines;

	std::map<unsigned int, ChartLine> clipboard;

	Chart chart;
	//functions
};
