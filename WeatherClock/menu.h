/****************************************************************************
	menu.h Interface for the menu class
	Author Andreas Dorrer
	Last Change: 08.11.2018
*****************************************************************************/
#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>

#include "ks0108lib/KS0108.h"

// Defines
#define SSID_MaxLenght 14
#define PWD_MaxLenght 14
#define CITYID_Lenght 8

// Enums

enum SUBMENU
{
	MAINMENU = 0,
	TIMEMENU = 1,
	DISPLAYMENU = 2,
	NETWORKMENU = 3,
	WEATHERMENU = 4,
	SETTIMEMENU = 5,
	SETTIMERMENU = 6,
	SETSSID = 7,
	SETPWD = 8
};

// Global Variables
extern void (*Rodarytick)(int);
extern void (*Rodarypush)();
extern int8_t gl_iMenuMode;
extern uint8_t gl_iBuzzerON, gl_iLightTimeOut;
extern uint8_t gl_iBeep;
extern volatile uint16_t iCountBeep;
extern char gl_cUSSID[SSID_MaxLenght], gl_cPASSWD[PWD_MaxLenght], gl_cCityID[CITYID_Lenght];

// Prototypes
void drawMainMenu();
void drawMenuPointer(int iIncrease);
 
 //Time menu
void drawTimeMenu();
void editTimeHour();
void editTimeMinute();
void editTimerHour();
void editTimerMinute();
void updateTimeHour(int iIncrease);
void updateTimeMinute(int iIncrease);
void setNewTime();
void setNewTimer();
void editTimerDays();
void updateTimerDays(int iIncrease);
void setNewTimerDays();
void drawTimerDays();
void editDateDay();
void editDateMonth();
void editDateYear();
void setNewDate();
void updateDateDay(int iIncrease);
void updateDateMonth(int iIncrease);
void updateDateYear(int iIncrease);

// Display menu
void drawDisplayMenu();
void editLightTimeOut();
void updateLightTimeOut(int iIncrease);
void setNewLightTimeOut();

// Network menu
void drawNetworkMenu();
void editSSID();
void editPassword();
void saveChar();
void updateChar(int iIncrease);

// Weather menu
void drawWeatherMenu();
void editCityID();
void saveDigit();
void updateDigit(int iIncrease);

void exitMenu();

void beepOn();
void beepOff();
void beep();
 



#endif
