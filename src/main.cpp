// TODO
// přidat dny u plodin a kontrolu i podle dne
// přejmenovat všechny metody tak, aby se v kódu četly jako věta
// RTC se přesune i do GROW knihovny, zkontrolovat, jestli je potřeba znovu initovat modul
// Zlepšit metody u Crop třídních metod, jestli nebude lepší dát to do jedné s více vstupními parametry
// Zkontrolovat, co musí být a co musí a nemusí být v private sekci
// Přepsat všechno podle konvencí jazyka C
// Přidat gettry a settry


#include <Arduino.h>
#include "Grow.h"
#include "RTClib.h"
#include "HCSR04.h"

// define of analog pins
#define SOIL_MOISTURE_METER_01 A0
#define SOIL_MOISTURE_METER_02 A1

// define of digital pins
#define VCC_SOIL_MOISTURE_METERS 13
#define PUMP_RELAY_01 7
#define PUMP_RELAY_02 8
#define US_SENSOR_TRIG 9
#define US_SENSOR_ECHO 10
#define BUTTON_BACK 2
#define BUTTON_NEXT 3
#define BUTTON_OK 4

// define of other things
#define WATER_LEVEL_LOW 75
#define WATER_LEVEL_HIGH 5
#define ELAPSED_TIME 5

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
void WaterAtRightTime(Crop crop);
void cultivate();
void printTime();
void printDate();
byte getWaterLevelPercent();
void printWithDelay(unsigned long);

// variables for crops
Crop tomatoes(PUMP_RELAY_01, SOIL_MOISTURE_METER_01, 600, 240, 70, 3000, 8);
Crop cucumbers(PUMP_RELAY_02, SOIL_MOISTURE_METER_02, 600, 240, 60, 6000, 8);

// variables for US sensor
HCSR04 usSensor(US_SENSOR_TRIG, US_SENSOR_ECHO);

// variables for RTC
RTC_DS1307 rtc;

// variables for time
DateTime dateTime;
unsigned long delayLastCheck;

// variables for buttons
Button back(BUTTON_BACK);
Button next(BUTTON_NEXT);
Button ok(BUTTON_OK);

// days and months arrays
char dayList[7][8] = {"nedele", "pondeli", "utery", "streda", "ctvrtek", "patek", "sobota"};
char monthList[12][4] = {"led", "uno", "bre", "dub", "kve", "cer", "cec", "srp", "zar", "rij", "lis", "pro"};

void setup()
{
	// init of delay variable for last check time
	delayLastCheck = millis();

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
	Crop::set_vcc_soil_moisture_meters_pin(VCC_SOIL_MOISTURE_METERS);
	Crop::set_elapsed_time_check(ELAPSED_TIME);
	tomatoes.init();
	cucumbers.init();

	// init of buttons
	back.init();
	next.init();
	ok.init();

	// init od US sensor
	usSensor.begin();
}

// checks if it is ideal hour for watering specific crop
void WaterAtRightTime(Crop crop)
{
	if (crop.is_in_watering_day() && crop.is_in_watering_hour() && crop.is_in_watering_minute() && crop.moisture_level_is_low())
	{
		crop.watering();
	}
}

// grow section executes here
void cultivate()
{
	WaterAtRightTime(tomatoes);
	WaterAtRightTime(cucumbers);
}

// print current time
void printTime()
{
	// set and print current time
	dateTime = rtc.now();
	Serial.print((String)dateTime.hour() + ":" + dateTime.minute() + ":" + dateTime.second());
}

// print current date
void printDate()
{
	// set and print current date
	dateTime = rtc.now();
	Serial.print((String)dayList[dateTime.dayOfTheWeek()] + " " + dateTime.day() + "." + monthList[dateTime.month()-1] + " " + dateTime.year());
}

// returns percent value of water level
byte getWaterLevelPercent()
{
	return map(usSensor.getDistance(), WATER_LEVEL_LOW, WATER_LEVEL_HIGH, 0, 100);
};

// replaces classic delay, all Serial print calls put here, default delay is set to 1000ms
void printWithDelay(unsigned long delay_check = 1000)
{
	if (millis() - delayLastCheck >= delay_check)
	{
		Serial.println("==============================");
		Serial.println((String)"Vlhkost rajčat: " + tomatoes.get_moisture_meter_percent_value() + "%");
		Serial.println((String)"Vlhkost okurek: " + cucumbers.get_moisture_meter_percent_value() + "%");
		Serial.println("==============================");
		Serial.println((String)"Zásoba vody je  : " + getWaterLevelPercent() + "%");
		Serial.println("==============================\n");

		delayLastCheck = millis();
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

	printWithDelay(5000);
	cultivate();
}
