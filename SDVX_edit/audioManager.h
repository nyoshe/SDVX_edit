#pragma once
#include <iostream>
#include <string>
#include <string>
#include "imgui.h"
#include "SoundTouch.h"
#include "plog/Log.h"
#include "SFML/Audio.hpp"

using namespace soundtouch;

class MyStream : public sf::SoundStream
{
public:
	void load(const sf::SoundBuffer& buffer)
	{
		processor.setChannels(buffer.getChannelCount());
		processor.setSampleRate(buffer.getSampleRate());
		processor.setSetting(SETTING_USE_AA_FILTER, 0);
		processor.setSetting(SETTING_SEQUENCE_MS, 5);

		free(output);
		free(input);

		//buffer half of a second
		buffSize = 0.5 * buffer.getSampleRate() * buffer.getChannelCount();
		output = static_cast<int16_t*>(malloc(sizeof(int16_t) * buffSize));

		input = static_cast<int16_t*>(malloc(sizeof(int16_t) * buffer.getChannelCount() * buffer.getSampleCount()));

		inputSize = buffer.getSampleCount();
		memcpy(input, buffer.getSamples(), sizeof(sf::Int16) * buffer.getSampleCount());

		// reset the current playing position 
		m_currentSample = 0;

		// initialize the base class
		initialize(buffer.getChannelCount(), buffer.getSampleRate());
	}

	~MyStream() override
	{
		free(output);
		free(input);
	}

	void setProcessorPitch(float p)
	{
		processor.setPitch(p);
		pitch = p;
	}

	float pitch = 1.0;
	int lastSampleLength = 0;
	int sampleDelay;
	SoundTouch processor;
	int buffSize = 50000;
	float delayMs = 0;

private:
	bool onGetData(Chunk& data) override
	{
		data.samples = output;

		int nSamples;

		int buffSizeSamples = buffSize / getChannelCount();

		nSamples = processor.receiveSamples(output, buffSizeSamples);
		delayMs = 1000.f * static_cast<float>(buffSizeSamples - nSamples) / getSampleRate();


		// have we reached the end of the sound?
		if (m_currentSample + nSamples <= inputSize) {
			data.sampleCount = nSamples * getChannelCount();
			processor.putSamples(&input[m_currentSample + buffSize], nSamples);
			m_currentSample += nSamples * getChannelCount();
			lastSampleLength = nSamples;
			return true;
		}
		// end of stream reached: stream the remaining samples and stop playback
		data.sampleCount = inputSize - m_currentSample;
		m_currentSample = inputSize;
		return false;
	}

	void onSeek(sf::Time timeOffset) override
	{
		processor.clear();
		// compute the corresponding sample index according to the sample rate and channel count
		m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
		processor.putSamples(&input[m_currentSample], buffSize / 2);
	}


	//int16_t* input;
	int16_t* output;

	long inputSize;
	int16_t* input;
	std::size_t m_currentSample;
};


class AudioManager
{
private:
	float seekTime;

	float playbackSpeed;

public:
	sf::Clock playClock;
	MyStream track;
	bool trackValid = false;
	bool loadFile(std::string fileName);
	void playFrom(int ms);
	void stop();
	unsigned int getMs();
	bool isPlaying();
	void setSpeed(float speed);
};
