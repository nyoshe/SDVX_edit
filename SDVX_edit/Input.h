#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <algorithm>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "Unique.h"

#include <functional>
#include <memory>

//might be nice to just turn this into a struct
typedef std::map < std::pair<std::vector<sf::Keyboard::Key>, std::vector<sf::Mouse::Button>>, std::vector<std::pair<std::string, std::function<void(void)>>>> InputMap;

class Input final : public Unique <Input>
{
private:
	InputMap keyDownFunctions;
	InputMap keyUpFunctions;
	InputMap mouseDownFunctions;
	InputMap mouseUpFunctions;
	std::string fileName = "bindings.txt";

	std::map<std::string, std::pair<std::vector<sf::Keyboard::Key>, std::vector<sf::Mouse::Button>>> nameMap;
	std::map<std::string, std::string> actionTypeMap;

	std::map<sf::Mouse::Button, std::string> buttonStrings;
	std::map<sf::Keyboard::Key, std::string> keyStrings;

	std::map<std::string, sf::Mouse::Button> buttonEnums;
	std::map<std::string, sf::Keyboard::Key> keyEnums;

	std::string onButtonDown = "onButtonDown";
	std::string onButtonUp = "onButtonUp";
	std::string onKeyDown = "onKeyDown";
	std::string onKeyUp = "onKeyUp";

public:

	Input();
	~Input();

	void writeBinding(std::ofstream& out, const InputMap& map, std::string type);

	void handleEvent(sf::Event event);

	void execute(std::vector<std::pair<std::string, std::function<void(void)>>>& functionObjects);

	//this checks to see if we already have a loaded binding
	void addAction(InputMap& map, std::vector<sf::Keyboard::Key> keyList, std::vector<sf::Mouse::Button> buttonList, std::function<void(void)> func, std::string name);

	std::vector<std::string> splitCommas(std::string str);


	//keyboard actions will trigger on a key down event
	void addKeyDownAction(std::vector<sf::Keyboard::Key>, std::vector<sf::Mouse::Button>, std::function<void(void)>, std::string name);
	void addKeyDownAction(std::vector<sf::Keyboard::Key>, std::function<void(void)>, std::string name);

	//keyboard actions will trigger on a key up event
	void addKeyUpAction(std::vector<sf::Keyboard::Key>, std::vector<sf::Mouse::Button>, std::function<void(void)>, std::string name);
	void addKeyUpAction(std::vector<sf::Keyboard::Key>, std::function<void(void)>, std::string name);
	
	//mouse actions will trigger on a mouse down event
	void addMouseDownAction(std::vector<sf::Keyboard::Key>, std::vector<sf::Mouse::Button>, std::function<void(void)>, std::string name);
	void addMouseDownAction(std::vector<sf::Mouse::Button>, std::function<void(void)>, std::string name);

	//mouse actions will trigger on a mouse up event
	void addMouseUpAction(std::vector<sf::Keyboard::Key>, std::vector<sf::Mouse::Button>, std::function<void(void)>, std::string name);
	void addMouseUpAction(std::vector<sf::Mouse::Button>, std::function<void(void)>, std::string name);
};

