//
//  PopcornProcessor.cpp
//  Microwave
//
//  Created by Carl Moore on 8/19/21.
//  Copyright © 2021 ANVIL. All rights reserved.
//

#include "PopcornProcessor.hpp"


PopcornProcessor::PopcornProcessor(int winSize, int channels) : windowSize(winSize), numChannels(channels), window(winSize, dsp::WindowingFunction<float>::hann)
//olaBuffer(winSize * 4, 4, channels)
{
	for(int i = 0; i < numBuffers; i++)
	{
		auto* buff = new AudioBuffer<float>(numChannels, winSize);
		buff -> clear();
		popcornBuffers.add(buff);
	}
	
	bufferIndex = 0;
}

PopcornProcessor::~PopcornProcessor()
{
	for(AudioBuffer<float>* buff : popcornBuffers)
		delete buff;
}

void PopcornProcessor::processBlock(AudioBuffer<float>& buffer)
{
	int numChan = buffer.getNumChannels();
	
	for(int channel = 0; channel < buffer.getNumChannels(); channel++)
	{
		float rms = buffer.getRMSLevel(channel, 0, buffer.getNumSamples());
		
		
		runningAvgRMS.add(rms);
		
		if (runningAvgRMS.size() > 256)
			runningAvgRMS.remove(0);
		
		float avgRMS = calculateAvgRMS();
//		float std_RMS = calculateStdRMS();
//		stdAccum.addValue(std_RMS);

		if (avgRMS > rms)
		{
			stdAccum.reset();
//			int buffIdx = rand.nextInt(numBuffers);
			popcornBuffers[bufferIndex % numBuffers] -> clear();
			popcornBuffers[bufferIndex % numBuffers] -> addFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
			
			auto* buffData = popcornBuffers[bufferIndex % numBuffers] -> getWritePointer(channel);
			
			window.multiplyWithWindowingTable(buffData, windowSize);
			
			int buffIdx = abs(bufferIndex - delayFactor) % numBuffers;
			std::cout << "buff idx " + std::to_string(buffIdx) + "\n";
//			std::cout << popcornBuffers[buffIdx]->getNumChannels();
			buffer.addFrom(channel, 0, *popcornBuffers[buffIdx], channel, 0, popcornBuffers[buffIdx]->getNumSamples());
			
			bufferIndex++;
		}

	}

}

float PopcornProcessor::calculateAvgRMS()
{
//	float avg = 0.f;
	StatisticsAccumulator<float> statAccum;
	
	for(float value : runningAvgRMS)
		statAccum.addValue(value);
	
	return statAccum.getAverage();
}

float PopcornProcessor::calculateStdRMS()
{
	StatisticsAccumulator<float> statAccum;
	
	for (float rms : runningAvgRMS)
		statAccum.addValue(rms);
	
	return statAccum.getStandardDeviation();
}

