/*
* ATmega16_WIFI
* http://www.electronicwings.com
*
*/

#include <avr/io.h>			/* Include AVR std. library file */
#include <util/delay.h>			/* Include Delay header file */
#include <stdbool.h>			/* Include standard boolean library */
#include <string.h>			/* Include string library */
#include <stdio.h>			/* Include standard IO library */
#include <stdlib.h>			/* Include standard library */
#include <avr/interrupt.h>		/* Include avr interrupt header file */
#include "../avr-uart/uart.h"		/* Include USART header file */

#define SREG    _SFR_IO8(0x3F)

#define DEFAULT_BUFFER_SIZE		1400

/* Connection Mode */
#define SINGLE				0
#define MULTIPLE			1

/* Application Mode */
#define NORMAL				0
#define TRANSPERANT			1

/* Application Mode */
#define STATION				1
#define ACCESSPOINT			2
#define BOTH_STATION_AND_ACCESPOINT	3

/* Define Required fields shown below */
#define DOMAIN				"api.openweathermap.org"
#define DOMAIN_COINDESK		"api.coindesk.com"
#define TCPPORT				"80"
#define CHANNEL_ID			"119922"

uint8_t gl_iIsSleeping = 0;
volatile int16_t Counter = 0, pointer = 0;
char RESPONSE_BUFFER[DEFAULT_BUFFER_SIZE];

void ESP8266_Clear()
{
	memset(RESPONSE_BUFFER,0,DEFAULT_BUFFER_SIZE);
	Counter = 0;	pointer = 0;
}

void sendAT(char* ATCommand)
{
	ESP8266_Clear();
	uart_puts(ATCommand);
	uart_puts("\r\n");
}

void ESP8266_ApplicationMode(uint8_t Mode)
{
	char _atCommand[20];
	memset(_atCommand, 0, 20);
	sprintf(_atCommand, "AT+CIPMODE=%d", Mode);
	_atCommand[19] = 0;
	
	sendAT(_atCommand);
	
}

void ESP8266_ConnectionMode(uint8_t Mode)
{
	char _atCommand[20];
	memset(_atCommand, 0, 20);
	sprintf(_atCommand, "AT+CIPMUX=%d", Mode);
	_atCommand[19] = 0;
	
	sendAT(_atCommand);
}


void ESP8266_WIFIMode(uint8_t _mode)
{
	char _atCommand[20];
	memset(_atCommand, 0, 20);
	sprintf(_atCommand, "AT+CWMODE=%d", _mode);
	_atCommand[19] = 0;
	
	sendAT(_atCommand);
}

void ESP8266_JoinAccessPoint(char* _SSID, char* _PASSWORD)
{
	char _atCommand[60];
	memset(_atCommand, 0, 60);
	sprintf(_atCommand, "AT+CWJAP=\"%s\",\"%s\"", _SSID, _PASSWORD);
	_atCommand[59] = 0;
	
	sendAT(_atCommand);
}

void ESP8266_Start(uint8_t _ConnectionNumber, char* Domain, char* Port)
{
	char _atCommand[60];
	memset(_atCommand, 0, 60);
	_atCommand[59] = 0;

	sprintf(_atCommand, "AT+CIPSTART=\"TCP\",\"%s\",%s", Domain, Port);
	sendAT(_atCommand);
}

void ESP8266_Send(char* Data)
{
	char _atCommand[20];
	char _atData[strlen(Data)+2];
	memset(_atCommand, 0, 20);
	sprintf(_atCommand, "AT+CIPSEND=%d", (strlen(Data)+2));
	_atCommand[19] = 0;
	sendAT(_atCommand);
	
	_delay_ms(1000);
	
	sprintf(_atData, "%s", Data);
	_atData[strlen(Data)+1] = 0;
	sendAT(_atData);
}

void ESP8266_Read_AndParseData()
{
	if(strstr(RESPONSE_BUFFER, "temp") != NULL && strstr(RESPONSE_BUFFER, "icon") != NULL)
		parseData(RESPONSE_BUFFER);
	
	ESP8266_Clear();
}

void ESP8266_Read_AndParseForecastData()
{
	if(strstr(RESPONSE_BUFFER, "\"dt\"") != NULL)
		parseForecastData(RESPONSE_BUFFER);
	
	ESP8266_Clear();
}

void ESP8266_Read_AndParseCoinDeskData()
{
	if(strstr(RESPONSE_BUFFER, "EUR") != NULL)
		parseCoinDeskData(RESPONSE_BUFFER);
	
	ESP8266_Clear();
}

void ESP8266_EnterDeepSleep(uint32_t iDuration)
{
	char _atCommand[20];
	sprintf(_atCommand, "AT+GSLP=%d", iDuration);
	_atCommand[19] = 0;
		
	sendAT(_atCommand);
		
	gl_iIsSleeping = 1;
}

void ESP8266_WakeUp()
{
	if(gl_iIsSleeping == 1)
	{
		PORTB &= ~(1<<PINB0);
		_delay_ms(50);
		PORTB |= (1<<PINB0);
		
		gl_iIsSleeping = 0;
	}
}
