/****************************************************************************
	currency.h Definition for the currency class
	Author Andreas Dorrer
	Last Change: 08.12.2020
*****************************************************************************/

#include "currency.h"
#include <avr/pgmspace.h>

#include "ks0108lib/KS0108.h"
#include "ks0108lib/watchNrSmall.h"
#include "ks0108lib/Font5x8.h"
#include "icons/generallIcons.h"

#define GRAPHMIN 1.0
#define GRAPHMAX 37.0
#define GRAPHSTART 62
#define GRAPHXSTART 26

float fBpi = 0;
//float fBpiHistory[20] = {16000, 16400, 16100, 15900, 15800, 16000, 16200, 16100, 16050, 15800, 15500, 15500, 15600, 15600, 15600, 15700, 15800, 15900, 15800, 15700 };
float fBpiHistory[20] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
uint8_t uiUpdateHour = 0, uiUpdateMinute = 0;


void drawCurrencyDisplay()
{			
	char cTemp[20];
	char cTemp1[20];
	float fMin = fBpiHistory[19], fMax = 0.0;
	uint8_t inewVal = 0, iOldVal = 0;
	GLCD_Clear();
	
	GLCD_GotoXY(2, 1);
	GLCD_DrawBitmap(ic_bitcoin_14x19, 16, 24, GLCD_Overwrite);
	
	GLCD_SetFont(System8x12, 8, 12, GLCD_Overwrite);
	GLCD_GotoXY(20, 5);
	sprintf(cTemp, "%s$", dtostrf(fBpi, 1, 2, cTemp1)); // ftoa(cTemp1, fBpi, 2));
	GLCD_PrintString(cTemp);

	// chart
	GLCD_DrawLine(GRAPHXSTART, 23, GRAPHXSTART, 62, GLCD_Black);
	GLCD_DrawLine(GRAPHXSTART, 62, 127, 62, GLCD_Black);
	// Arrow y
	GLCD_SetPixel(GRAPHXSTART - 1, 24, GLCD_Black);
	GLCD_SetPixel(GRAPHXSTART + 1, 24, GLCD_Black);
	// Arrow x
	GLCD_SetPixel(127 - 1, 61, GLCD_Black);
	GLCD_SetPixel(127 - 1, 63, GLCD_Black);

	// doted line horizontal
	for(uint8_t i = GRAPHXSTART + 2; i<123; i+=3)
		GLCD_SetPixel(i, 43, GLCD_Black);

	// doted lines vertical
	uint8_t iX = GRAPHXSTART + 32;
	for(uint8_t i = 0; i < 2; i ++)
	{
		for(uint8_t j = 25; j < 62; j+= 3)
			GLCD_SetPixel(iX, j, GLCD_Black);
		iX += 33;
	}
					
	for(uint8_t i = 0; i < 20; i++)
	{
		if(fBpiHistory[i] > fMax)
			fMax = fBpiHistory[i];
		else if(fBpiHistory[i] < fMin)
			fMin = fBpiHistory[i];
	}

	GLCD_SetFont(Font5x8, 5, 8, GLCD_Overwrite);
	GLCD_GotoXY(0, 24);
	sprintf(cTemp, "%s", ftoa(cTemp1, fMax / 1000.0, 1));
	GLCD_PrintString(cTemp);

	GLCD_GotoX(0);
	GLCD_GotoLine(5);
	sprintf(cTemp, "%s", ftoa(cTemp1, (fMax + fMin) / 2000.0, 1));
	GLCD_PrintString(cTemp);

	GLCD_GotoX(0);
	GLCD_GotoLine(7);
	sprintf(cTemp, "%s", ftoa(cTemp1, fMin / 1000.0, 1));
	GLCD_PrintString(cTemp);

	GLCD_GotoX(98);
	GLCD_GotoLine(1);
	sprintf(cTemp, "%02d:%02d", uiUpdateHour, uiUpdateMinute );
	GLCD_PrintString(cTemp);

	float fRange = fMax - fMin;
	float fNewRange = GRAPHMAX - GRAPHMIN;

	if (fRange == 0)
		iOldVal = (GRAPHMAX + GRAPHMIN) / 2.0;
	else
		iOldVal = (((fBpiHistory[0] - fMin) * fNewRange) / fRange) + GRAPHMIN;

	iX = GRAPHXSTART + 2;
	for(uint8_t i = 1; i < 20; i++)
	{
		if (fRange == 0)
			inewVal = (GRAPHMAX + GRAPHMIN) / 2.0;
		else
			inewVal = (((fBpiHistory[i] - fMin) * fNewRange) / fRange) + GRAPHMIN;

		GLCD_DrawLine(iX, GRAPHSTART - iOldVal, iX + 5, GRAPHSTART - inewVal, GLCD_Black);
		iX += 5;

		iOldVal = inewVal;
	}
	GLCD_Render();
}