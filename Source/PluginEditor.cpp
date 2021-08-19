
#include "PluginProcessor.h"
#include "PluginEditor.h"

//================================================================

MicrowaveAudioProcessorEditor::MicrowaveAudioProcessorEditor (MicrowaveAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), digitDisplay(p)
{
    setSize (1000, 543);
	microwaveFrontPanel = ImageCache::getFromMemory (BinaryData::Microwave_Closed_Dark_png, BinaryData::Microwave_Closed_Dark_pngSize);
	microwaveInternal = ImageCache::getFromMemory(BinaryData::Microwave_Open_png, BinaryData::Microwave_Open_pngSize);
	
	addAndMakeVisible(digitDisplay);
	
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
	buttonStart.onClick = [this] { digitDisplay.setTimeCountdown(); };
	
	addAndMakeVisible(buttonCancel);
	buttonCancel.setBounds(923, 385, 30, 30);
	buttonCancel.setAlpha(0.0f);
	buttonCancel.onClick = [this] { digitDisplay.cancelOperation(); };
	
	addAndMakeVisible(buttonDoor);
	buttonDoor.setBounds(785, 84, 44, 390);
	buttonDoor.setAlpha(0.0f);
	buttonDoor.onClick = [this] { openCloseDoor(); };
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


	g.drawImageAt (microwaveInternal, 0, 0);
	
	if (!m_DoorOpen)
		g.drawImageAt (microwaveFrontPanel, 0, 0);
//    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MicrowaveAudioProcessorEditor::resized()
{
	digitDisplay.setBounds(getLocalBounds());
}

void MicrowaveAudioProcessorEditor::digitButtonClick(TextButton* button, int num)
{
	if(num <= 9)
		digitDisplay.addDigit(num);
	if(num == 10)
		digitDisplay.setTimeCountdown();
}

void MicrowaveAudioProcessorEditor::buttonClicked(Button* button)
{
}

//===============================================================

void MicrowaveAudioProcessorEditor::openCloseDoor()
{
	if(m_DoorOpen)
	{
		audioProcessor.openDoor -> resetPlay();
	} else {
		audioProcessor.closeDoor -> resetPlay();
	}
	
	m_DoorOpen = !m_DoorOpen;
	repaint();
}

