#include "scrubBar.h"
void ScrubBar::update() {
	if (!enabled) return;
	EditWindow &editor = EditWindow::getInstance();

	sf::RenderWindow* beforeWin = editor.appWindow;

	sf::RenderTexture targetTex;

	float height = window->getSize().y * yScale;
	
	//editor.updateVars();

	int measuresPerColumn = editor.measuresPerColumn;
	int columns = editor.columns;
	int editorMeasure = editor.editorMeasure;

	editor.editorMeasure = currentMeasure;
	editor.columns = 1;
	editor.measuresPerColumn = 1;

	targetTex.create(height, window->getSize().x / static_cast<float>(displayMeasures));

	editor.setWindow(&targetTex);
	
	editor.drawChart();

	editor.editorMeasure = editorMeasure;
	editor.measuresPerColumn = measuresPerColumn;
	editor.columns = columns;
	editor.setWindow(beforeWin);

	sf::Sprite spr(targetTex.getTexture());
	spr.setPosition(sf::Vector2f(0, window->getSize().x * (static_cast<float>(currentMeasure) / static_cast<float>(displayMeasures))));
	
	//tex.draw(spr);

	sf::BlendMode blender = sf::BlendMode(sf::BlendMode::Factor::One, sf::BlendMode::Factor::Zero, sf::BlendMode::Equation::Add);
	tex.draw(spr, blender);
	tex.display();
	
	currentMeasure++;

	if (currentMeasure > displayMeasures) {
		sf::Sprite newSpr;
		displaySpr = newSpr;
		displaySpr.setTexture(tex.getTexture());
		displaySpr.setRotation(-90);
		displaySpr.setPosition(sf::Vector2f(0, topPadding * window->getSize().y + height));
		currentMeasure = 0;
		//tex.clear();
		//tex.create(100, window->getSize().x);
	}
	float linePos = window->getSize().x * static_cast<float>(editor.editorMeasure) / static_cast<float>(displayMeasures);

	sf::Vertex line[] = {
		sf::Vertex(sf::Vector2f(linePos, topPadding * window->getSize().y), sf::Color::Red),
		sf::Vertex(sf::Vector2f(linePos, topPadding * window->getSize().y + height), sf::Color::Red)
	};
	
	window->draw(line, 2, sf::Lines);
	//window->draw(quad);
	
	window->draw(displaySpr);
}

void ScrubBar::setWindow(sf::RenderWindow* _window) {
	window = _window;
	tex.create(window->getSize().y * yScale, window->getSize().x);
	
}