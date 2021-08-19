//
//  DigitDisplay.hpp
//  Microwave
//
//  Created by Carl Moore on 8/16/21.
//  Copyright © 2021 ANVIL. All rights reserved.
//

#ifndef Microwave_h
#define Microwave_h

#include <stdio.h>
#include <JuceHeader.h>
#include "PluginProcessor.h"

#endif /* Microwave_h */


class Microwave : public Component, public Timer, private Time
{
public:
	Microwave(MicrowaveAudioProcessor&);
	~Microwave();
	
	void paint(Graphics&) override;
	
	void timerCallback() override;
	
	void startCook();
	
	void endCook();
	
	// set the 4 digits, -1 for off
	void setDigits(int, int, int, int);
	
	// button clicked to add new digit
	void addDigit(int);
	
	// begins counting down the input time as a timer
	void setTimeCountdown();
	
	// cancels any current microwave operation
	void cancelOperation();
	
	// immediately shows clock
	void showClock();
	
	// handles the graphics switch for opening and closing door
	void openCloseDoor();
	

private:
	MicrowaveAudioProcessor& processor;
	
	Array<Image> loadDigits();
	Array<Image> m_Digits;
	Array<Image> imagePanels;

	int digitLoc [2] = {872, 161};
	
	// contains current digits being displayed, -1=no digits
	int displayedDigits [4] = { -1, -1, -1, -1 };
	
	bool doorOpen = false;
	
	// 0 = display time
	// 1 = digit entry
	// 2 = digit countdown
	// 3 = cook
	int operationMode = 0;
	
	int timerValue;
	
	Image ui_InteriorOff;
	Image ui_InteriorOn;
	Image ui_DoorClosed;
	Image ui_DoorOpen;
	
	void setInterfaceImages(int);
	
	// draw a digit on the microwave display
	// position starting right to left
	void drawDigit(Graphics&, int, int);
	
	// handles the digit countdown
	void timerCountdown();

	void displayTime();
	
	// return total time entered in display
	int getCumulativeTime();
	
	// converts time entry to base 60
	void convertTimeEntry();
	
	// converts an int (cumulative seconds) to
	void setDisplaySecondsTime(int);
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Microwave);
};
