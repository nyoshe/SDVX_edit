#include "audioManager.h"

void audioManager::loadFile(std::string fileName) {
	if (!buffer.loadFromFile(fileName)) {
		std::cout << "couldn't find sound file at: " + fileName;
	}
	track.setBuffer(buffer);
}

void audioManager::playFrom(int ms) {
	track.setPlayingOffset(sf::milliseconds(ms));
	track.play();
}

void audioManager::stop() {
	track.pause();
}

unsigned int audioManager::getMs() {
	return track.getPlayingOffset().asMilliseconds();
}

bool audioManager::isPlaying() {
	return (track.getStatus() == sf::Sound::Playing);
}