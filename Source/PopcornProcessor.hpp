//
//  PopcornProcessor.hpp
//  Microwave
//
//  Created by Carl Moore on 8/19/21.
//  Copyright © 2021 ANVIL. All rights reserved.
//

#ifndef PopcornProcessor_hpp
#define PopcornProcessor_hpp

#include <stdio.h>
#include <JuceHeader.h>
//#include "OlaBuffer.hpp"

#endif /* PopcornProcessor_hpp */


class PopcornProcessor : public Component
{
public:
	PopcornProcessor(int, int);
	~PopcornProcessor();
	
	void processBlock(AudioBuffer<float>&);

private:
	int windowSize;
	int numChannels;
	int numBuffers = 128;
	int delayFactor = 4;
	int bufferIndex;
	
	dsp::WindowingFunction<float> window;

	float calculateAvgRMS();
	float calculateStdRMS();
	float avgRMS = 0;
	float avgSTD = 0;
	
	StatisticsAccumulator<float> stdAccum;
	
	Array<float> runningAvgRMS;
	
	Random rand;
	
//	OlaBuffer olaBuffer;

	Array<AudioBuffer<float>*> popcornBuffers;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopcornProcessor)
};
