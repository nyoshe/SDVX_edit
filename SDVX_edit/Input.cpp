#include "Input.h"
#include <plog/Log.h>

template<typename K, typename V>
static std::map<V, K> swap_map(const std::map<K, V>& m) {
	std::map<V, K> r;
	for (const auto& kv : m)
		r[kv.second] = kv.first;
	return r;
}

Input::Input() {
	if (buttonStrings.size() == 0) {
#define INSERT_ELEMENT(p) buttonStrings[sf::Mouse::p] = #p
		INSERT_ELEMENT(Left);
		INSERT_ELEMENT(Right);
		INSERT_ELEMENT(Middle);
		INSERT_ELEMENT(XButton1);
		INSERT_ELEMENT(XButton2);
		INSERT_ELEMENT(ButtonCount);

#undef INSERT_ELEMENT
	}

		//thank god for sublime text multiline selection

	if (keyStrings.size() == 0) {
#define INSERT_ELEMENT(p) keyStrings[sf::Keyboard::p] = #p
		INSERT_ELEMENT(Unknown);
		INSERT_ELEMENT(A);
		INSERT_ELEMENT(B);
		INSERT_ELEMENT(C);
		INSERT_ELEMENT(D);
		INSERT_ELEMENT(E);
		INSERT_ELEMENT(F);
		INSERT_ELEMENT(G);
		INSERT_ELEMENT(H);
		INSERT_ELEMENT(I);
		INSERT_ELEMENT(J);
		INSERT_ELEMENT(K);
		INSERT_ELEMENT(L);
		INSERT_ELEMENT(M);
		INSERT_ELEMENT(N);
		INSERT_ELEMENT(O);
		INSERT_ELEMENT(P);
		INSERT_ELEMENT(Q);
		INSERT_ELEMENT(R);
		INSERT_ELEMENT(S);
		INSERT_ELEMENT(T);
		INSERT_ELEMENT(U);
		INSERT_ELEMENT(V);
		INSERT_ELEMENT(W);
		INSERT_ELEMENT(X);
		INSERT_ELEMENT(Y);
		INSERT_ELEMENT(Z);
		INSERT_ELEMENT(Num0);
		INSERT_ELEMENT(Num1);
		INSERT_ELEMENT(Num2);
		INSERT_ELEMENT(Num3);
		INSERT_ELEMENT(Num4);
		INSERT_ELEMENT(Num5);
		INSERT_ELEMENT(Num6);
		INSERT_ELEMENT(Num7);
		INSERT_ELEMENT(Num8);
		INSERT_ELEMENT(Num9);
		INSERT_ELEMENT(Escape);
		INSERT_ELEMENT(LControl);
		INSERT_ELEMENT(LShift);
		INSERT_ELEMENT(LAlt);
		INSERT_ELEMENT(LSystem);
		INSERT_ELEMENT(RControl);
		INSERT_ELEMENT(RShift);
		INSERT_ELEMENT(RAlt);
		INSERT_ELEMENT(RSystem);
		INSERT_ELEMENT(Menu);
		INSERT_ELEMENT(LBracket);
		INSERT_ELEMENT(RBracket);
		INSERT_ELEMENT(Semicolon);
		INSERT_ELEMENT(Comma);
		INSERT_ELEMENT(Period);
		INSERT_ELEMENT(Slash);
		INSERT_ELEMENT(Backslash);
		INSERT_ELEMENT(Equal);
		INSERT_ELEMENT(Hyphen);
		INSERT_ELEMENT(Space);
		INSERT_ELEMENT(Enter);
		INSERT_ELEMENT(Backspace);
		INSERT_ELEMENT(Tab);
		INSERT_ELEMENT(PageUp);
		INSERT_ELEMENT(PageDown);
		INSERT_ELEMENT(End);
		INSERT_ELEMENT(Home);
		INSERT_ELEMENT(Insert);
		INSERT_ELEMENT(Delete);
		INSERT_ELEMENT(Add);
		INSERT_ELEMENT(Subtract);
		INSERT_ELEMENT(Multiply);
		INSERT_ELEMENT(Divide);
		INSERT_ELEMENT(Left);
		INSERT_ELEMENT(Right);
		INSERT_ELEMENT(Up);
		INSERT_ELEMENT(Down);
		INSERT_ELEMENT(Numpad0);
		INSERT_ELEMENT(Numpad1);
		INSERT_ELEMENT(Numpad2);
		INSERT_ELEMENT(Numpad3);
		INSERT_ELEMENT(Numpad4);
		INSERT_ELEMENT(Numpad5);
		INSERT_ELEMENT(Numpad6);
		INSERT_ELEMENT(Numpad7);
		INSERT_ELEMENT(Numpad8);
		INSERT_ELEMENT(Numpad9);
		INSERT_ELEMENT(F1);
		INSERT_ELEMENT(F2);
		INSERT_ELEMENT(F3);
		INSERT_ELEMENT(F4);
		INSERT_ELEMENT(F5);
		INSERT_ELEMENT(F6);
		INSERT_ELEMENT(F7);
		INSERT_ELEMENT(F8);
		INSERT_ELEMENT(F9);
		INSERT_ELEMENT(F10);
		INSERT_ELEMENT(F11);
		INSERT_ELEMENT(F12);
		INSERT_ELEMENT(F13);
		INSERT_ELEMENT(F14);
		INSERT_ELEMENT(F15);
		INSERT_ELEMENT(Pause);
#undef  INSERT_ELEMENT
	}
	keyEnums = swap_map(keyStrings);
	buttonEnums = swap_map(buttonStrings);

	std::ifstream input(fileName);

	//TODO: loading from file

	//check if bindings file exists
	if (input.good()) {
		
		std::string s;
		std::string actionType;
		std::vector<std::string> actionList = {};
		std::vector<sf::Keyboard::Key> keyList = {};
		std::vector<sf::Mouse::Button> buttonList = {};
		while (getline(input, s)) {
			if (s.find("action:") != std::string::npos) {
				actionList = splitCommas(s.substr(s.find(":") + 1));
			}
			if (s.find("type:") != std::string::npos) {
				actionType = s.substr(s.find(":") + 1);
			}
			if (s.find("keys:") != std::string::npos) {
				std::vector<std::string> keyStr = splitCommas(s.substr(s.find(":") + 1));
				for (auto key : keyStr) {
					keyList.push_back(keyEnums[key]);
				}
			}
			if (s.find("buttons:") != std::string::npos) {
				std::vector<std::string> buttonStr = splitCommas(s.substr(s.find(":") + 1));
				for (auto button : buttonStr) {
					buttonList.push_back(buttonEnums[button]);
				}
			}
			if (s.find("endBinding") != std::string::npos) {
				for (auto action : actionList) {
					nameMap[action] = std::make_pair(keyList, buttonList);
					actionTypeMap[action] = actionType;
					actionList.clear();
					keyList.clear();
					buttonList.clear();
				}
			}
		}
		
	}
	std::cout << "test";
	//else just return nothing
}

Input::~Input() {
	std::ofstream output(fileName);
	writeBinding(output, keyDownFunctions, onKeyDown);
	writeBinding(output, keyUpFunctions, onKeyUp);
	writeBinding(output, mouseDownFunctions, onButtonDown);
	writeBinding(output, mouseUpFunctions, onButtonUp);
}

std::vector<std::string>  Input::splitCommas(std::string str) {
	std::stringstream ss(str);
	std::vector<std::string> out = {};
	if (str.empty()) {
		return out;
	}
	while (ss.good()) {
		std::string substr;
		getline(ss, substr, ',');
		out.push_back(substr);
	}
	return out;
}

void Input::writeBinding(std::ofstream& out, const InputMap& map, std::string type) {
	for (auto mapping : map) {
		out << "action:";
		for (auto name : mapping.second) {
			out << name.first << ((name.first != mapping.second.back().first) ? "," : "");
		}
		out << std::endl;
		out << "type:" << type << std::endl;

		if (mapping.first.first.size() > 0) {
			out << "keys:";
			for (auto key : mapping.first.first) {
				out << keyStrings[key] << ((key != mapping.first.first.back()) ? "," : "");
			}
			out << std::endl;
		}
		
		if (mapping.first.second.size() > 0) {
			out << "buttons:";
			for (auto button : mapping.first.second) {
				out << buttonStrings[button] << ((button != mapping.first.second.back()) ? "," : "");
			}
			out << std::endl;
		}
		
		out << "endBinding" << std::endl;
		out << std::endl;
	}
}

void Input::handleEvent(sf::Event event) {
	std::vector<sf::Keyboard::Key> keyVec;
	std::vector<sf::Mouse::Button> mouseVec;
	for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
		if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i))) {
			keyVec.push_back(static_cast<sf::Keyboard::Key>(i));
		}
	}
	for (int i = 0; i < sf::Mouse::ButtonCount; i++) {
		if (sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i))) {
			mouseVec.push_back(static_cast<sf::Mouse::Button>(i));
		}
	}


	switch (event.type) {
		case sf::Event::KeyPressed:
			if (keyDownFunctions.find(std::make_pair(keyVec, mouseVec)) != keyDownFunctions.end()) {
				execute(keyDownFunctions[std::make_pair(keyVec, mouseVec)]);
			}
			break;
		case event.KeyReleased:
			if (keyUpFunctions.find(std::make_pair(keyVec, mouseVec)) != keyUpFunctions.end()) {
				execute(keyUpFunctions[std::make_pair(keyVec, mouseVec)]);
			}
			break;
		case event.MouseButtonPressed:
			if (mouseDownFunctions.find(std::make_pair(keyVec, mouseVec)) != mouseDownFunctions.end()) {
				execute(mouseDownFunctions[std::make_pair(keyVec, mouseVec)]);
			}
			break;
		case event.MouseButtonReleased:
			if (mouseUpFunctions.find(std::make_pair(keyVec, mouseVec)) != mouseUpFunctions.end()) {
				execute(mouseUpFunctions[std::make_pair(keyVec, mouseVec)]);
			}
			break;
	}
}

void Input::execute(std::vector<std::pair<std::string, std::function<void(void)>>>& functionObjects) {

	for (auto function : functionObjects) {
		try {
			function.second();
		}
		catch (std::bad_function_call& e)
		{
			PLOG_WARNING << "Bad function call for function: "<< function.first;
		}
	}
}

void Input::addAction(InputMap& map, std::vector<sf::Keyboard::Key> keyList, std::vector<sf::Mouse::Button> buttonList, std::function<void(void)> func, std::string name) {
	//check if we already have a binding
	if (nameMap.find(name) != nameMap.end()) {
		if (actionTypeMap[name] == onKeyDown) {
			keyDownFunctions[nameMap[name]].push_back(std::make_pair(name, func));
		}
		if (actionTypeMap[name] == onKeyUp) {
			keyUpFunctions[nameMap[name]].push_back(std::make_pair(name, func));
		}
		if (actionTypeMap[name] == onButtonDown) {
			mouseDownFunctions[nameMap[name]].push_back(std::make_pair(name, func));
		}
		if (actionTypeMap[name] == onButtonUp) {
			mouseUpFunctions[nameMap[name]].push_back(std::make_pair(name, func));
		}
	}
	else {
		map[std::make_pair(keyList, buttonList)].push_back(std::make_pair(name, func));
	}
}

void Input::addKeyDownAction(std::vector<sf::Keyboard::Key> keyList, std::vector<sf::Mouse::Button> mouseList, std::function<void(void)> func, std::string name) {
	std::sort(keyList.begin(), keyList.end());
	std::sort(mouseList.begin(), mouseList.end());
	addAction(keyDownFunctions, keyList, mouseList, func, name);
	//keyDownFunctions[std::make_pair(keyList, mouseList)].push_back(std::make_pair(name, func));
}

void Input::addKeyDownAction(std::vector<sf::Keyboard::Key> keyList, std::function<void(void)> func, std::string name) {
	addKeyDownAction(keyList, {}, func, name);
}

void Input::addKeyUpAction(std::vector<sf::Keyboard::Key> keyList, std::vector<sf::Mouse::Button> mouseList, std::function<void(void)> func, std::string name) {
	std::sort(keyList.begin(), keyList.end());
	std::sort(mouseList.begin(), mouseList.end());
	addAction(keyDownFunctions, keyList, mouseList, func, name);
}

void Input::addKeyUpAction(std::vector<sf::Keyboard::Key> keyList, std::function<void(void)> func, std::string name) {
	addKeyUpAction(keyList, {}, func, name);
}


void Input::addMouseDownAction(std::vector<sf::Keyboard::Key> keyList, std::vector<sf::Mouse::Button> mouseList, std::function<void(void)> func, std::string name) {\
	std::sort(keyList.begin(), keyList.end());
	std::sort(mouseList.begin(), mouseList.end());
	addAction(keyDownFunctions, keyList, mouseList, func, name);
}

void Input::addMouseDownAction(std::vector<sf::Mouse::Button> mouseList, std::function<void(void)> func, std::string name) {
	addMouseDownAction({}, mouseList, func, name);
}

void Input::addMouseUpAction(std::vector<sf::Keyboard::Key> keyList, std::vector<sf::Mouse::Button> mouseList, std::function<void(void)> func, std::string name) {\
	std::sort(keyList.begin(), keyList.end());
	std::sort(mouseList.begin(), mouseList.end());
	addAction(keyDownFunctions, keyList, mouseList, func, name);
}

void Input::addMouseUpAction(std::vector<sf::Mouse::Button> mouseList, std::function<void(void)> func, std::string name) {
	addMouseUpAction({}, mouseList, func, name);
}