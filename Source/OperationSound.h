/*
  ==============================================================================

    OperationSound.h
    Created: 19 Aug 2021 12:54:58pm
    Author:  Carl Moore

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// handles multi-sequence sounds with granular capabilities
class OperationSound : public Component
{
public:
	OperationSound(int, int);
	~OperationSound();
	
	void writeNextBuffer(AudioBuffer<float>&);
	
	// starts buffer playback routines
	void startPlay();
	// gracefully ends playback
	void stopPlay();
	
private:
	
	int m_windowSize;
	int m_hopSize;
	
	dsp::WindowingFunction<float> window;
	
	bool bufferPlaying = false;
	// startup sound needs to complete before granular play
	bool playingStartup = false;
	
	int sampleIndex = 0;
	int ringBuffWriteIdx = 0;
	int ringBuffReadIdx = 0;
	int ringBuffSamplesRead = 0;
	int ringBuffSizeStep;
	int ringBuffSize;
	
	AudioBuffer<float> ringBuffer;
	AudioBuffer<float> grainBuffer;
	
	// returns positions within buffer to play next
	Array<int> getGranularPlayheads(int, int, int, int);
	
	// write granular samples to ring buffer
	void writeGranularToRing(AudioBuffer<float>&, int, int);
	
	void writeRingToOutput(AudioBuffer<float>&, AudioBuffer<float>&);
	
	void writeGranularSamples(AudioBuffer<float>&, AudioBuffer<float>&, int, int);
	
	void loadBuffer(AudioBuffer<float>&, std::string);
	
	AudioBuffer<float> microwaveStart;
	AudioBuffer<float> microwaveContinue;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OperationSound)
};
