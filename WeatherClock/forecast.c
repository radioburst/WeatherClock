/****************************************************************************
	forecast.h Definition for the forecast class
	Author Andreas Dorrer
	Last Change: 25.12.2018
*****************************************************************************/

#include "forecast.h"
#include <avr/pgmspace.h>

#include "ks0108lib/KS0108.h"
#include "ks0108lib/Font5x8.h"

struct ForecastData forecast_data[3];

void drawForecastDisplay()
{			
	char cIntTemp[25];
	char cIntTemp1[10];
	char cTempS[5] = { "%s@C" };
	char cTempS1[3] = { "%s" };
	char cTempKm[5] = { "%skm" };
			
	GLCD_Clear();
	GLCD_SetFont(Font5x8, 5, 8, GLCD_Overwrite);
	
	for(uint8_t i = 1; i < 64; i+=2)
		GLCD_SetPixel(42, i, GLCD_Black);
		
	for(uint8_t i = 1; i < 64; i+=2)
		GLCD_SetPixel(85, i, GLCD_Black);
		
	GLCD_GotoXY(5, 2);
	drawWeatherIcon(forecast_data[0].icon);
	GLCD_GotoXY(48, 2);
	drawWeatherIcon(forecast_data[1].icon);
	GLCD_GotoXY(91, 2);
	drawWeatherIcon(forecast_data[2].icon);
				
	GLCD_GotoLine(4);
	GLCD_GotoX(0);
	sprintf(cIntTemp, cTempS1, strstr(forecast_data[0].timeStamp, " "));
	cIntTemp[6] = 0;
	GLCD_PrintString(cIntTemp);
	GLCD_GotoX(44);
	sprintf(cIntTemp, cTempS1, strstr(forecast_data[1].timeStamp, " "));
	cIntTemp[6] = 0;
	GLCD_PrintString(cIntTemp);
	GLCD_GotoX(87);
	sprintf(cIntTemp, cTempS1, strstr(forecast_data[2].timeStamp, " "));
	cIntTemp[6] = 0;
	GLCD_PrintString(cIntTemp);	
		
	GLCD_GotoLine(5);
	GLCD_GotoX(2);
	sprintf(cIntTemp, cTempS, ftoa(cIntTemp1, forecast_data[0].fTempMax-273.15, 1));
	GLCD_PrintString(cIntTemp);
	GLCD_GotoX(46);
	sprintf(cIntTemp, cTempS, ftoa(cIntTemp1, forecast_data[1].fTempMax-273.15, 1));
	GLCD_PrintString(cIntTemp);	
	GLCD_GotoX(89);
	sprintf(cIntTemp, cTempS, ftoa(cIntTemp1, forecast_data[2].fTempMax-273.15, 1));
	GLCD_PrintString(cIntTemp);
	
	GLCD_GotoLine(6);
	GLCD_GotoX(2);	
	sprintf(cIntTemp, cTempS, ftoa(cIntTemp1, forecast_data[0].fTempMin-273.15, 1));
	GLCD_PrintString(cIntTemp);		
	GLCD_GotoX(46);
	sprintf(cIntTemp, cTempS, ftoa(cIntTemp1, forecast_data[1].fTempMin-273.15, 1));
	GLCD_PrintString(cIntTemp);	
	GLCD_GotoX(89);
	sprintf(cIntTemp, cTempS, ftoa(cIntTemp1, forecast_data[2].fTempMin-273.15, 1));
	GLCD_PrintString(cIntTemp);
	
	GLCD_GotoLine(7);
	GLCD_GotoX(2);
	sprintf(cIntTemp, cTempKm, ftoa(cIntTemp1, forecast_data[0].fSpeed * 3.6, 1));
	GLCD_PrintString(cIntTemp);
	GLCD_GotoX(46);
	sprintf(cIntTemp, cTempKm, ftoa(cIntTemp1, forecast_data[1].fSpeed * 3.6, 1));
	GLCD_PrintString(cIntTemp);
	GLCD_GotoX(89);
	sprintf(cIntTemp, cTempKm, ftoa(cIntTemp1, forecast_data[2].fSpeed * 3.6, 1));
	GLCD_PrintString(cIntTemp);
	
	GLCD_Render();
}
