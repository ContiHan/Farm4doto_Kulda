/*
 * Grow.cpp
 *
 *  Created on: 13. 1. 2021
 *      Author: Daniel Hanak
 */

#include "Arduino.h"
#include "Grow.h"


/*
*
*	CROP SECTION STARTS HERE
*
*/

// constructor definition
Crop::Crop(byte pumpRelayPin, byte moistureMeterPin, int moistureMeterAir, int moistureMeterWater, byte moistureLimit, int wateringTime, int wateringHour)
{
	this->soilMoisturePercent = 255;
	this->pumpRelayPin = pumpRelayPin;
	this->moistureMeterPin = moistureMeterPin;
	this->moistureMeterAir = moistureMeterAir;
	this->moistureMeterWater = moistureMeterWater;
	this->moistureLimit = moistureLimit;
	this->wateringTime = wateringTime;
	this->wateringHour = wateringHour;

	for (byte i = 0; i < sizeof(wateringDays); i++)
	{
		wateringDays[i] = true;
	}

	instances[instanceCount++] = this;
}

// initializes crop instance
void Crop::Init()
{
	PumpRelayInit();
	lastCheck = currCheck = rtc.now();
}

// initializes instance pump relay pin
void Crop::PumpRelayInit()
{
	pinMode(pumpRelayPin, OUTPUT);

	// needs to be HIGH = Relay is NO (normally open)
	digitalWrite(pumpRelayPin, HIGH);
}

// sets watering days in week
void Crop::SetWateringDays(bool mon, bool tue, bool wed, bool thu, bool fri, bool sat, bool sun)
{
	wateringDays[0] = sun;
	wateringDays[1] = mon;
	wateringDays[2] = tue;
	wateringDays[3] = wed;
	wateringDays[4] = thu;
	wateringDays[5] = fri;
	wateringDays[6] = sat;
}

// gets sensor value
int Crop::GetMoistureMeterValue()
{
	// temp variable for sensor value
	int value;
	// turn soil meters voltage on
	digitalWrite(vccMoistureMetersPin, HIGH);
	// delay for corretly turn sensor on
	delay(100);
	// saves the value, because of return action and that the voltage needs to be turn first off
	value = analogRead(moistureMeterPin);
	// turn soil meters voltage off
	digitalWrite(vccMoistureMetersPin, LOW);
	// returns sensor value
	return value;
}

// gets sensor value in percent scale
byte Crop::GetMoistureMeterPercentValue()
{
	// recalculating value from sensor to percent scale
	return map(GetMoistureMeterValue(), moistureMeterAir, moistureMeterWater, 0, 100);
}


// watering method, just NC pump relay for defined time and than NO pump relay
void Crop::Watering()
{
	digitalWrite(pumpRelayPin, LOW);
	delay(wateringTime);
	digitalWrite(pumpRelayPin, HIGH);
}

// returns watering hour
byte Crop::GetWateringHour()
{
	return wateringHour;
}

// true if today is watering day
bool Crop::IsInWateringDay()
{
	currCheck = rtc.now();	
	return wateringDays[currCheck.dayOfTheWeek()];
}

// true if current hour is watering hour
bool Crop::IsInWateringHour()
{
	bool value = false;
	currCheck = rtc.now();
	if (currCheck.hour() == wateringHour)
	{
		value = true;
	}
	return value;
}

// true if current minute is watering minute
bool Crop::IsInWateringMinute()
{
	bool value = false;
	currCheck = rtc.now();
	byte minute;

	// saves current time minute to minute for compare and eventual icrease of value 60
	minute = currCheck.minute();

	// if current time hour is not equal with last check time hour, increase time_unit by 60 (minutes) to avoid overflow
	if (currCheck.hour() != lastCheck.hour())
	{
		minute += 60;
	}

	if (minute - lastCheck.minute() >= elapsedTime)
	{
		value = true;
	}

	lastCheck = rtc.now();
	
	return value;
}

// true if moisture level is low
bool Crop::MoistureLevelIsLow()
{
	bool value = false;
	if (GetMoistureMeterPercentValue() < moistureLimit)
	{
		value = true;
	}
	return value;
}

// sets elapsed time check value
void Crop::SetElapsedTimeCheck(byte minute)
{
	elapsedTime = minute;
}

// this method set vcc sensor pin and then call its initialization
void Crop::SetVccSoilMoistureMetersPin(byte pin)
{
	vccMoistureMetersPin = pin;
	MoistureMetersInit();
}

// return vcc sensor pin as value
byte Crop::GetVccSoilMoistureMetersPin()
{
	return vccMoistureMetersPin;
}

void Crop::InstancesWatering()
{
	for (size_t i = 0; i < instanceCount; i++)
	{
		instances[i]->Watering();
	}
	
}

// definition of class variables
byte Crop::vccMoistureMetersPin = 255;
byte Crop::elapsedTime = 255;
size_t Crop::instanceCount = 0;
Crop* Crop::instances[] = {0};


// vcc sensor pin initialization
void Crop::MoistureMetersInit()
{
	pinMode(vccMoistureMetersPin, OUTPUT);
	digitalWrite(vccMoistureMetersPin, LOW);
}


/*
*
*	BUTTON SECTION STARTS HERE
*
*/

// constructor, takes pin in argument
Button::Button(byte buttonPin)
{
	this->buttonPin = buttonPin;
	buttonLastState = LOW;
}

// initializes button instance
void Button::Init()
{
	pinMode(buttonPin, INPUT_PULLUP);
}

// returns true, if button is pressed
bool Button::ButtonIsPressed()
{
	// true, if button state change from LOW to HIGH
	return GetButtonChange(LOW, HIGH);
}

// returns true, if button is released
bool Button::ButtonIsReleased()
{
	// true if button state change from HIGH to LOW
	return GetButtonChange(HIGH, LOW);
}

// gets change of button state
bool Button::GetButtonChange(bool requiredLastState, bool requiredCurrState)
{
	bool value = false;
	// reads value from pin, that needs to be negate, because INPUT_PULLUP MODE returns LOW = pressed, HIGH = released in default
	buttonCurrState = !digitalRead(buttonPin);
	// checks if button state changes
	if (buttonLastState == requiredLastState && buttonCurrState == requiredCurrState)
	{
		value = true;
	}
	buttonLastState = buttonCurrState;

	return value;
}
