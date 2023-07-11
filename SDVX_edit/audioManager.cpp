#include "AudioManager.h"

bool AudioManager::loadFile(std::string fileName)
{
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile(fileName)) {
		PLOG_ERROR << "couldn't find sound file at: " + fileName;
		return false;
	}

	track.load(buffer);
	trackValid = true;
	return true;
}

void AudioManager::playFrom(int ms)
{
	if (trackValid) {
		seekTime = ms;
		track.setPlayingOffset(sf::milliseconds(ms));
		track.play();
		playClock.restart();
	}
}

void AudioManager::stop()
{
	track.stop();
}

unsigned int AudioManager::getMs()
{
	//return playClock.getElapsedTime().asMilliseconds() + seekTime;
	return track.getPlayingOffset().asMilliseconds() - track.delayMs;
}

bool AudioManager::isPlaying()
{
	return (track.getStatus() == sf::Sound::Playing);
}

void AudioManager::setSpeed(float speed)
{
	track.setProcessorPitch(1.0 / speed);
	playbackSpeed = speed;
	track.setPitch(playbackSpeed);
}
