/*
 * Grow.h
 *
 *  Created on: 13. 1. 2021
 *      Author: Daniel Hanak
 */

#ifndef Grow_h
#define Grow_h
#include "Arduino.h"
#include "RTClib.h"

byte const maxCropInstances = 10;

class Crop
{
	public:
		// constructor, need 6 specific arguments
    	Crop(byte pumpPin, byte sensorPin, int sensorAir, int sensorWater, byte moistureLimit, int wateringTime, int wateringHour);

		// public instance variables

    	// public instance methods
    	void Init();
		void SetWateringDays(bool mon, bool tue, bool wed, bool thu, bool fri, bool sat, bool sun);
		int  GetMoistureMeterValue();
    	byte GetMoistureMeterPercentValue();
    	void Watering();
		byte GetWateringHour();
		bool IsInWateringDay();
		bool IsInWateringHour();
		bool IsInWateringMinute();
		bool MoistureLevelIsLow();

		// public class variables

    	// public class methods
		static void SetElapsedTimeCheck(byte minute);
    	static void SetVccSoilMoistureMetersPin(byte vccPin);
    	static byte GetVccSoilMoistureMetersPin();
		static void InstancesWatering();

	private:
    	// private instance variables
    	byte soilMoisturePercent;
    	byte pumpRelayPin;
    	byte moistureMeterPin;
    	int  moistureMeterAir;
    	int  moistureMeterWater;
    	byte moistureLimit;
    	int  wateringTime;
		byte wateringHour;
		bool wateringDays[7];
		RTC_DS1307 rtc;
		DateTime lastCheck, currCheck;

		// private instance methods
    	void PumpRelayInit();

    	// private class variables
    	static byte vccMoistureMetersPin;
		static byte elapsedTime;
		static size_t instanceCount;
    	static Crop* instances[maxCropInstances];

    	// private class methods
    	static void MoistureMetersInit();
};

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

#endif /* Grow_h */
