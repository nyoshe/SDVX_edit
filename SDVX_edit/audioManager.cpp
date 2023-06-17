#include "audioManager.h"

void AudioManager::loadFile(std::string fileName) {

	if (!buffer.loadFromFile(fileName)) {
		std::cout << "couldn't find sound file at: " + fileName;
	}

	track.load(buffer);
}

void AudioManager::playFrom(int ms) {
	track.setPlayingOffset(sf::milliseconds(ms));
	track.play();
}

void AudioManager::stop() {
	track.stop();
}

unsigned int AudioManager::getMs() {
	return track.getPlayingOffset().asMilliseconds();
}

bool AudioManager::isPlaying() {
	return (track.getStatus() == sf::Sound::Playing);
}

void AudioManager::setSpeed(float speed) {
	track.setProcessorPitch(1.0 / speed);
	playbackSpeed = speed;
	track.setPitch(playbackSpeed);
}