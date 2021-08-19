//
//  DigitDisplay.cpp
//  Microwave
//
//  Created by Carl Moore on 8/16/21.
//  Copyright © 2021 ANVIL. All rights reserved.
//

#include "Microwave.h"

Microwave::Microwave(MicrowaveAudioProcessor& p) : processor(p)
{
	m_Digits = loadDigits();
	Timer::startTimerHz(1);
}

Microwave::~Microwave()
{
	Timer::stopTimer();
}

void Microwave::paint(Graphics& g)
{
	for(int i = 0; i < 4; i++)
	{
		if(displayedDigits[i] >= 0)
			drawDigit(g, displayedDigits[i], i);
	}
}

void Microwave::timerCallback()
{
	switch (operationMode) {
		case 0:
			displayTime();
			break;
		case 1:
			break;
		case 2:
			countdownRoutine();
			break;
		default:
			break;
	}
	repaint();
}

void Microwave::startCook()
{
	operationMode = 3;
}

void Microwave::endCook()
{
	
}


void Microwave::setDigits(int d0, int d1, int d2, int d3)
{
	displayedDigits[0] = d0;
	displayedDigits[1] = d1;
	displayedDigits[2] = d2;
	displayedDigits[3] = d3;
}

void Microwave::addDigit(int newDigit)
{
	switch (operationMode){
		case 0:
			operationMode = 1;
			setDigits(newDigit, -1, -1, -1);
			processor.beepShort -> resetPlay();
			break;
		case 1:
			// roll digits
			setDigits(newDigit, displayedDigits[0], displayedDigits[1], displayedDigits[2]);
			processor.beepShort -> resetPlay();
			break;
		case 2:
			processor.beepError -> resetPlay();
			break;
		default:
			processor.beepError -> resetPlay();
			break;
	}
	
	repaint();
}

void Microwave::drawDigit(Graphics& g, int digit, int position)
{
	jassert(digit < 10 && digit >= 0);
	g.drawImageAt(m_Digits[digit + (position * 10)], digitLoc[0], digitLoc[1]);
}


Array<Image> Microwave::loadDigits()
{
	Array<Image> digitImages;
	int d_place = 0;
	for(int i = 0; i < 40; i++)
	{
		std::string filename = "digit_" + std::to_string(d_place) + "_" + std::to_string(i % 10) + "_png";
		int imageSize;
		char* resource_name;
		resource_name = &filename[0];
		const char* imageData = BinaryData::getNamedResource(resource_name, imageSize);
		
		if (imageData == nullptr)
			std::cout << "image data is nullptr! \n";
		Image digit = ImageCache::getFromMemory(imageData, imageSize);
		digitImages.add(digit);
		
		if (i % 10 == 9)
			d_place++;
	}
	return digitImages;
}


void Microwave::countdownRoutine()
{
	if (timerValue > 0){
		setDisplaySecondsTime(timerValue);
		timerValue--;
	} else {
		setDigits(-1,-1,-1,-1);
		processor.beepLong -> resetPlay();
		operationMode = 0;
	}
}

void Microwave::displayTime()
{
	Time time(Time::getCurrentTime());
	int hrs = time.getHoursInAmPmFormat();
	int min = time.getMinutes();

	int hr0 = 0;
	int hr1 = 0;
	
	if (hrs > 9){
		hr0 = 1;
		hr1 = hrs - 10;
	} else {
		hr1 = hrs;
	}
	
	int min0 = int(min / 10) % 10;
	int min1 = min % 10;

	displayedDigits[0] = min1;
	displayedDigits[1] = min0;
	displayedDigits[2] = hr1;
	displayedDigits[3] = hr0;
}

void Microwave::setTimeCountdown()
{
	switch (operationMode) {
		case 0:
			processor.beepError -> resetPlay();
			break;
		case 1:
			timerValue = getCumulativeTime();
			operationMode = 2;
			processor.beepShort -> resetPlay();
			break;
		case 2:
			// timer cancel, show time
			operationMode = 0;
			processor.beepLong -> resetPlay();
			break;
		case 3:
			processor.beepError -> resetPlay();
			break;
		default:
			break;
	}
}

void Microwave::cancelOperation()
{
	switch (operationMode) {
		case 0:
			processor.beepShort -> resetPlay();
			break;
		case 1:
			processor.beepShort -> resetPlay();
			displayTime();
			repaint();
			break;
		case 2:
			processor.beepShort -> resetPlay();
			displayTime();
			repaint();
		case 3:
//			processor.
			break;

		default:
			break;
	}
	operationMode = 0;
}

int Microwave::getCumulativeTime()
{
	int fullTime = 0;
	
	if (displayedDigits[3] > 0)
		fullTime += displayedDigits[3] * 1000;
	if (displayedDigits[2] > 0)
		fullTime += displayedDigits[2] * 100;
	if (displayedDigits[1] > 0)
		fullTime += displayedDigits[1] * 10;
	if (displayedDigits[0] > 0)
		fullTime += displayedDigits[0];
	
	return fullTime;
}

void Microwave::convertTimeEntry()
{
	int fullTime = getCumulativeTime();
	std::cout << "full time " + std::to_string(fullTime) + "\n";
	
	Time converted(2021, 0, 0, 0, 0, fullTime);
	
	std::cout << "hours " + std::to_string(converted.getHours()) + " mins " + std::to_string(converted.getMinutes()) + " secs " + std::to_string(converted.getSeconds()) + "\n";
	
	setDigits(converted.getSeconds() % 10, converted.getSeconds() / 10 % 10, converted.getMinutes() % 10, converted.getMinutes() / 10 % 10);
}

void Microwave::setDisplaySecondsTime(int seconds)
{
	Time converted(2021, 0, 0, 0, 0, seconds);
	setDigits(converted.getSeconds() % 10, converted.getSeconds() / 10 % 10, converted.getMinutes() % 10, converted.getMinutes() / 10 % 10);
}


