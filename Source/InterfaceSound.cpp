//
//  InterfaceSound.cpp
//  Microwave
//
//  Created by Carl Moore on 8/18/21.
//  Copyright © 2021 ANVIL. All rights reserved.
//

//#include <stdio.h>
#include "InterfaceSound.h"

InterfaceSound::InterfaceSound(std::string clipName, int buffSize) : bufferSize(buffSize)
{
   int clipSize;
   char* resourceName = &clipName[0];
   const char* clip = BinaryData::getNamedResource(resourceName, clipSize);
   MemoryInputStream inputStream(clip, clipSize, false);
   WavAudioFormat wavFormat;
   AudioFormatReader* formatReader = wavFormat.createReaderFor (&inputStream, false);
   
   buffer = new AudioBuffer<float>(formatReader->numChannels, formatReader->lengthInSamples);
	
   bufferNumSamples = buffer -> getNumSamples();
   formatReader->read(buffer, 0, bufferNumSamples, 0, true, true);
	
}

InterfaceSound::~InterfaceSound()
{
	delete buffer;
}

void InterfaceSound::writeNextBuffer(AudioBuffer<float>& outBuffer)
{
	if (!bufferPlaying)
		return;
	
	if (sampleIndex + outBuffer.getNumSamples() > bufferNumSamples)
	{
		bufferPlaying = false;
		sampleIndex = 0; // reset buffer index to 0
		return;
	}
	
	for (int channel = 0; channel < outBuffer.getNumChannels(); channel++)
		outBuffer.addFrom(channel, 0, *buffer, channel % buffer->getNumChannels(), sampleIndex, outBuffer.getNumSamples());
	
	sampleIndex += outBuffer.getNumSamples();
}

void InterfaceSound::resetPlay()
{
	sampleIndex = 0;
	bufferPlaying = true;
}
