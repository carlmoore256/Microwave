//
//  DigitDisplay.hpp
//  Microwave
//
//  Created by Carl Moore on 8/16/21.
//  Copyright © 2021 ANVIL. All rights reserved.
//

#ifndef DigitDisplay_h
#define DigitDisplay_h

#include <stdio.h>
#include <JuceHeader.h>
#include "PluginProcessor.h"

#endif /* DigitDisplay_h */


class DigitDisplay : public Component, public Timer, private Time
{
public:
	DigitDisplay(MicrowaveAudioProcessor&);
	~DigitDisplay();
	
	void paint(Graphics&) override;
	
	void timerCallback() override;
	
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

private:
	MicrowaveAudioProcessor& processor;
	
	Array<Image> loadDigits();
	Array<Image> m_Digits;
	int digitLoc [2] = {872, 161};
	

	// contains current digits being displayed, -1=no digits
	int displayedDigits [4] = { -1, -1, -1, -1 };
	
	// 0 = display time
	// 1 = digit entry
	// 2 = digit countdown
	int displayMode = 0;
	
	int timerValue;
	
	// draw a digit on the microwave display
	// position starting right to left
	void drawDigit(Graphics&, int, int);
	
	// handles the digit countdown
	void countdownRoutine();

	void displayTime();
	
	// return total time entered in display
	int getCumulativeTime();
	
	// converts time entry to base 60
	void convertTimeEntry();
	
	// converts an int (cumulative seconds) to
	void setDisplaySecondsTime(int);
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DigitDisplay);
};
