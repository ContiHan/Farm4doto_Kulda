/*
 * Button.h
 *
 *  Created on: 13.11.2022
 *      Author: Daniel Hanak
 */

#ifndef Button_h
#define Button_h
#include "Arduino.h"
#include "RTClib.h"

class Button
{
public:
    // constructor, need pin as argument
    Button(byte buttonPin);

    // public instance variables

    // public instance methods
    void Init();
    bool ButtonIsPressed();
    bool ButtonIsReleased();

    // public class variables

    // public class methods

private:
    // private instance variables
    byte buttonPin;
    bool buttonLastState;
    bool buttonCurrState;

    // private instance methods
    bool GetButtonChange(bool requiredLastState, bool requiredCurrState);

    // private class variables

    // private class methods
};

// constructor, take pin in argument
Button::Button(byte buttonPin)
{
	this->buttonPin = buttonPin;
	buttonLastState = LOW;
}

// initialize button instance
void Button::Init()
{
	pinMode(buttonPin, INPUT_PULLUP);
}

// return true, if button is pressed
bool Button::ButtonIsPressed()
{
	// true, if button state change from LOW to HIGH
	return GetButtonChange(false, true);
}

// returns true, if button is released
bool Button::ButtonIsReleased()
{
	// true if button state change from HIGH to LOW
	return GetButtonChange(true, false);
}

// get change of button state
bool Button::GetButtonChange(bool requiredLastState, bool requiredCurrState)
{
	bool value = false;
	// read value from pin, that need to be negate, because INPUT_PULLUP MODE returns LOW = pressed, HIGH = released in default
	buttonCurrState = !digitalRead(buttonPin);
	// check if button state changes
	if (buttonLastState == requiredLastState && buttonCurrState == requiredCurrState)
	{
		value = true;
	}
	buttonLastState = buttonCurrState;

	return value;
}

#endif /* Button_h */
