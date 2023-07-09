#include "fontManager.h"


void FontManager::loadDefault()
{
	//push default font
	ImGui::GetIO().Fonts->Clear();
	//load a default font
	addFont("default", 1.0);
}

FontManager::FontManager()
{
	loadDefault();
}

void FontManager::setWindow(sf::RenderWindow* _window)
{
	window = _window;
	scale = window->getSize().y / 900.0;
	needsUpdate = true;
}

void FontManager::setScale(float _scale)
{
	absoluteScale = _scale;
	needsUpdate = true;
}

float FontManager::getScale()
{
	return scale * absoluteScale;
}

float FontManager::getAbsoluteScale() {
	return absoluteScale;
}

void FontManager::addFont(std::string id, std::string file, float scale)
{
	FontInfo font;
	font.font = ImGui::GetIO().Fonts->AddFontFromFileTTF(file.c_str(), 16.f * scale);
	ImGui::SFML::UpdateFontTexture();
	font.scale = scale;
	font.file = file;
	fontMap.insert(std::make_pair(id, font));
}

void FontManager::addFont(std::string id, float scale)
{
	addFont(id, defaultFontFile.c_str(), scale);
}

void FontManager::updateFonts()
{
	if(ImGui::GetFrameCount())
		ImGui::SFML::Render(*window);
		
	ImGui::GetIO().Fonts->Clear();
	for (auto& [key, val] : fontMap) {
		val.font = ImGui::GetIO().Fonts->AddFontFromFileTTF(val.file.c_str(), 16.f * val.scale * getScale());
		ImGui::SFML::UpdateFontTexture();
	}
	if (ImGui::GetFrameCount())
		ImGui::NewFrame();
}

ImFont* FontManager::getFont(std::string id)
{
	if (fontMap.find(id) != fontMap.end()) {
		return fontMap[id].font;
	}
	else {
		return fontMap["default"].font;
	}
}

ImFont* FontManager::getDefaultFont()
{
	return fontMap["default"].font;
}

void FontManager::update()
{
	if (needsUpdate) {
		updateFonts();
		needsUpdate = false;
	}
}
