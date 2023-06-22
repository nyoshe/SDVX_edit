#include "scrubBar.h"
void ScrubBar::update() {
	tex.create(100,window->getSize().x);
	tex.clear();
	EditWindow &editor = EditWindow::getInstance();

	sf::RenderTarget* beforeWin = editor.window;

	float bottomPadding = editor.bottomPadding;
	float topPadding = editor.topPadding;
	int columns = editor.columns;
	int measureNum = editor.measuresPerColumn;
	int totalMeasures = editor.chart.measures.size();
	int viewLines = editor.viewLines;
	int beatsPerColumn = editor.beatsPerColumn;
	int width = editor.width;
	int editorLineStart = editor.editorLineStart;
	//begin evil gaslighting of editor window
	editor.bottomPadding = 0;
	editor.topPadding = 0;
	editor.columns = 1;
	editor.measuresPerColumn = totalMeasures;
	editor.editorLineStart = 0;
	editor.viewLines = 2000;
	editor.beatsPerColumn = 2000;
	editor.window = &tex;
	editor.width = 100;
	//editor.updateVars();

	editor.setWindow(&tex);
	
	editor.drawChart();

	editor.setWindow(beforeWin);

	//gaslight it into thinking everything is fine
	//editor.window = beforeWin;
	editor.bottomPadding = bottomPadding;
	editor.topPadding = topPadding;
	editor.columns = columns;
	editor.measuresPerColumn = measureNum;
	editor.viewLines = viewLines;
	editor.beatsPerColumn = beatsPerColumn;
	editor.width = width;
	editor.editorLineStart = editorLineStart;
	editor.updateVars();

	tex.display();
}
void ScrubBar::draw() {
	sf::Sprite spr(tex.getTexture());
	spr.setRotation(90);
	spr.setPosition(sf::Vector2f(window->getSize().x, 100));
	window->draw(spr);
}
void ScrubBar::setWindow(sf::RenderWindow* _window) {

	window = _window;
}