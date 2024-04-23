/****************************************************************************
	clock.h Interface for the menu class
	Author Andreas Dorrer
	Last Change: 08.11.2018
*****************************************************************************/
#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>

#include "ks0108lib/KS0108.h"

// Defines

// eeprom addresses
#define EEPROM_TimerHour 1
#define EEPROM_TimerMinute 2
#define EEPROM_TimerDays 3
#define EEPROM_DisplayTimeOut 11
#define EEPROM_SSID 12
#define EEPROM_PWD 26
#define EEPROM_CityID 40

// Enums

// Global Variables

// Some containers for weather data


// Prototypes
void clockInit();
void drawClockDisplay(uint16_t weather_pressure, float weather_temp, uint8_t weather_humidity, float weather_speed, char *weather_icon, int8_t fTempInside, int8_t fHumInside, uint8_t *p_hour, uint8_t *p_minute);
void drawWeatherIcon(char *Icon_Name);
char *getdayName(uint8_t dayofWeek);

uint8_t checkTimer(uint8_t hour, uint8_t minute, uint8_t second, uint8_t dayofweek);

void setNewTimerValue(uint8_t hour, uint8_t minute);
void readTimerValue();

uint8_t getTimerValueHour();
uint8_t getTimerValueMinute();
uint8_t *getTimerDays();
void saveTimerDays();
void readTimerDays();

void setNewLightTimeOutValue(uint8_t iTimeOut);
void setNewSSID(char *cUSSID, uint8_t iLenght);
void setNewPWD(char *cPWD, uint8_t iLenght);

#endif
