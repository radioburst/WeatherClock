/****************************************************************************
	clock.h Interface for the menu class
	Author Andreas Dorrer
	Last Change: 08.11.2018
*****************************************************************************/
#ifndef FORECAST_H_INCLUDED
#define FORECAST_H_INCLUDED

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>

// Defines

// eeprom addresses

// Enums

// Global Variables

struct ForecastData
{
	char icon[5];
	char timeStamp[25];
	float fTempMin;
	float fTempMax;
	float fSpeed;
};

extern struct ForecastData forecast_data[3];

// Prototypes

void drawForecastDisplay();

#endif //FORECAST_H_INCLUDED
