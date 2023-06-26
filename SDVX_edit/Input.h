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

struct FunctionInfo {
	sf::Event::EventType eventType;
	std::string name;
	std::function<void(sf::Event)> function;
	FunctionInfo(sf::Event::EventType _eventType, std::string _name, std::function<void(sf::Event)> _function) : function(_function), name(_name), eventType(_eventType) {};
};

//might be nice to just turn this into a struct
typedef std::map < std::pair<std::vector<sf::Keyboard::Key>, std::vector<sf::Mouse::Button>>, std::vector<FunctionInfo>> InputMap;

class Input final : public Unique <Input>
{
private:
	//InputMap keyDownFunctions;
	//InputMap keyUpFunctions;
	//InputMap mouseDownFunctions;
	//InputMap mouseUpFunctions;

	InputMap functionMap;
	std::string fileName = "bindings.txt";

	std::map<std::string, std::pair<std::vector<sf::Keyboard::Key>, std::vector<sf::Mouse::Button>>> nameMap;
	std::map<std::string, sf::Event::EventType> eventTypeMap;

	std::map<sf::Mouse::Button, std::string> buttonStrings;
	std::map<sf::Keyboard::Key, std::string> keyStrings;
	std::map<sf::Event::EventType, std::string> eventStrings;

	std::map<std::string, sf::Mouse::Button> buttonEnums;
	std::map<std::string, sf::Keyboard::Key> keyEnums;
	std::map<std::string, sf::Event::EventType> eventEnums;

	std::string onButtonDown = "onButtonDown";
	std::string onButtonUp = "onButtonUp";
	std::string onKeyDown = "onKeyDown";
	std::string onKeyUp = "onKeyUp";

public:

	Input();
	~Input();

	void writeBinding(std::ofstream& out);

	void handleEvent(sf::Event event);

	std::vector<std::string> splitCommas(std::string str);


	void addAction(sf::Event::EventType event, std::vector<sf::Keyboard::Key>, std::vector<sf::Mouse::Button>, std::function<void(sf::Event)>, std::string name);
	void addActionMouse(sf::Event::EventType event, std::vector<sf::Mouse::Button>, std::function<void(sf::Event)>, std::string name);
	void addActionKey(sf::Event::EventType event, std::vector<sf::Keyboard::Key>, std::function<void(sf::Event)>, std::string name);
	void addEventAction(sf::Event::EventType event, std::function<void(sf::Event)>, std::string name);
};

