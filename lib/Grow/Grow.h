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

class Crop
{
	public:
		// constructor that needs 6 specific values
    	Crop(byte pump_pin, byte sensor_pin, int sensor_air, int sensor_water, byte moisture_limit, int watering_time, int watering_hour);

		// public instance variables

    	// public instance methods
    	void init();
		void set_watering_days(bool mon, bool tue, bool wed, bool thu, bool fri, bool sat, bool sun);
		int get_moisture_meter_value();
    	byte get_moisture_meter_percent_value();
    	void watering();
		byte get_watering_hour();
		bool is_in_watering_day();
		bool is_in_watering_hour();
		bool is_in_watering_minute();
		bool moisture_level_is_low();

    	// public class methods
		static void set_elapsed_time_check(byte minute);
    	static void set_vcc_soil_moisture_meters_pin(byte vcc_pin);
    	static byte get_vcc_soil_moisture_meters_pin();

	private:
    	// private instance variables
    	byte _soil_mosture_percent;
    	byte _pump_relay_pin;
    	byte _moisture_meter_pin;
    	int _moisture_meter_air;
    	int _moisture_meter_water;
    	byte _moisture_limit;
    	int _watering_time;
		byte _watering_hour;
		bool _watering_days[7];
		RTC_DS1307 _rtc;
		DateTime _last_check, _curr_check;

    	// private class variables
    	static byte _vcc_moisture_meters_pin;
		static byte _elapsed_time;

    	// private instance methods
    	void _pump_relay_init();

    	// private class methods
    	static void _moisture_meters_init();
};

class Button
{
	public:
		// constructor, that needs pin as argument
		Button(byte button_pin);

		// public instance variable

		// public instance methods
		void init();
		bool button_is_pressed();
		bool button_is_released();

	private:
		// private instance variables
		byte _button_pin;
		bool _button_last_state;
		bool _button_curr_state;

		// private instance mathods
		bool _get_button_change(bool required_last_state, bool requried_curr_state);
};

#endif /* Grow_h */
