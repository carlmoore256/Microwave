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
	
	ringBuffSize = winSize * 32;
	ringBuffSizeStep = winSize * 8;
	
	ringBuffer.setSize(microwaveContinue.getNumChannels(), ringBuffSize);
	ringBuffer.clear();
	
	// initialize ring buffer
	for (int i = 0; i < 16; i++)
		writeGranularToRing(microwaveContinue, m_windowSize, m_hopSize);
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
		writeRingToOutput(outBuffer, microwaveContinue);

//		writeGranularSamples(outBuffer, microwaveContinue, m_windowSize, m_hopSize);
	}
}

void OperationSound::startPlay()
{
	ringBuffReadIdx = m_windowSize;
	bufferPlaying = true;
	playingStartup = true;
	sampleIndex = 0;
	// initialize ring buffer
	for (int i = 0; i < 16; i++)
		writeGranularToRing(microwaveContinue, m_windowSize, m_hopSize);
}

void OperationSound::stopPlay()
{
	bufferPlaying = false;
	grainBuffer.clear();
}

// =====================================================================

void OperationSound::writeGranularToRing(AudioBuffer<float>& sourceBuffer, int grainSize, int hopSize)
{
	Random rand;
	int sourceBuffSize = sourceBuffer.getNumSamples();
	int numHops = 0;
	int numWin = 0;
	
	
	int startIdx = ringBuffWriteIdx;

//	while ((numWin * hopSize) + grainSize < ringBuffSizeStep)
	for (int i = 0; i < (ringBuffSizeStep-grainSize) / hopSize; i++)
	{
		// location within source grain to copy to buffer
		int grainStartIdx = 0;
		int grainNumSamples = grainSize;
		
//		 ensure beginning  of buffer is properly filled
		if(ringBuffWriteIdx < grainSize && (grainSize)-(numHops * hopSize) > 0)
		{
			grainStartIdx = (grainSize) - (numHops * hopSize);
			grainNumSamples = grainSize - grainStartIdx;
		}
//
		if(ringBuffWriteIdx + grainSize > ringBuffSize)
			grainNumSamples = ringBuffSize - ringBuffWriteIdx;

		
		for(int channel=0; channel<ringBuffer.getNumChannels(); channel++)
		{
			int playhead = rand.nextInt(sourceBuffSize - grainSize - 1);
			// copy from source to grain buffer
			grainBuffer.copyFrom(0, 0, sourceBuffer, channel % sourceBuffer.getNumChannels(), playhead, grainSize);
			// get write pointer to intermediate grain buffer
			auto* grainBuffData = grainBuffer.getWritePointer(channel);
			// apply a window to the grain buffer
			window.multiplyWithWindowingTable(grainBuffData, grainSize);
			
//			std::cout << " buff mag " + std::to_string(grainBuffer.getRMSLevel(0, 0, grainSize)) + "\n";
//			// add the windowed grain to the output buffer
//			ringBuffer.addFrom(channel, ringBuffWriteIdx, grainBuffer, channel, grainStartIdx, grainNumSamples);
			// add the windowed grain to the output buffer
			ringBuffer.addFrom(channel, ringBuffWriteIdx, grainBuffer, channel, grainStartIdx, grainNumSamples);
			
			// take care of the wrap around when running into end of buffer
			if (grainNumSamples != grainSize)
			{

				ringBuffer.addFrom(channel,
								   0,
								   grainBuffer,
								   channel,
								   grainNumSamples,
								   grainSize-grainNumSamples);
			}
		}
		// step forward in hop
		numHops++;
		numWin++;
		
		ringBuffWriteIdx += hopSize;
		
		if(ringBuffWriteIdx >= ringBuffSize)
			numHops = 0;
		
		ringBuffWriteIdx %= ringBuffSize;

	}
	
//	std::cout << "DEST PLAYHEAD " + std::to_string(destPlayhead) + " ring buff write idx " + std::to_string(ringBuffWriteIdx) + "\n";
	
	std::cout << "START IDX " + std::to_string(startIdx) + " END IDX " + std::to_string(ringBuffWriteIdx) + " PLAY IDX " + std::to_string(ringBuffReadIdx) + "\n";
	
}

void OperationSound::writeRingToOutput(AudioBuffer<float>& outBuffer, AudioBuffer<float>& sourceBuffer)
{
	
	int blockSize = outBuffer.getNumSamples();
	
//	if (ringBuffReadIdx % ringBuffSizeStep >= ringBuffSizeStep - 1)
//	{
//		writeGranularToRing(sourceBuffer, m_windowSize, m_hopSize);
//	}
//	if (ringBuffSamplesRead >= ringBuffSizeStep)
//	{
//		writeGranularToRing(sourceBuffer, m_windowSize, m_hopSize);
//		ringBuffSamplesRead = 0;
//	}

	
	for(int channel = 0; channel < outBuffer.getNumChannels(); channel++)
		outBuffer.addFrom(channel, 0, ringBuffer, channel % ringBuffer.getNumChannels(), ringBuffReadIdx, blockSize);
	
	// clear out part played
//	ringBuffer.clear(ringBuffReadIdx, blockSize);
	
	ringBuffReadIdx += blockSize;
	
//	if (ringBuffReadIdx >= ringBuffSize)
//		ringBuffReadIdx = 0;
	
	ringBuffReadIdx %= ringBuffSize;
	
	// process next batch into the ring buffer
//	ringBuffSamplesRead += blockSize;

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
	
//	std::cout << "num grains " + std::to_string(numGrains) + "\n";
	
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
