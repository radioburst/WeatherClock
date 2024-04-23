/****************************************************************************
	clock.h Definition for the menu class
	Author Andreas Dorrer
	Last Change: 08.11.2018
*****************************************************************************/

#include "clock.h"
#include "icons/weatherIcons.h"
#include "icons/generallIcons.h"
#include <time.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include "ks0108lib/KS0108.h"
#include "ks0108lib/Font5x8.h"
#include "ks0108lib/fixednums8x16.h"
#include "ks0108lib/watchNrSmall.h"
#include "ks0108lib/DaysOfWeek12x12.h"

// Global Variables
char c_temp[20], c_temp1[10];
uint8_t iClock1Hours = 0, iClock1Min = 0; //iHoursLeftClock1 = 0, iMinLeftClock1 = 0, 
uint8_t iClock1Days[7];
struct tm t1, t2;

const char cMo[] PROGMEM = {"Mo"};
const char cDi[] PROGMEM = {"Di"};
const char cMi[] PROGMEM = {"Mi"};
const char cDo[] PROGMEM = {"Do"};
const char cFr[] PROGMEM = {"Fr"};
const char cSa[] PROGMEM = {"Sa"};
const char cSo[] PROGMEM = {"So"};
const char cEr[] PROGMEM = {"Er"};
const char cStartUp[] PROGMEM = {"Startup..."};

void clockInit()
{
	GLCD_Setup();
	
	GLCD_Clear();
	GLCD_SetFont(Font5x8, 5, 8, GLCD_Overwrite);

	GLCD_GotoLine(3);
	GLCD_GotoX(35);
	GLCD_PrintString_P(cStartUp);
	GLCD_Render();
	
	readTimerValue();
	readTimerDays();
}

void drawWeatherIcon(char *Icon_Name)
{
	if(strcmp(Icon_Name, "01d") == 0)					// clear sky
		GLCD_DrawBitmap(ic_01d_32x24, 32, 24, GLCD_Overwrite);
	else if(strcmp(Icon_Name, "02d") == 0)				// few clouds
		GLCD_DrawBitmap(ic_02d_32x24, 32, 24, GLCD_Overwrite);
	else if(strcmp(Icon_Name, "03d") == 0 || strcmp(Icon_Name, "04d") == 0  || strcmp(Icon_Name, "03n") == 0 || strcmp(Icon_Name, "04n") == 0) // scattered clouds
		GLCD_DrawBitmap(ic_03d_32x24, 32, 24, GLCD_Overwrite);
	else if(strcmp(Icon_Name, "09d") == 0 || strcmp(Icon_Name, "09n") == 0) // shower rain
		GLCD_DrawBitmap(ic_09d_32x24, 32, 24, GLCD_Overwrite);
	else if(strcmp(Icon_Name, "10d") == 0 || strcmp(Icon_Name, "10n") == 0) // rain
		GLCD_DrawBitmap(ic_10d_32x24, 32, 24, GLCD_Overwrite);
	else if(strcmp(Icon_Name, "11d") == 0 || strcmp(Icon_Name, "11n") == 0) // thunder storm
		GLCD_DrawBitmap(ic_11d_32x24, 32, 24, GLCD_Overwrite);
	else if(strcmp(Icon_Name, "13d") == 0 || strcmp(Icon_Name, "13n") == 0) // snow
		GLCD_DrawBitmap(ic_13d_32x24, 32, 24, GLCD_Overwrite);
	else if(strcmp(Icon_Name, "50d") == 0 || strcmp(Icon_Name, "50n") == 0) // mist
		GLCD_DrawBitmap(ic_50d_32x24, 32, 24, GLCD_Overwrite);
	else if(strcmp(Icon_Name, "01n") == 0)								// clear sky night
		GLCD_DrawBitmap(ic_01n_32x24, 32, 24, GLCD_Overwrite);
	else if(strcmp(Icon_Name, "02n") == 0)								// few clouds night
		GLCD_DrawBitmap(ic_02n_32x24, 32, 24, GLCD_Overwrite);
	else
		GLCD_PrintString(Icon_Name);
}

char *getdayName(uint8_t dayofWeek)
{
	if(dayofWeek == 1)
		return cMo;
	else if(dayofWeek == 2)
		return cDi;
	else if(dayofWeek == 3)
		return cMi;
	else if(dayofWeek == 4)
		return cDo;
	else if(dayofWeek == 5)
		return cFr;
	else if(dayofWeek == 6)
		return cSa;
	else if(dayofWeek == 0)
		return cSo;
	return cEr;
}

void drawClockDisplay(uint16_t weather_pressure, float weather_temp, uint8_t weather_humidity, float weather_speed, char *weather_icon, int8_t iTempInside, int8_t iHumInside, uint8_t *p_hour, uint8_t *p_minute)
{			
	uint8_t year, month, day, hour, minute, second, dayofweek, iHoursLeftClock1, iMinLeftClock1;
	uint32_t diffsec = 0; 
	
	ds1307_getdate(&year, &month, &day, &hour, &minute, &second, &dayofweek);
	
	*p_hour = hour;
	*p_minute = minute;

	t1.tm_hour = hour;
	t1.tm_min = minute;
	t1.tm_sec = second;
	t1.tm_mday = day;
	t1.tm_mon = month;
	t1.tm_year = year;
	
	t2.tm_hour = iClock1Hours;
	t2.tm_min = iClock1Min;
	t2.tm_sec = 0;
	
	if(hour == iClock1Hours && minute >= iClock1Min)
		t2.tm_mday = day + 1;
	else if(hour <= iClock1Hours)
		t2.tm_mday = day;	
	else
		t2.tm_mday = day + 1;
	t2.tm_mon = month;
	t2.tm_year = year;
		
	diffsec = difftime(mktime(&t2), mktime(&t1));
	iHoursLeftClock1 = diffsec / 3600;
	iMinLeftClock1 = (diffsec % 3600) / 60;
			
	GLCD_Clear();
	GLCD_SetFont(System16x22, 16, 28, GLCD_Overwrite);
	GLCD_GotoXY(0, 2);
	sprintf(c_temp, "%02d:%02d", hour, minute);
	c_temp[19] = 0;
	GLCD_PrintString(c_temp);
	GLCD_SetFont(System8x12, 8, 12, GLCD_Overwrite);
	GLCD_GotoXY(72, 17);
	sprintf(c_temp, "%02d", second);
	c_temp[19] = 0;
	GLCD_PrintString(c_temp);
	GLCD_GotoXY(4, 32);
	sprintf(c_temp, "%02d/%02d/%d", day, month, year);
	c_temp[19] = 0;
	GLCD_PrintString(c_temp);
	GLCD_SetFont(DaysOfWeek12x12, 12, 12, GLCD_Overwrite);
	GLCD_GotoXY(66, 32);
	GLCD_PrintString_P(getdayName(dayofweek));
	for(uint8_t i = 1; i<89; i+=2)
		GLCD_SetPixel(i, 45, GLCD_Black);
	GLCD_SetFont(Font5x8, 5, 8, GLCD_Overwrite);
	GLCD_GotoLine(4);
	GLCD_GotoX(92);	
	sprintf(c_temp, "%s@C", ftoa(c_temp1, weather_temp-273.15, 1));
	c_temp[19] = 0;
	GLCD_PrintString(c_temp);
	GLCD_GotoLine(5);
	GLCD_GotoX(110);
	sprintf(c_temp, "%d%%", weather_humidity);
	c_temp[19] = 0;
	GLCD_PrintString(c_temp);
	GLCD_GotoLine(6);
	GLCD_GotoX(92);
	sprintf(c_temp, "%dhPa", weather_pressure);
	c_temp[19] = 0;
	GLCD_PrintString(c_temp);
	GLCD_GotoLine(7);
	GLCD_GotoX(92);
	sprintf(c_temp, "%skm", ftoa(c_temp1, weather_speed * 3.6, 1));
	c_temp[19] = 0;
	GLCD_PrintString(c_temp);
	for(uint8_t i = 1; i<63; i+=2)
		GLCD_SetPixel(89, i, GLCD_Black);
	GLCD_GotoXY(93, 2);
	drawWeatherIcon(weather_icon);
	GLCD_GotoLine(6);
	GLCD_GotoX(2);
	GLCD_DrawBitmap(ic_clock_8x8, 8, 8, GLCD_Overwrite);
	GLCD_GotoX(12);
	sprintf(c_temp, "%02d:%02d +%02d:%02d", iClock1Hours, iClock1Min, iHoursLeftClock1, iMinLeftClock1);
	c_temp[19] = 0;
	GLCD_PrintString(c_temp);
	GLCD_GotoLine(7);
	GLCD_GotoX(2);
	GLCD_DrawBitmap(ic_temp_8x8, 8, 8, GLCD_Overwrite);
	GLCD_GotoX(12);
	sprintf(c_temp, "%d@C", iTempInside);
	c_temp[19] = 0;
	GLCD_PrintString(c_temp);

	GLCD_GotoX(54);
	sprintf(c_temp, "%d%%", iHumInside);
	c_temp[19] = 0;
	GLCD_PrintString(c_temp);

	cli();
	GLCD_Render();
	sei();
	if(checkTimer(hour, minute, second, dayofweek) == 1)
	{
		beepOn();
		light_on();
	}
}

uint8_t checkTimer(uint8_t hour, uint8_t minute, uint8_t second, uint8_t dayofweek)
{
	if(hour == iClock1Hours &&  minute == iClock1Min && (second == 0 || second == 1 || second == 2) && iClock1Days[dayofweek] >= 1)
		return 1;
		
	return 0;
}

void setNewTimerValue(uint8_t hour, uint8_t minute)
{
	iClock1Hours = hour;
	iClock1Min = minute;
	
	eeprom_write_byte((uint8_t*) EEPROM_TimerHour, (uint8_t)(iClock1Hours));
	eeprom_write_byte((uint8_t*) EEPROM_TimerMinute, (uint8_t)(iClock1Min));
}

void readTimerValue()
{
	iClock1Hours = eeprom_read_byte((uint8_t*)EEPROM_TimerHour);
	iClock1Min = eeprom_read_byte((uint8_t*)EEPROM_TimerMinute);
}

void saveTimerDays()
{
	for(uint8_t i = 0; i < 7; i++)
		eeprom_write_byte((uint8_t*) i + EEPROM_TimerDays, (uint8_t)(iClock1Days[i]));
}

void readTimerDays()
{
	for(uint8_t i = 0; i < 7; i++)
		iClock1Days[i] = eeprom_read_byte((uint8_t*) i + EEPROM_TimerDays);
}

uint8_t getTimerValueHour()
{
	return iClock1Hours;
}

uint8_t getTimerValueMinute()
{
	return iClock1Min;
}

uint8_t *getTimerDays()
{
	return iClock1Days;
}

void setNewLightTimeOutValue(uint8_t iTimeOut)
{
	eeprom_write_byte((uint8_t*) EEPROM_DisplayTimeOut, (uint8_t)(iTimeOut));
}

void setNewSSID(char *cUSSID, uint8_t iLenght)
{
	for(uint8_t i = 0; i < iLenght; i++)
		eeprom_write_byte((uint8_t*) EEPROM_SSID + i, (char)(cUSSID[i]));
}

void setNewPWD(char *cPWD, uint8_t iLenght)
{
	for(uint8_t i = 0; i < iLenght; i++)
		eeprom_write_byte((uint8_t*) EEPROM_PWD + i, (char)(cPWD[i]));
}

void setNewCityID(char *cCityID, uint8_t iLenght)
{
	for(uint8_t i = 0; i < iLenght; i++)
		eeprom_write_byte((uint8_t*) EEPROM_CityID + i, (char)(cCityID[i]));
}
