#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Microwave.h"

//================================================================
/**
*/
class MicrowaveAudioProcessorEditor  : public juce::AudioProcessorEditor,
	public Button::Listener
{
public:
    MicrowaveAudioProcessorEditor (MicrowaveAudioProcessor&);
    ~MicrowaveAudioProcessorEditor() override;

	void paint (juce::Graphics&) override;
    void resized() override;
	
	void buttonClicked(Button* button) override;
	void digitButtonClick(TextButton*, int);

private:
	
    MicrowaveAudioProcessor& audioProcessor;

	Image microwaveFrontPanel;
	Image microwaveInternal;
	
	Microwave digitDisplay;
	
	TextButton* digitButtons [12];
	
	TextButton buttonCancel;
	TextButton buttonStart;
	TextButton buttonDoor;
	
	bool m_DoorOpen = false;
	
	// handles the graphics switch for opening and closing door
	void openCloseDoor();
	
	void startStopMicrowave();
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicrowaveAudioProcessorEditor)
};
