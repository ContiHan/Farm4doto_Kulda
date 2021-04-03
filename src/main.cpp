// TODO
// ====================================================================================================
// přidat dny u plodin a kontrolu i podle dne
// přejmenovat všechny metody tak, aby se v kódu četly jako věta
// RTC se přesune i do GROW knihovny, zkontrolovat, jestli je potřeba znovu initovat modul
// Zlepšit metody u Crop třídních metod, jestli nebude lepší dát to do jedné s více vstupními parametry
// Zkontrolovat, co musí být a co musí a nemusí být v private sekci


#include <Arduino.h>
#include "Grow.h"
#include "RTClib.h"
#include "HCSR04.h"

// define of analog pins
#define soil_moisture_meter_01 A0
#define soil_moisture_meter_02 A1

// define of digital pins
#define VCC_soil_mosture_meters 13
#define pump_relay_01 7
#define pump_relay_02 8
#define US_sensor_TRIG 9
#define US_sensor_ECHO 10
#define button_BACK 2
#define button_NEXT 3
#define button_OK 4

// define of other things
#define water_level_low 75
#define water_level_high 5
#define elapsed_time 5

// ARDUINO UNO PINOUT
// ------------------------
// ANALOG
// A0 --- Půdní vlhkoměr 01
// A1 --- Půdní vlhkoměr 02
// A2 ---
// A3 ---
// A4 SDA RTC modul, OLED
// A5 SCL RTC modul, OLED
// ------------------------
// DIGITAL
//  0 RX- Nelze použít, protože piny blokuje
//  1 TX- komunikace s PC a Serial.begin() Serial monitor
//  2 --- Tlačítko BACK
//  3 490 Tlačítko NEXT
//  4 --- Tlačítko OK
//  5 980 
//  6 980 
//  7 --- Relé čerpadlo 01
//  8 --- Relé čerpadlo 02
//  9 490 US sensor TRIG
// 10 490 US sensor ECHO
// 11 490 
// 12 --- 
// 13 LED VCC vlhkoměry

// declaration of local methods
void water_at_right_time(Crop crop);
void cultivate();
void print_time();
void print_date();
byte get_water_level_percent();
void print_with_delay(unsigned long);

// variables for crops
Crop tomatoes(pump_relay_01, soil_moisture_meter_01, 600, 240, 70, 3000, 8);
Crop cucumbers(pump_relay_02, soil_moisture_meter_02, 600, 240, 60, 6000, 8);

// variables for US sensor
HCSR04 US_sensor(US_sensor_TRIG, US_sensor_ECHO);

// variables for RTC
RTC_DS1307 rtc;

// variables for time
DateTime date_time;
unsigned long delay_last_check;

// variables for buttons
Button back(button_BACK);
Button next(button_NEXT);
Button ok(button_OK);

// days and months arrays
char day_list[7][8] = {"nedele", "pondeli", "utery", "streda", "ctvrtek", "patek", "sobota"};
char month_list[12][4] = {"led", "uno", "bre", "dub", "kve", "cer", "cec", "srp", "zar", "rij", "lis", "pro"};

void setup()
{
	// init of delay variable for last check time
	delay_last_check = millis();

	// init of serial line (baud 9600)
	Serial.begin(9600);
	while(! Serial);

	// check of connected RTC modul
	if (! rtc.begin())
	{
		Serial.println("Hodiny nejsou pripojeny!");
	    while (1);
	}
	// check if RTC is running
	if (! rtc.isrunning())
	{
	    Serial.println("Hodiny nejsou spusteny! Spoustim nyni..");
	}

	// for set up RTC date and time use this syntax below in order
  	// YEAR, MONTH, DAY, HOUR, MINUTE, SECOND
  	// EXAMPLE: 16.1.2021 12:51:00
 	// rtc.adjust(DateTime(2021, 1, 16, 12, 51, 00));

	// init of vcc sensors pin and crops 
	Crop::set_vcc_soil_moisture_meters_pin(VCC_soil_mosture_meters);
	Crop::set_elapsed_time_check(elapsed_time);
	tomatoes.init();
	cucumbers.init();

	// init of buttons
	back.init();
	next.init();
	ok.init();

	// init od US sensor
	US_sensor.begin();
}

// checks if it is ideal hour for watering specific crop
void water_at_right_time(Crop crop)
{
	if (crop.is_in_watering_day() && crop.is_in_watering_hour() && crop.is_in_watering_minute() && crop.moisture_level_is_low())
	{
		crop.watering();
	}
}

// grow section executes here
void cultivate()
{
	water_at_right_time(tomatoes);
	water_at_right_time(cucumbers);
}

// print current time
void print_time()
{
	// set and print current time
	date_time = rtc.now();
	Serial.print((String)date_time.hour() + ":" + date_time.minute() + ":" + date_time.second());
}

// print current date
void print_date()
{
	// set and print current date
	date_time = rtc.now();
	Serial.print((String)day_list[date_time.dayOfTheWeek()] + " " + date_time.day() + "." + month_list[date_time.month()-1] + " " + date_time.year());
}

// returns percent value of water level
byte get_water_level_percent()
{
	return map(US_sensor.getDistance(), water_level_low, water_level_high, 0, 100);
};

// replaces classic delay, all Serial print calls put here, default delay is set to 1000ms
void print_with_delay(unsigned long delay_check = 1000)
{
	if (millis() - delay_last_check >= delay_check)
	{
		Serial.println("==============================");
		Serial.println((String)"Vlhkost rajčat: " + tomatoes.get_moisture_meter_percent_value() + "%");
		Serial.println((String)"Vlhkost okurek: " + cucumbers.get_moisture_meter_percent_value() + "%");
		Serial.println("==============================");
		Serial.println((String)"Zásoba vody je  : " + get_water_level_percent() + "%");
		Serial.println("==============================\n");

		delay_last_check = millis();
	}
}

void loop()
{
	if (back.button_is_pressed())
	{
		Serial.println("Zmáčnuté tlačítko BACK");
	}

	if (next.button_is_pressed())
	{
		Serial.println("Zmáčnuté tlačítko NEXT");
	}

	if (ok.button_is_pressed())
	{
		Serial.println("Zmáčnuté tlačítko OK");
	}

	print_with_delay(5000);
	cultivate();
}
