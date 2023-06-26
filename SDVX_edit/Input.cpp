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
	if (eventStrings.size() == 0) {
#define INSERT_ELEMENT(p) eventStrings[sf::Event::EventType::p] = #p
		INSERT_ELEMENT(Resized);
		INSERT_ELEMENT(LostFocus);
		INSERT_ELEMENT(GainedFocus);
		INSERT_ELEMENT(TextEntered);
		INSERT_ELEMENT(KeyPressed);
		INSERT_ELEMENT(KeyReleased);
		INSERT_ELEMENT(MouseWheelMoved);
		INSERT_ELEMENT(MouseWheelScrolled);
		INSERT_ELEMENT(MouseButtonPressed);
		INSERT_ELEMENT(MouseButtonReleased);
		INSERT_ELEMENT(MouseMoved);
		INSERT_ELEMENT(MouseEntered);
		INSERT_ELEMENT(MouseLeft);
		INSERT_ELEMENT(JoystickButtonPressed);
		INSERT_ELEMENT(JoystickButtonReleased);
		INSERT_ELEMENT(JoystickMoved);
		INSERT_ELEMENT(JoystickConnected);
		INSERT_ELEMENT(JoystickDisconnected);
		INSERT_ELEMENT(TouchBegan);
		INSERT_ELEMENT(TouchMoved);
		INSERT_ELEMENT(TouchEnded);
		INSERT_ELEMENT(SensorChanged);

#undef INSERT_ELEMENT
	}

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
	eventEnums = swap_map(eventStrings);

	std::ifstream input(fileName);

	//check if bindings file exists
	if (input.good()) {
		
		std::string s;
		sf::Event::EventType eventType = sf::Event::EventType::Count;
		std::vector<std::string> actionList = {};
		std::vector<sf::Keyboard::Key> keyList = {};
		std::vector<sf::Mouse::Button> buttonList = {};
		while (getline(input, s)) {
			if (s.find("action:") != std::string::npos) {
				actionList = splitCommas(s.substr(s.find(":") + 1));
			}
			if (s.find("type:") != std::string::npos) {
				eventType = eventEnums[s.substr(s.find(":") + 1)];
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
					eventTypeMap[action] = eventType;
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
	writeBinding(output);
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

void Input::writeBinding(std::ofstream& out) {
	for (auto mapping : functionMap) {
		for (auto item : mapping.second) {
			out << "action:";
			out << item.name << ((item.name != mapping.second.back().name) ? "," : "");
			out << std::endl;
			out << "type:" << eventStrings[item.eventType] << std::endl;

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
	

	if (functionMap.find(std::make_pair(keyVec, mouseVec)) != functionMap.end()) {
		for (auto mapping : functionMap[std::make_pair(keyVec, mouseVec)]) {
			if (mapping.eventType == event.type) {
				try {
					mapping.function(event);
				}
				catch (std::bad_function_call& e)
				{
					PLOG_WARNING << "Bad function call for function: " << mapping.name;
				}
			}
		}
	}
}

void Input::addEventAction(sf::Event::EventType event, std::function<void(sf::Event)> func, std::string name) {
	addAction(event, {}, {}, func, name);
}


void Input::addActionMouse(sf::Event::EventType event,  std::vector<sf::Mouse::Button> buttonList, std::function<void(sf::Event)> func, std::string name) {
	addAction(event, {}, buttonList, func, name);
}

void Input::addActionKey(sf::Event::EventType event, std::vector<sf::Keyboard::Key> keyList, std::function<void(sf::Event)> func, std::string name) {
	addAction(event, keyList, {}, func, name);
}

void Input::addAction(sf::Event::EventType event, std::vector<sf::Keyboard::Key> keyList, std::vector<sf::Mouse::Button> buttonList, std::function<void(sf::Event)> func, std::string name) {
	std::sort(keyList.begin(), keyList.end());
	std::sort(buttonList.begin(), buttonList.end());
	//check if we already have a binding
	if (nameMap.find(name) != nameMap.end()) {
		if (eventTypeMap[name] == event) {
			functionMap[nameMap[name]].push_back(FunctionInfo(eventTypeMap[name], name, func));
		}
	}
	else {
		functionMap[std::make_pair(keyList, buttonList)].push_back(FunctionInfo(event, name, func));
	}
}
