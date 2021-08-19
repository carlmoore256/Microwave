/*
  ==========================================================================

    OperationSound.cpp
    Created: 19 Aug 2021 12:54:58pm
    Author:  Carl Moore

  ==========================================================================
*/

#include "OperationSound.h"

OperationSound::OperationSound(int winSize, int hopSize) : m_windowSize(winSize), m_hopSize(hopSize), window(winSize, dsp::WindowingFunction<float>::hann)
{
	loadBuffer(microwaveStart, "Microwave_Start_wav");
	loadBuffer(microwaveContinue, "Microwave_Continue_wav");
//	std::cout << "numstd::to_string(microwaveContinue -> getNumSamples())
	grainBuffer.setSize(1, winSize);
	grainBuffer.clear();
	
	ringBuffSize = winSize * 16;
	ringBuffSizeStep = winSize * 4;
	
	ringBuffer.setSize(microwaveContinue.getNumChannels(), ringBuffSize);
	ringBuffer.clear();
}

OperationSound::~OperationSound()
{
//	delete microwaveStart;
//	delete microwaveContinue;
}

void OperationSound::writeNextBuffer(AudioBuffer<float>& outBuffer)
{
	if (!bufferPlaying)
		return;
	
	// play startup sound
	if(playingStartup)
	{
		if (sampleIndex + outBuffer.getNumSamples() > microwaveStart.getNumSamples())
		{
			playingStartup = false;
			sampleIndex = 0; // reset buffer index to 0
		}
		
		for (int channel = 0; channel < outBuffer.getNumChannels(); channel++)
			outBuffer.addFrom(channel, 0, microwaveStart, channel % microwaveStart.getNumChannels(), sampleIndex, outBuffer.getNumSamples());
		
		sampleIndex += outBuffer.getNumSamples();
	} else {
		writeGranularSamples(outBuffer, microwaveContinue, m_windowSize, m_hopSize);
	}
}

void OperationSound::startPlay()
{
	bufferPlaying = true;
	playingStartup = true;
	sampleIndex = 0;
}

void OperationSound::stopPlay()
{
	bufferPlaying = false;
	grainBuffer.clear();
}

// =====================================================================

void OperationSound::writeGranularToRing(AudioBuffer<float>& sourceBuffer, int grainSize, int hopSize)
{
	if (ringBuffWriteIdx + ringBuffSizeStep > ringBuffSize)
		ringBuffWriteIdx = 0;
	
	Array<int> granularPlayheads = getGranularPlayheads(sourceBuffer.getNumSamples(), grainSize, ringBuffSizeStep, hopSize);
	
	int destPlayhead = 0;
	
	for(int playhead : granularPlayheads)
	{
		for(int channel = 0; channel < ringBuffer.getNumChannels(); channel++)
		{
			// copy from source to grain buffer
			grainBuffer.copyFrom(0, 0, sourceBuffer, channel % sourceBuffer.getNumChannels(), playhead, grainSize);
			// get write pointer to intermediate grain buffer
			auto* grainBuffData = grainBuffer.getWritePointer(channel);
			// apply a window to the grain buffer
			window.multiplyWithWindowingTable(grainBuffData, grainSize);
			
			// add the windowed grain to the output buffer
			ringBuffer.addFrom(channel, ringBuffWriteIdx + destPlayhead, grainBuffer, channel, 0, grainSize);
		}
		// step forward in hop
		destPlayhead += hopSize;
	}
	ringBuffWriteIdx += ringBuffSizeStep;
}

void OperationSound::writeRingToOutput(AudioBuffer<float>& outBuffer, AudioBuffer<float>& sourceBuffer)
{
	int blockSize = outBuffer.getNumSamples();
	
	if(ringBuffReadIdx + blockSize > ringBuffSize)
		ringBuffReadIdx = 0;
	
	for(int channel = 0; channel < outBuffer.getNumChannels(); channel++)
	{
		outBuffer.addFrom(channel, 0, ringBuffer, channel % ringBuffer.getNumChannels(), ringBuffReadIdx, blockSize);
	}
	ringBuffSamplesRead += blockSize;
	if (ringBuffSamplesRead >= ringBuffSizeStep)
		writeGranularToRing(sourceBuffer, m_windowSize, m_hopSize);
}

// outBuffer is the destination, source provides samples and is granulated
void OperationSound::writeGranularSamples(AudioBuffer<float>& outBuffer, AudioBuffer<float>& sourceBuffer, int grainSize, int hopSize)
{
	// gets an array of random playback positions in the source buffer
	Array<int> granularPlayheads = getGranularPlayheads(sourceBuffer.getNumSamples(), grainSize, outBuffer.getNumSamples(), hopSize);
	
//	std::cout << "len of gran playheads " + std::to_string(granularPlayheads.size()) + "\n";
	
	int destPlayhead = 0;
	
	for(int playhead : granularPlayheads)
	{
		for(int channel = 0; channel < outBuffer.getNumChannels(); channel++)
		{
			// copy from source to grain buffer
			grainBuffer.copyFrom(0, 0, sourceBuffer, channel % sourceBuffer.getNumChannels(), playhead, grainSize);
			// get write pointer to intermediate grain buffer
			auto* grainBuffData = grainBuffer.getWritePointer(channel);
			// apply a window to the grain buffer
			window.multiplyWithWindowingTable(grainBuffData, grainSize);
			// add the windowed grain to the output buffer
			outBuffer.addFrom(channel, destPlayhead, grainBuffer, channel, 0, grainSize);
		}
		// step forward in hop
		destPlayhead += hopSize;
	}
}

Array<int> OperationSound::getGranularPlayheads(int bufferSize, int grainSize, int blockSize, int hopSize)
{
	int numGrains = (int)((float)(blockSize - grainSize) / (float)hopSize);
	
	std::cout << "num grains " + std::to_string(numGrains) + "\n";
	
	Array<int> granularPlayheads;

	Random random;
	
	for (int i = 0; i < numGrains; i++)
		granularPlayheads.add(random.nextInt(bufferSize - grainSize - 1));
	
	return granularPlayheads;
}

void OperationSound::loadBuffer(AudioBuffer<float>& buffer, std::string clipName)
{
	int clipSize;
	
	char* resourceName = &clipName[0];
	const char* clip = BinaryData::getNamedResource(resourceName, clipSize);
	MemoryInputStream inputStream(clip, clipSize, false);
	WavAudioFormat wavFormat;
	AudioFormatReader* formatReader = wavFormat.createReaderFor (&inputStream, false);
	
	buffer.setSize(formatReader->numChannels, formatReader->lengthInSamples);
	
//	buffer = new AudioBuffer<float>(formatReader->numChannels, formatReader->lengthInSamples);
	 
	formatReader->read(&buffer, 0, buffer.getNumSamples(), 0, true, true);
}
