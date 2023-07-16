#pragma once
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "chartLine.h"
#include  "Chart.h"

typedef std::vector<std::pair<ChartLine*, std::vector<sf::VertexArray>>> QuadArray;
typedef std::map<unsigned int, ChartLine*>::const_iterator LineIterator;

class ChartDisplay
{
private:
	void drawLineButtons(sf::RenderTarget& window, const ChartLine* line, bool selected);
	void drawSelected(sf::RenderTarget& window, const ChartLine* line, const sf::Sprite& sprite);
	void setVariables(sf::RenderTarget& window);
	QuadArray generateLaserQuads(int laser, const std::map<unsigned, ChartLine*>& objects,
	                             LineIterator startIter, LineIterator endIter, std::vector<sf::Color> cols);
	
	void drawLaserQuads(sf::RenderTarget& window, const QuadArray& arr);
	float triArea(sf::Vector2f A, sf::Vector2f B, sf::Vector2f C);
	bool getMouseOverlap(const sf::VertexArray quad, float mouseX, float mouseY);

	QuadArray qArray[2] = {};
	float laneWidth = 0;
	float width = 0;
	float height = 0;
	int visibleLines = 0;
	static constexpr int beatSeparation = 48;

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
	bool DEBUG = false;
	int offset = 0;
	ChartDisplay();
	float getLaserX(ChartLine* line, int laser);
	void draw(sf::RenderTarget& window, const Chart& chart, int _offset, int start, int end);
	void drawAsColor(sf::RenderTarget& window, std::map<unsigned, ChartLine*>& objects, sf::Color col,
	                 int _offset, int start, int end, LineMask mask);
	sf::Vector2f getNoteLocation(int lane, int line);
	sf::Vector2f getNoteLocation(int line);
	void drawLineButtons(sf::RenderTarget& window, const ChartLine* line, bool selected, sf::Color col);
	void drawSelection(sf::RenderTarget& window, std::map<unsigned int, ChartLine*>& selectedLines, int offset, int start, int end);
	ChartLine* getLaserHover(int laser, float mouseX, float mouseY);
	const sf::Color laserLColor = sf::Color(0, 160, 255, 150);
	const sf::Color laserRColor = sf::Color(255, 0, 200, 150);
};

