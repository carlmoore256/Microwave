//
//  PopcornProcessor.cpp
//  Microwave
//
//  Created by Carl Moore on 8/19/21.
//  Copyright © 2021 ANVIL. All rights reserved.
//

#include "PopcornProcessor.hpp"


PopcornProcessor::PopcornProcessor(int winSize, int channels) : windowSize(winSize), numChannels(channels), window(winSize, dsp::WindowingFunction<float>::hann)
{
	
	for(int i = 0; i < numBuffers; i++)
	{
		auto* buff = new AudioBuffer<float>(numChannels, winSize);
		buff -> clear();
		popcornBuffers.add(buff);
	}
}

PopcornProcessor::~PopcornProcessor()
{
	for(AudioBuffer<float>* buff : popcornBuffers)
		delete buff;
}

void PopcornProcessor::processBlock(AudioBuffer<float>& buffer)
{
	int numChan = buffer.getNumChannels();
	
	int test = numChannels;
	for(int channel = 0; channel < buffer.getNumChannels(); channel++)
	{
		float rms = buffer.getRMSLevel(channel, 0, buffer.getNumSamples());
		
		runningAvgRMS.add(rms);
		
		if (runningAvgRMS.size() > 256)
			runningAvgRMS.remove(0);
		
		float avgRMS = calculateAvgRMS();

		if (rms > avgRMS)
		{
			int buffIdx = rand.nextInt(numBuffers);
			popcornBuffers[buffIdx] -> clear();
			popcornBuffers[buffIdx] -> addFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
		}
		
		
		if(rms > avgRMS)
		{
			int buffIdx = rand.nextInt(numBuffers);
			
//			std::cout << popcornBuffers[buffIdx]->getNumChannels();
			buffer.addFrom(channel, 0, *popcornBuffers[buffIdx], channel, 0, popcornBuffers[buffIdx]->getNumSamples());
		}

	}

}

float PopcornProcessor::calculateAvgRMS()
{
	float avg = 0.f;
	for(float rms : runningAvgRMS)
		avg += rms;
	
	avg /= runningAvgRMS.size();
	
	return avg;
}

