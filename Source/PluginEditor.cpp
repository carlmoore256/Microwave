
#include "PluginProcessor.h"
#include "PluginEditor.h"

//================================================================

MicrowaveAudioProcessorEditor::MicrowaveAudioProcessorEditor (MicrowaveAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), microwaveController(p)
{
    setSize (1000, 543);
	addAndMakeVisible(microwaveController);
	
	// set up digit buttons
	int x_pos = 864;
	int y_pos = 289;
	
	for (int i = 0; i < 12; i++)
	{
		if (i % 3 == 0 && i != 0)
			y_pos += 19;
		TextButton* button = new TextButton();
		button -> setAlpha(0.0f);
		digitButtons[i] = button;
		addAndMakeVisible(digitButtons[i]);
		digitButtons[i] -> setBounds(x_pos + ((i % 3) * 30), y_pos, 30, 19);
		int button_id = i + 1;
		if (i == 10)
			button_id = 0;
		button -> onClick = [this, button_id, button] { digitButtonClick(button, button_id); };
	}
	
	addAndMakeVisible(buttonStart);
	buttonStart.setBounds(861, 385, 30, 30);
	buttonStart.setAlpha(0.0f);
	buttonStart.onClick = [this] { microwaveController.startCook(); };
	
	addAndMakeVisible(buttonCancel);
	buttonCancel.setBounds(923, 385, 30, 30);
	buttonCancel.setAlpha(0.0f);
	buttonCancel.onClick = [this] { microwaveController.cancelOperation(); };
	
	addAndMakeVisible(buttonDoor);
	buttonDoor.setBounds(785, 84, 44, 390);
	buttonDoor.setAlpha(0.0f);
	buttonDoor.onClick = [this] { microwaveController.openCloseDoor(); };
}

MicrowaveAudioProcessorEditor::~MicrowaveAudioProcessorEditor()
{
	
	for(TextButton* button : digitButtons)
		delete button;
}

//===============================================================
void MicrowaveAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MicrowaveAudioProcessorEditor::resized()
{
	microwaveController.setBounds(getLocalBounds());
}

void MicrowaveAudioProcessorEditor::digitButtonClick(TextButton* button, int num)
{
	if(num <= 9)
		microwaveController.addDigit(num);
	if(num == 10)
		microwaveController.setTimeCountdown();
}

void MicrowaveAudioProcessorEditor::buttonClicked(Button* button)
{
}

//===============================================================
