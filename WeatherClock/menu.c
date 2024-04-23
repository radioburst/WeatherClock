/****************************************************************************
	menu.h Definition for the menu class
	Author Andreas Dorrer
	Last Change: 08.11.2018
*****************************************************************************/

#include "menu.h"
#include "icons/weatherIcons.h"
#include "icons/menuIcons.h"

#include "ks0108lib/Font5x8.h"

// ext. Global Function pointers
void (*Rodarytick)(int) = NULL;
void (*Rodarypush)() = NULL;

// ext Global Variables
int8_t gl_iMenuMode = 1;
uint8_t gl_iSubMenu = 0,  gl_iBuzzerON = 0, gl_iLightTimeOut = 5;
uint8_t gl_iBeep = 0;
volatile uint16_t iCountBeep = 0;
char gl_cUSSID[SSID_MaxLenght], gl_cPASSWD[PWD_MaxLenght], gl_cCityID[CITYID_Lenght];

// Global Variables
uint8_t gl_iCurrLine = 0, gl_iMax, gl_iMin, gl_iCharPos = 0, gl_iYOffset = 0;
char gl_cCurrChar = 32; 
int8_t iCurrHour = 0, iCurrMinute = 0, iCurrDay = 0, iCurrMonth = 0, iCurrYear = 0;

void (*mainMenuFunctions[5])() = {drawTimeMenu, drawDisplayMenu, drawNetworkMenu, drawWeatherMenu, exitMenu};
void (*timeMenuFunctions[5])() = {editTimeHour, editDateDay, editTimerHour, editTimerDays, drawMainMenu};
void (*displayMenuFunctions[2])() = {editLightTimeOut, drawMainMenu};
void (*networkMenuFunctions[3])() = {editSSID, editPassword, drawMainMenu};
void (*weatherMenuFunctions[2])() = {editCityID, drawMainMenu};
	
const char cTimeMenu[] PROGMEM = {"Time"};
const char cDisplayMenu[] PROGMEM = {"Display"};
const char cNetworkMenu[] PROGMEM = {"Network"};
const char cWeatherMenu[] PROGMEM = {"Weather"};
const char cExitMenu[] PROGMEM = {"Exit"};		
const char cTimerDays[] PROGMEM = {"Day: s m d m d f s"};
const char cPointer[] PROGMEM = {"->"};	
	
// functions
void drawMenuPointer(int iIncrease)
{
	if((gl_iCurrLine + iIncrease) > gl_iMax)
	{
		beep();
		iIncrease = gl_iMax - gl_iCurrLine;
	}
	else if((gl_iCurrLine + iIncrease) < gl_iMin)
	{
		beep();
		iIncrease = gl_iMin - gl_iCurrLine;
	}
	
	GLCD_SetFont(Font5x8, 5, 8, GLCD_Overwrite);
	GLCD_GotoLine(gl_iCurrLine);
	GLCD_GotoX(4);
	GLCD_PrintString("  ");
	
	gl_iCurrLine += iIncrease;
	
	GLCD_GotoLine(gl_iCurrLine);
	GLCD_GotoX(4);
	GLCD_PrintString_P(cPointer);
	
	if(gl_iSubMenu == MAINMENU)
	{	
		Rodarypush = mainMenuFunctions[gl_iCurrLine - 3];	
		GLCD_GotoX(80);
		GLCD_GotoLine(3);
		if(gl_iCurrLine == 3)
			GLCD_DrawBitmap(ic_time_32x32, 32, 32, GLCD_Overwrite);
		else if(gl_iCurrLine == 4)
			GLCD_DrawBitmap(ic_display_32x32, 32, 32, GLCD_Overwrite);
		else if(gl_iCurrLine == 5)	
			GLCD_DrawBitmap(ic_wifi_32x32, 32, 32, GLCD_Overwrite);
		else if(gl_iCurrLine == 6)
			GLCD_DrawBitmap(ic_weather_32x32, 32, 32, GLCD_Overwrite);
		else if(gl_iCurrLine == 7)
			GLCD_DrawBitmap(ic_exit_32x32, 32, 32, GLCD_Overwrite);
	}
	else if(gl_iSubMenu == TIMEMENU)
		Rodarypush = timeMenuFunctions[gl_iCurrLine - 3];
	else if(gl_iSubMenu == DISPLAYMENU)
		Rodarypush = displayMenuFunctions[gl_iCurrLine - 3];
	else if(gl_iSubMenu == NETWORKMENU)
		Rodarypush = networkMenuFunctions[gl_iCurrLine - 3];
	else if(gl_iSubMenu == WEATHERMENU)
		Rodarypush = weatherMenuFunctions[gl_iCurrLine - 3];	
		
	GLCD_Render();
}

void drawMainMenu()
{
	GLCD_Clear();
	GLCD_GotoLine(1);
	GLCD_GotoX(2);
	GLCD_SetFont(Font5x8, 5, 8, GLCD_Overwrite);
	GLCD_PrintString("Settings");
	
	gl_iMax = 7;
	gl_iMin = gl_iCurrLine = 3;
	gl_iSubMenu = MAINMENU;
	GLCD_GotoLine(3);
	GLCD_GotoX(17);
	GLCD_PrintString_P(cTimeMenu);
	GLCD_GotoLine(4);
	GLCD_GotoX(17);
	GLCD_PrintString_P(cDisplayMenu);
	GLCD_GotoLine(5);
	GLCD_GotoX(17);
	GLCD_PrintString_P(cNetworkMenu);
	GLCD_GotoLine(6);
	GLCD_GotoX(17);
	GLCD_PrintString_P(cWeatherMenu);
	GLCD_Render();
	GLCD_GotoLine(7);
	GLCD_GotoX(17);
	GLCD_PrintString_P(cExitMenu);
	GLCD_Render();
	
	drawMenuPointer(0);
	Rodarytick = &drawMenuPointer;
}

void drawTimeMenu()
{
	char cTemp[20];
	
	uint8_t iSecond = 0, iDayOfWeek = 0, iTimerHour, iTimerMinute;
	
	GLCD_Clear();
	GLCD_GotoLine(1);
	GLCD_GotoX(2);
	GLCD_SetFont(Font5x8, 5, 8, GLCD_Overwrite);
	GLCD_PrintString_P(cTimeMenu);
	
	gl_iMax = 7;
	gl_iMin = gl_iCurrLine = 3;
	gl_iSubMenu = TIMEMENU;
	GLCD_GotoLine(3);
	GLCD_GotoX(17);
	ds1307_getdate(&iCurrYear, &iCurrMonth, &iCurrDay, &iCurrHour, &iCurrMinute, &iSecond, &iDayOfWeek);
	cTemp[19] = 0;
	sprintf(cTemp, "Time:  %02d : %02d", iCurrHour, iCurrMinute);	
	GLCD_PrintString(cTemp);
	GLCD_GotoLine(4);
	GLCD_GotoX(17);
	cTemp[19] = 0;
	sprintf(cTemp, "Date:  %02d.%02d.20%d", iCurrDay, iCurrMonth, iCurrYear);
	GLCD_PrintString(cTemp);
	GLCD_GotoLine(5);
	GLCD_GotoX(17);
	iTimerHour = getTimerValueHour();
	iTimerMinute = getTimerValueMinute();
	cTemp[19] = 0;
	sprintf(cTemp, "Timer: %02d : %02d", iTimerHour, iTimerMinute);	
	GLCD_PrintString(cTemp);
	GLCD_GotoLine(6);
	GLCD_GotoX(17);	
	GLCD_PrintString_P(cTimerDays);
	GLCD_GotoLine(7);
	GLCD_GotoX(17);
	GLCD_PrintString("Back");
	
	drawTimerDays();
	
	GLCD_Render();	
	drawMenuPointer(0);
	Rodarytick = &drawMenuPointer;
}

void editTimeHour()
{
	Rodarypush = &editTimeMinute;
	Rodarytick = &updateTimeHour;
	gl_iSubMenu = SETTIMEMENU;
	
	GLCD_DrawRectangle(57, 22, 71, 32, GLCD_Black);
	GLCD_Render();
}

void editTimeMinute()
{
	Rodarypush = &setNewTime;
	Rodarytick = &updateTimeMinute;
	
	GLCD_DrawRectangle(57, 22, 71, 32, GLCD_White);
	GLCD_DrawRectangle(87, 22, 101, 32, GLCD_Black);
	GLCD_Render();
}

void editTimerHour()
{
	Rodarypush = &editTimerMinute;
	Rodarytick = &updateTimeHour;
	gl_iSubMenu = SETTIMERMENU;
	iCurrHour = getTimerValueHour();
	iCurrMinute = getTimerValueMinute();
	
	GLCD_DrawRectangle(57, 38, 71, 48, GLCD_Black);
	GLCD_Render();
}

void editTimerMinute()
{
	Rodarypush = &setNewTimer;
	Rodarytick = &updateTimeMinute;
	
	GLCD_DrawRectangle(57, 38, 71, 48, GLCD_White);
	GLCD_DrawRectangle(87, 38, 101, 48, GLCD_Black);
	GLCD_Render();
}

void updateTimeHour(int iIncrease)
{
	char cTemp[5];
	
	if(gl_iSubMenu == SETTIMEMENU)
		GLCD_GotoLine(3);
	else
		GLCD_GotoLine(5);
	GLCD_GotoX(59);
	
	iCurrHour += iIncrease;
	if(iCurrHour > 24)
		iCurrHour = 0;
	else if(iCurrHour < 0)
		iCurrHour = 24;
	
	cTemp[4] = 0;
	sprintf(cTemp, "%02d", iCurrHour);
	GLCD_PrintString(cTemp);
	GLCD_Render();
}

void updateTimeMinute(int iIncrease)
{
	char cTemp[5];
	
	if(gl_iSubMenu == SETTIMEMENU)
		GLCD_GotoLine(3);
	else
		GLCD_GotoLine(5);
	GLCD_GotoX(89);
	
	iCurrMinute += iIncrease;
	if(iCurrMinute > 59)
		iCurrMinute = 0;
	else if(iCurrMinute < 0)
		iCurrMinute = 59;
		
	cTemp[4] = 0;
	sprintf(cTemp, "%02d", iCurrMinute);
	GLCD_PrintString(cTemp);
	GLCD_Render();
}

void editTimerDays()
{
	iCurrDay = 0;
	Rodarypush = &setNewTimerDays;
	Rodarytick = &updateTimerDays;	
	updateTimerDays(0);
}

void updateTimerDays(int iIncrease)
{
	uint8_t *timerdays = getTimerDays();
	
	iCurrDay += iIncrease;
	if(iCurrDay > 6)
		iCurrDay = 0;
	if(iCurrDay < 0)
		iCurrDay = 6;
	
	GLCD_ClearLine(6);
	
	GLCD_GotoLine(6);
	GLCD_GotoX(17);
	GLCD_PrintString("Day: s m d m d f s");
	drawTimerDays();
			
	if(*(timerdays+iCurrDay) <= 0)
		GLCD_DrawRectangle(45 + 12 * iCurrDay, 48, 53 + 12 * iCurrDay, 55, GLCD_White);
	
	GLCD_FillRectangle(45 + 12 * iCurrDay, 48, 53 + 12 * iCurrDay, 55, GLCD_Black);
		
	GLCD_Render();
}

void setNewTimerDays()
{
	uint8_t *timerdays = getTimerDays();
	
	if(*(timerdays+iCurrDay) <= 0)
		*(timerdays+iCurrDay) = 1;
	else
		*(timerdays+iCurrDay) = 0;
		
	saveTimerDays();
	drawTimeMenu();
}

void drawTimerDays()
{
	uint8_t *timerdays = getTimerDays();
	
	for(uint8_t i = 0; i < 7; i++)
	{
		if(*(timerdays+i) >= 1)
			GLCD_DrawRectangle(45 + 12 * i, 48, 53 + 12 * i, 55, GLCD_Black);
	}
}

void setNewTime()
{
	ds1307_setdate(iCurrYear, iCurrMonth, iCurrDay, iCurrHour, iCurrMinute, 0);
	drawTimeMenu();
}

void setNewTimer()
{
	setNewTimerValue(iCurrHour, iCurrMinute);
	drawTimeMenu();
}

void editDateDay()
{
	Rodarypush = &editDateMonth;
	Rodarytick = &updateDateDay;
	
	GLCD_DrawRectangle(57, 30, 71, 40, GLCD_Black);
	GLCD_Render();	
}

void editDateMonth()
{
	Rodarypush = &editDateYear;
	Rodarytick = &updateDateMonth;
	
	GLCD_DrawRectangle(57, 30, 71, 40, GLCD_White);
	GLCD_DrawRectangle(76, 30, 89, 40, GLCD_Black);
	GLCD_Render();
}

void editDateYear()
{
	Rodarypush = &setNewDate;
	Rodarytick = &updateDateYear;
	
	GLCD_DrawRectangle(76, 30, 89, 40, GLCD_White);
	GLCD_DrawRectangle(93, 30, 119, 40, GLCD_Black);
	GLCD_Render();
}

void setNewDate()
{
	uint8_t iYear, iMonth, iDay, iDayOfWeek, iSecond;
	
	ds1307_getdate(&iYear, &iMonth, &iDay, &iCurrHour, &iCurrMinute, &iSecond, &iDayOfWeek);
	
	ds1307_setdate(iCurrYear, iCurrMonth, iCurrDay, iCurrHour, iCurrMinute, iSecond);
	drawTimeMenu();
}

void updateDateDay(int iIncrease)
{
	char cTemp[5]; 
	iCurrDay += iIncrease;
	
	if(iCurrDay > 31)
		iCurrDay = 1;
	if(iCurrDay < 1)
		iCurrDay = 31;
	
	GLCD_GotoLine(4);
	GLCD_GotoX(59);
	cTemp[4] = 0;
	sprintf(cTemp, "%02d", iCurrDay);
	GLCD_PrintString(cTemp);
	GLCD_Render();			
}

void updateDateMonth(int iIncrease)
{
	char cTemp[5];
	iCurrMonth += iIncrease;
	
	if(iCurrMonth > 12)
		iCurrMonth = 1;
	if(iCurrMonth < 1)
		iCurrMonth = 12;
	
	GLCD_GotoLine(4);
	GLCD_GotoX(77);
	cTemp[4] = 0;
	sprintf(cTemp, "%02d", iCurrMonth);
	GLCD_PrintString(cTemp);
	GLCD_Render();
}

void updateDateYear(int iIncrease)
{
	char cTemp[5];
	iCurrYear += iIncrease;
	
	GLCD_GotoLine(4);
	GLCD_GotoX(95);
	cTemp[4] = 0;
	sprintf(cTemp, "20%d", iCurrYear);
	GLCD_PrintString(cTemp);
	GLCD_Render();
}

void drawDisplayMenu()
{
	char cTemp[20];
	
	GLCD_Clear();
	GLCD_GotoLine(1);
	GLCD_GotoX(2);
	GLCD_SetFont(Font5x8, 5, 8, GLCD_Overwrite);
	GLCD_PrintString_P(cDisplayMenu);
	
	gl_iMax = 4;
	gl_iMin = gl_iCurrLine = 3;
	gl_iSubMenu = DISPLAYMENU;
	GLCD_GotoLine(3);
	GLCD_GotoX(17);
	cTemp[19] = 0;
	sprintf(cTemp, "Light timeout:%02d", gl_iLightTimeOut);
	GLCD_PrintString(cTemp);
	GLCD_GotoLine(4);
	GLCD_GotoX(17);
	GLCD_PrintString("Back");
	
	GLCD_Render();
	drawMenuPointer(0);
	Rodarytick = &drawMenuPointer;
		
}

void editLightTimeOut()
{
	Rodarypush = &setNewLightTimeOut;
	Rodarytick = &updateLightTimeOut;
	
	GLCD_DrawRectangle(99, 22, 113, 32, GLCD_Black);
	GLCD_Render();
}

void updateLightTimeOut(int iIncrease)
{
	char cTemp[5];
	
	GLCD_GotoLine(3);
	GLCD_GotoX(101);
	
	gl_iLightTimeOut += iIncrease;
	if(gl_iLightTimeOut > 60)
		gl_iLightTimeOut = 0;
	else if(gl_iLightTimeOut < 0)
		gl_iLightTimeOut = 60;
	
	cTemp[4] = 0;
	sprintf(cTemp, "%02d", gl_iLightTimeOut);
	GLCD_PrintString(cTemp);
	GLCD_Render();
}

void setNewLightTimeOut()
{
	setNewLightTimeOutValue(gl_iLightTimeOut);	
	drawDisplayMenu();
}

void drawNetworkMenu()
{
	char cTemp[40];
	
	GLCD_Clear();
	GLCD_GotoLine(1);
	GLCD_GotoX(2);
	GLCD_SetFont(Font5x8, 5, 8, GLCD_Overwrite);
	GLCD_PrintString_P(cNetworkMenu);
			
	gl_iMax = 5;
	gl_iMin = gl_iCurrLine = 3;
	gl_iSubMenu = NETWORKMENU;
	GLCD_GotoLine(3);
	GLCD_GotoX(17);
	sprintf(cTemp, "SSID:%s", gl_cUSSID);
	GLCD_PrintString(cTemp);
	GLCD_GotoLine(4);
	GLCD_GotoX(17);
	cTemp[19] = 0;
	sprintf(cTemp, "PWD: %s", gl_cPASSWD);
	GLCD_PrintString(cTemp);
	
	GLCD_GotoLine(5);
	GLCD_GotoX(17);
	GLCD_PrintString("Back");
		
	GLCD_Render();
	drawMenuPointer(0);
	Rodarytick = &drawMenuPointer;	
}

void editSSID()
{
	Rodarypush = &saveChar;
	Rodarytick = &updateChar;
	gl_iCharPos = 0;
	gl_iSubMenu = SETSSID;
	gl_iYOffset = 0;
	
	GLCD_DrawRectangle(45, 22, 53, 32, GLCD_Black);
	GLCD_Render();
	gl_cCurrChar = gl_cUSSID[0];
}

void editPassword()
{
	Rodarypush = &saveChar;
	Rodarytick = &updateChar;
	gl_iCharPos = 0;
	gl_iSubMenu = SETPWD;
	gl_iYOffset = 8;
	
	GLCD_DrawRectangle(45, 30, 53, 40, GLCD_Black);
	GLCD_Render();
	gl_cCurrChar = gl_cPASSWD[0];
}

void saveChar()
{
	char cTemp[25];
	
	if(gl_cCurrChar == 32) // save current input
	{
		if(gl_iSubMenu == SETSSID)
		{
			for(uint8_t i = gl_iCharPos; i < SSID_MaxLenght; i++)
				gl_cUSSID[i] = 0;
			setNewSSID(gl_cUSSID, SSID_MaxLenght);
		}
		else
		{		
			for(uint8_t i = gl_iCharPos; i < PWD_MaxLenght; i++)
				gl_cPASSWD[i] = 0;
			setNewPWD(gl_cPASSWD, PWD_MaxLenght);
		}
		drawNetworkMenu();
	}
	else
	{	
		GLCD_DrawRectangle(45 + (6 * gl_iCharPos), 22 + gl_iYOffset, 53 + (6 * gl_iCharPos), 32 + gl_iYOffset, GLCD_White);
		
		if(gl_iSubMenu == SETSSID)
		{			
			gl_cUSSID[gl_iCharPos] = gl_cCurrChar;
			GLCD_GotoLine(3);
			GLCD_GotoX(17);
			sprintf(cTemp, "SSID:%s", gl_cUSSID);
			GLCD_PrintString(cTemp);
			
			if(gl_iCharPos + 1 < SSID_MaxLenght)
			{
				if(gl_cUSSID[gl_iCharPos + 1] == 0)
					gl_cCurrChar = 32;
				else
					gl_cCurrChar = gl_cUSSID[gl_iCharPos + 1];
			}
		}
		else if(gl_iSubMenu == SETPWD)
		{
			gl_cPASSWD[gl_iCharPos] = gl_cCurrChar;
			GLCD_GotoLine(4);
			GLCD_GotoX(17);
			sprintf(cTemp, "PWD: %s", gl_cPASSWD);
			GLCD_PrintString(cTemp);
			
			if(gl_iCharPos + 1 < PWD_MaxLenght)
			{	
				if(gl_cPASSWD[gl_iCharPos + 1] == 0)
					gl_cCurrChar = 32;
				else
					gl_cCurrChar = gl_cPASSWD[gl_iCharPos + 1];
			}
		}
		
		gl_iCharPos++;
		GLCD_DrawRectangle(45 + (6 * gl_iCharPos), 22 + gl_iYOffset, 53 + (6 * gl_iCharPos), 32 + gl_iYOffset, GLCD_Black);
		GLCD_Render();
	}
	
	if((gl_iSubMenu == SETSSID && gl_iCharPos + 1 >= SSID_MaxLenght) || (gl_iSubMenu == SETPWD && gl_iCharPos + 1 >= PWD_MaxLenght))
	{
		drawNetworkMenu();
		if(gl_iSubMenu == SETSSID)
		{	
			gl_cUSSID[SSID_MaxLenght - 1] = 0;
			setNewSSID(gl_cUSSID, SSID_MaxLenght);
		}
		else
		{	
			gl_cPASSWD[PWD_MaxLenght - 1] = 0;
			setNewPWD(gl_cPASSWD, PWD_MaxLenght);
		}
	}
}

void updateChar(int iIncrease)
{
	char cTemp[2];
	gl_cCurrChar += iIncrease;
	
	if(gl_cCurrChar > 126)
		gl_cCurrChar = 32;
	else if(gl_cCurrChar < 32)
		gl_cCurrChar = 126;
			
	if(gl_iSubMenu == SETSSID)
		GLCD_GotoLine(3);
	else
		GLCD_GotoLine(4);
	GLCD_GotoX(47 + (6 * gl_iCharPos));
	sprintf(cTemp, "%c", gl_cCurrChar);
	cTemp[1] = 0;
	GLCD_PrintString(cTemp);
	GLCD_Render();
}

void drawWeatherMenu()
{
	char cTemp[25];
	
	GLCD_Clear();
	GLCD_GotoLine(1);
	GLCD_GotoX(2);
	GLCD_SetFont(Font5x8, 5, 8, GLCD_Overwrite);
	GLCD_PrintString_P(cWeatherMenu);
	
	gl_iMax = 4;
	gl_iMin = gl_iCurrLine = 3;
	gl_iSubMenu = WEATHERMENU;
	GLCD_GotoLine(3);
	GLCD_GotoX(17);
	sprintf(cTemp, "City-ID:%s", gl_cCityID);
	GLCD_PrintString(cTemp);
	GLCD_GotoLine(4);
	GLCD_GotoX(17);
	GLCD_PrintString("Back");
	
	GLCD_Render();
	drawMenuPointer(0);
	Rodarytick = &drawMenuPointer;
}

void editCityID()
{
	Rodarypush = &saveDigit;
	Rodarytick = &updateDigit;
	gl_iCharPos = 0;
	
	GLCD_DrawRectangle(63, 22, 71, 32, GLCD_Black);
	GLCD_Render();
	gl_cCurrChar = gl_cCityID[0];
}

void saveDigit()
{
	char cTemp[25];
	

	if(gl_cCurrChar == 32) // save current input
	{
		for(uint8_t i = gl_iCharPos; i < CITYID_Lenght; i++)
			gl_cCityID[i] = 0;
		setNewCityID(gl_cCityID, CITYID_Lenght);
		drawWeatherMenu();
	}
	else
	{
		GLCD_DrawRectangle(63 + (6 * gl_iCharPos), 22, 71 + (6 * gl_iCharPos), 32, GLCD_White);
	
		gl_cCityID[gl_iCharPos] = gl_cCurrChar;
		GLCD_GotoLine(3);
		GLCD_GotoX(17);
		sprintf(cTemp, "City-ID:%s", gl_cCityID);
		GLCD_PrintString(cTemp);
		
		if(gl_iCharPos + 1 < CITYID_Lenght)
		{
			if(gl_cCityID[gl_iCharPos + 1] == 0)
			gl_cCurrChar = 32;
			else
			gl_cCurrChar = gl_cCityID[gl_iCharPos + 1];
		}
	
		gl_iCharPos++;
		GLCD_DrawRectangle(63 + (6 * gl_iCharPos), 22, 71 + (6 * gl_iCharPos), 32, GLCD_Black);
		GLCD_Render();
	}
	
	if(gl_iCharPos + 1 >= CITYID_Lenght)
	{
		gl_cCityID[CITYID_Lenght - 1] = 0;
		setNewCityID(gl_cCityID, CITYID_Lenght);
		drawWeatherMenu();
	}	
}

void updateDigit(int iIncrease)
{
	char cTemp[2];
	gl_cCurrChar += iIncrease;
	
	if(gl_cCurrChar > 57)
		gl_cCurrChar = 48;
	else if(gl_cCurrChar < 48)
		gl_cCurrChar = 57;
	
	GLCD_GotoLine(3);
	GLCD_GotoX(65 + (6 * gl_iCharPos));
	sprintf(cTemp, "%c", gl_cCurrChar);
	cTemp[1] = 0;
	GLCD_PrintString(cTemp);
	GLCD_Render();
}

void exitMenu()
{
	gl_iMenuMode = 1;
	TIMSK0 &=~ 1<<OCIE0A;	// stop rodary timer
}

 void beepOn()
 {
	 PORTB |= (1<<PINB6);
	 TIMSK2 |= 1<<TOIE2;
	 gl_iBuzzerON = 1;
 }

 void beepOff()
 {
	 TIMSK2 &=~ 1<<TOIE2;
	 PORTB &=~ (1<<PINB6);
	 iCountBeep = 0;
     gl_iBuzzerON = 0;
	 gl_iBeep = 0;
 }
 
 void beep()
 {
	 beepOn();
	 _delay_ms(100);
	 beepOff();
 }
