

#pragma once

#include <JuceHeader.h>
#include "InterfaceSound.h"

//==============================================================================
/**
*/
class MicrowaveAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MicrowaveAudioProcessor();
    ~MicrowaveAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
	
	InterfaceSound* beepShort;
	InterfaceSound* beepError;
	InterfaceSound* beepLong;
	InterfaceSound* beepFinished;

	InterfaceSound* openDoor;
	InterfaceSound* closeDoor;

private:
	
	void processInterfaceSounds (AudioBuffer<float>&);
	AudioBuffer<float> beepBuffer;
	
	Array<InterfaceSound*> interfaceSounds;
	
	int m_filePos = 0;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicrowaveAudioProcessor)
};
