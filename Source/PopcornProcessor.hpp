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
	dsp::WindowingFunction<float> window;

	float calculateAvgRMS();
	float avgRMS = 0;
	
	Array<float> runningAvgRMS;
	
	Random rand;

	Array<AudioBuffer<float>*> popcornBuffers;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopcornProcessor)
};
