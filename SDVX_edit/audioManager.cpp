#include "audioManager.h"

void AudioManager::loadFile(std::string fileName) {

	if (!buffer.loadFromFile(fileName)) {
		std::cout << "couldn't find sound file at: " + fileName;
	}

	track.load(buffer);
}

void AudioManager::playFrom(int ms) {
	seekTime = ms;
	track.setPlayingOffset(sf::milliseconds(ms));
	track.play();
	playClock.restart();
}

void AudioManager::stop() {
	track.stop();
}

unsigned int AudioManager::getMs() {
	//return playClock.getElapsedTime().asMilliseconds() + seekTime;
	return track.getPlayingOffset().asMilliseconds() - track.delayMs;
}

bool AudioManager::isPlaying() {
	return (track.getStatus() == sf::Sound::Playing);
}

void AudioManager::setSpeed(float speed) {
	track.setProcessorPitch(1.0 / speed);
	playbackSpeed = speed;
	track.setPitch(playbackSpeed);
}