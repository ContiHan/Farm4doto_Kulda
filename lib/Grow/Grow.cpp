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
Crop::Crop(byte pump_relay_pin,
		   byte moisture_meter_pin,
		   int moisture_meter_air,
		   int moisture_meter_water,
		   byte moisture_limit,
		   int watering_time,
		   int watering_hour)
{
	_soil_mosture_percent = 255;
	_pump_relay_pin = pump_relay_pin;
	_moisture_meter_pin = moisture_meter_pin;
	_moisture_meter_air = moisture_meter_air;
	_moisture_meter_water = moisture_meter_water;
	_moisture_limit = moisture_limit;
	_watering_time = watering_time;
	_watering_hour = watering_hour;

	for (byte i = 0; i < sizeof(_watering_days); i++)
	{
		_watering_days[i] = true;
	}
}

// initializes crop instance
void Crop::init()
{
	_pump_relay_init();
	_last_check = _curr_check = _rtc.now();
}

// initializes instance pump relay pin
void Crop::_pump_relay_init()
{
	pinMode(_pump_relay_pin, OUTPUT);

	// needs to be HIGH = Relay is NO (normally open)
	digitalWrite(_pump_relay_pin, HIGH);
}

// sets watering days in week
void Crop::set_watering_days(bool mon, bool tue, bool wed, bool thu, bool fri, bool sat, bool sun)
{
	_watering_days[0] = sun;
	_watering_days[1] = mon;
	_watering_days[2] = tue;
	_watering_days[3] = wed;
	_watering_days[4] = thu;
	_watering_days[5] = fri;
	_watering_days[6] = sat;
}

// gets sensor value
int Crop::get_moisture_meter_value()
{
	// temp variable for sensor value
	int value;
	// turn soil meters voltage on
	digitalWrite(_vcc_moisture_meters_pin, HIGH);
	// delay for corretly turn sensor on
	delay(100);
	// saves the value, because of return action and that the voltage needs to be turn first off
	value = analogRead(_moisture_meter_pin);
	// turn soil meters voltage off
	digitalWrite(_vcc_moisture_meters_pin, LOW);
	// returns sensor value
	return value;
}

// gets sensor value in percent scale
byte Crop::get_moisture_meter_percent_value()
{
	// recalculating value from sensor to percent scale
	return map(get_moisture_meter_value(), _moisture_meter_air, _moisture_meter_water, 0, 100);
}


// watering method, just NC pump relay for defined time and than NO pump relay
void Crop::watering()
{
	digitalWrite(_pump_relay_pin, LOW);
	delay(_watering_time);
	digitalWrite(_pump_relay_pin, HIGH);
}

// returns watering hour
byte Crop::get_watering_hour()
{
	return _watering_hour;
}

// true if today is watering day
bool Crop::is_in_watering_day()
{
	_curr_check = _rtc.now();	
	return _watering_days[_curr_check.dayOfTheWeek()];
}

// true if current hour is watering hour
bool Crop::is_in_watering_hour()
{
	bool value = false;
	_curr_check = _rtc.now();
	if (_curr_check.hour() == _watering_hour)
	{
		value = true;
	}
	return value;
}

// true if current minute is watering minute
bool Crop::is_in_watering_minute()
{
	bool value = false;
	_curr_check = _rtc.now();
	byte minute;

	// saves current time minute to minute for compare and eventual icrease of value 60
	minute = _curr_check.minute();

	// if current time hour is not equal with last check time hour, increase time_unit by 60 (minutes) to avoid overflow
	if (_curr_check.hour() != _last_check.hour())
	{
		minute += 60;
	}

	if (minute - _last_check.minute() >= _elapsed_time)
	{
		value = true;
	}

	_last_check = _rtc.now();
	
	return value;
}

// true if moisture level is low
bool Crop::moisture_level_is_low()
{
	bool value = false;
	if (get_moisture_meter_percent_value() < _moisture_limit)
	{
		value = true;
	}
	return value;
}

// sets elapsed time check value
void Crop::set_elapsed_time_check(byte minute)
{
	_elapsed_time = minute;
}

// this method set vcc sensor pin and then call its initialization
void Crop::set_vcc_soil_moisture_meters_pin(byte pin)
{
	_vcc_moisture_meters_pin = pin;
	_moisture_meters_init();
}

// return vcc sensor pin as value
byte Crop::get_vcc_soil_moisture_meters_pin()
{
	return _vcc_moisture_meters_pin;
}

// definition of class variables
byte Crop::_vcc_moisture_meters_pin = 255;
byte Crop::_elapsed_time = 255;

// vcc sensor pin initialization
void Crop::_moisture_meters_init()
{
	pinMode(_vcc_moisture_meters_pin, OUTPUT);
	digitalWrite(_vcc_moisture_meters_pin, LOW);
}


/*
*
*	BUTTON SECTION STARTS HERE
*
*/

// constructor, takes pin in argument
Button::Button(byte button_pin)
{
	_button_pin = button_pin;
	_button_last_state = LOW;
}

// initializes button instance
void Button::init()
{
	pinMode(_button_pin, INPUT_PULLUP);
}

// returns true, if button is pressed
bool Button::button_is_pressed()
{
	// true, if button state change from LOW to HIGH
	return _get_button_change(LOW, HIGH);
}

// returns true, if button is released
bool Button::button_is_released()
{
	// true if button state change from HIGH to LOW
	return _get_button_change(HIGH, LOW);
}

// gets change of button state
bool Button::_get_button_change(bool required_last_state, bool required_curr_state)
{
	bool value = false;
	// reads value from pin, that needs to be negate, because INPUT_PULLUP MODE returns LOW = pressed, HIGH = released in default
	_button_curr_state = !digitalRead(_button_pin);
	// checks if button state changes
	if (_button_last_state == required_last_state && _button_curr_state == required_curr_state)
	{
		value = true;
	}
	_button_last_state = _button_curr_state;

	return value;
}
