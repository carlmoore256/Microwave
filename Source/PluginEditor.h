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
	void operationButtonClick(TextButton*, int);

private:
	
    MicrowaveAudioProcessor& audioProcessor;

//	Image microwaveFrontPanel;
//	Image microwaveInternal;
	
	Microwave microwaveController;
	
	TextButton* digitButtons [12];
	
	TextButton buttonCancel;
	TextButton buttonStart;
	TextButton buttonDoor;
	
//	TextButton popcornButton;
//	TextButton potatoButton;
//	TextButton pizzaButton;
//	TextButton cookButton;
//	TextButton reheatButton;
//	TextButton softenMeltButton;
//	TextButton cookTimeButton;
//	TextButton cookPowerButton;
//	TextButton defrostButton;
//	TextButton* operationButtons[9] = { &popcornButton, &potatoButton, &pizzaButton, &cookButton, &reheatButton, &softenMeltButton, &cookTimeButton, &cookPowerButton, &defrostButton };
	
	TextButton* operationButtons [9];

	
//	bool m_DoorOpen = false;
	
//	void openCloseDoor();
	
	void startStopMicrowave();
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicrowaveAudioProcessorEditor)
};
