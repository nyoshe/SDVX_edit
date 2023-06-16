#pragma once
#include "SFML/Audio.hpp"
#include <string>
#include <iostream>

class audioManager
{
private:
    sf::SoundBuffer buffer;

    sf::Sound track;
public:
    void loadFile(std::string fileName);
    void playFrom(int ms);
    void stop();
    unsigned int getMs();
    bool isPlaying();
};

