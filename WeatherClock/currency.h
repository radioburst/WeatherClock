/****************************************************************************
	clock.h Interface for the menu class
	Author Andreas Dorrer
	Last Change: 08.11.2018
*****************************************************************************/
#ifndef CURRENCY_H_INCLUDED
#define CURRENCY_H_INCLUDED

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>

// Global Variables
extern float fBpi;
extern float fBpiHistory[20];

extern uint8_t uiUpdateHour, uiUpdateMinute;

// Prototypes
void drawCurrencyDisplay();

#endif //CURRENCY_H_INCLUDED
