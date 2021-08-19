//
//  InterfaceSound.h
//  Microwave
//
//  Created by Carl Moore on 8/18/21.
//  Copyright © 2021 ANVIL. All rights reserved.
//

//#ifndef InterfaceSound_h
//#define InterfaceSound_h
#pragma once

#include <stdio.h>
#include <JuceHeader.h>

//#endif /* InterfaceSound_h */


class InterfaceSound : public juce::Component
{
public:
	InterfaceSound(std::string, int);
	
	~InterfaceSound();
	
	AudioBuffer<float>* buffer;
	
	void writeNextBuffer(AudioBuffer<float>& outBuffer);
	
	//	set sound to play when called
	void resetPlay();

private:
	bool bufferPlaying = false;
	int bufferNumSamples;
	int sampleIndex = 0;
	int bufferSize = 0;
	
};
