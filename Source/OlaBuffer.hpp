//
//  OlaBuffer.hpp
//  Microwave
//
//  Created by Carl Moore on 8/19/21.
//  Copyright © 2021 ANVIL. All rights reserved.
//

#ifndef OlaBuffer_hpp
#define OlaBuffer_hpp

#include <stdio.h>
#include <JuceHeader.h>

#endif /* OlaBuffer_hpp */

class OlaBuffer : public Component
{
public:
	OlaBuffer(int ringBuffSize, int overlap, int channels);
	~OlaBuffer();
	
	void reset();
	
	void fillRingBuf(AudioBuffer<float>& outBuffer);
	
	void fillOverlapBuffer();
	
	void windowBuffers(int);
	
	void outputOlaBlock(AudioBuffer<float>&);
	
	void processOlaBuffer(AudioBuffer<float>&, void*);
	
private:
	dsp::WindowingFunction<float> m_window;
	
//	Array<AudioBuffer<float>>
	Array<AudioBuffer<float>*> overlapBuffers;
	AudioBuffer<float> ringBuffer;
	
	int m_channels;
	int m_ringBuffSize;
	int m_ringBuffWriteIdx;
	int m_overlap;
	int m_hop;
	int m_overlapBuffTargetChannel;
	
	Array<bool> processFlags;

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OlaBuffer);

};
