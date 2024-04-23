/*
	SmartCharger V1
	Andreas Dorrer
	16.05.2018
*/

/**
// Configure UART RX buffer
*/
#define F_CPU 16000000UL

#define PHASE_A		(PIND & 1<<PD2)
#define PHASE_B		(PIND & 1<<PD3)

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <string.h>

#include "utils.h"
#include "menu.h"
#include "clock.h"
#include "forecast.h"
#include "currency.h"
#include "ds1307/ds1307.h"
//#include "ds1820/ds1820.h"
#include "dht11/DHT.h"

#include "avr-wifi-esp8266/avr-wifi.h"
#include <avr/wdt.h>

/*
// Wifi access point ussid and password (to connect to)
**/
volatile uint16_t gl_iBreak = 0;
volatile uint8_t encoder_state=0, gl_iLightOnTimer = 0, gl_iLightOn = 0;
volatile uint8_t encoder_buffer[]={'N','N','N','N'};
volatile char ctempH, ctempL, chilf;
double fTist = 0, fHumIst = 0;
int8_t iTempInside = 0, iHumInside = 0;
	
char weather_data_buf[25];
char weather_icon[5] = "01d";
uint16_t weather_pressure = 0;
float weather_temp = 273.15;
uint8_t weather_humidity = 0, uiCurrHour = 0, uiCurrMinute = 0;
float weather_speed = 0;
int8_t enc_delta = 0;	

/**
// Simple JSON parser based on string finding
// This MAY easily break on invalid JSON
// But who cares
// Fully-loaded JSON parser for AVR is an overkill!
// So lets use old school methods :)
*/
void search_json(char* dest, const char* input, const char* prop) {
	char* loc = strstr(input, prop);
	const int propLen = strlen(prop);
	loc += propLen;
	int i = 0;
	while(*loc == '\"' || *loc == ':') ++loc;
	while(i < 25) {
		if(*loc == '\"' || *loc == '\0') {
			break;
		}
		dest[i] = *loc;
		++i;
		++loc;
	}
	dest[i] = '\0';
}

void parseData(char *input)
{
	weather_icon[4] = 0;
	search_json(weather_icon, input, "icon");
	
	memset(weather_data_buf, 0, sizeof(weather_data_buf));		
	search_json(weather_data_buf, input, "humidity");
	weather_humidity = atoi(weather_data_buf);
		
	memset(weather_data_buf, 0, sizeof(weather_data_buf));	
	search_json(weather_data_buf, input, "pressure");
	weather_pressure = atoi(weather_data_buf);
	
	memset(weather_data_buf, 0, sizeof(weather_data_buf));			
	search_json(weather_data_buf, input, "temp");
	weather_temp = atof(weather_data_buf);
	
	memset(weather_data_buf, 0, sizeof(weather_data_buf));		
	search_json(weather_data_buf, input, "speed");
	weather_speed = atof(weather_data_buf);
}

void parseForecastData(char *input)
{
	char *currelement = strstr(input, "\"dt\"");
	
	for(uint8_t i = 0; i < 3; i++)
	{
		if(currelement != NULL)
		{
			search_json(forecast_data[i].icon, currelement, "icon");
			search_json(forecast_data[i].timeStamp, currelement, "dt_txt");

			search_json(weather_data_buf, currelement, "temp_min");
			forecast_data[i].fTempMin = atof(weather_data_buf);
			
			search_json(weather_data_buf, currelement, "temp_max");
			forecast_data[i].fTempMax = atof(weather_data_buf);
			
			search_json(weather_data_buf, currelement, "speed");
			forecast_data[i].fSpeed = atof(weather_data_buf);			
		}
		else
			break;

		currelement = strstr(currelement + 4, "\"dt\"");
	}
}

void parseCoinDeskData(char *input)
{
	char *currelement = strstr(input, "\"EUR\"");
	
	memset(weather_data_buf, 0, sizeof(weather_data_buf));
	search_json(weather_data_buf, currelement, "rate_float");
	fBpi = atof(weather_data_buf);

	for(uint8_t i = 0; i < 20; i++)
		fBpiHistory[i] = fBpiHistory[i + 1];
	
	fBpiHistory[19] = fBpi;
	uiUpdateHour = uiCurrHour;
	uiUpdateMinute = uiCurrMinute;
}

int8_t encode_read4( void )			// read four step encoders
{
  int8_t val;

  cli();
  val = enc_delta;
  enc_delta &= 3;
  sei();
  return val >> 2;
}
 
 void encode_init( void )
 {
	 TCCR0B = 1<<WGM01^1<<CS01^1<<CS00;		// CTC, XTAL / 64
	 OCR0A = (uint8_t)(F_CPU / 64 * 1e-3 - 0.5);	// 2ms
	 
	 /*TCCR0A = 1<<COM0B0 | 1<<COM0B1; // set pin on OCROB compare interupt
	 OCR0B = 230;
	 TIMSK0 |= 1<<TOIE0 | 1<<OCR0B; */
 }
 
 void light_on()
 {
	PORTB |= (1<<PINB4);
	gl_iLightOnTimer = 0;
	gl_iLightOn = 1;
 }
 
 void light_off()
 {
	PORTB &=~ (1<<PINB4);
	gl_iLightOn = 0;
 }
 
int main(void) {
	
	uint16_t iCount = 1;
	int8_t iTemp = 0;
	char cTemp[80];

	//Rodary Encoder Pins
	DDRD &= ~(1<<DDD2);	// Rodary A
	DDRD &= ~(1<<DDD3); // Rodary B
	DDRB &= ~(1<<DDB2);	// Rodary Push
	DDRB |= (1<<DDB0); // esp reset 
	DDRB |= (1<<DDB4);	// lights
	DDRB |= (1<<DDB6);	// buzzer
	
	PORTD |= (1<<PD2);	// int. pull up
	PORTD |= (1<<PD3);	// int. pull up
	PORTB |= (1<<PB2);	// int. pull up
	PORTB &=~ (1<<PINB6); // buzzer off
	PORTB |= (1<<PINB0); // esp reset to high

	EICRA = 1<<ISC01 | 1<<ISC11 | 1<<ISC21;
	EIMSK = 1<<INT0 | 1<<INT1 | 1<<INT2;	
	
	// beep timer
	TCCR2B = 1<<CS21 | 1<<CS20 | 1<<CS22; // Clock prescaler 1024
	
	//enable watchdog interupt but no reset
	MCUSR = 0;
	wdt_enable(WDTO_1S);
	WDTCSR = 1<<WDCE;
	WDTCSR = 1<<WDIE;
	sei();
	
	gl_iLightTimeOut =  eeprom_read_byte((uint8_t*)EEPROM_DisplayTimeOut);
	
	for(uint8_t i = 0; i < SSID_MaxLenght; i++)
		gl_cUSSID[i] = eeprom_read_byte((uint8_t*) EEPROM_SSID + i);
		
	for(uint8_t i = 0; i < PWD_MaxLenght; i++)
		gl_cPASSWD[i] = eeprom_read_byte((uint8_t*) EEPROM_PWD + i);
		
	for(uint8_t i = 0; i < CITYID_Lenght; i++)
		gl_cCityID[i] = eeprom_read_byte((uint8_t*) EEPROM_CityID + i);
	
	_delay_ms(1500);	// start up delay
	
	// Init UART
	uart_init(UART_BAUD_SELECT(9600, F_CPU));	// ESP must be set to 9600Baud!! -> AT+CIOBAUD=9600
		
	//init ds1307
	ds1307_init();
	light_on();
	clockInit();
			
	encode_init();
	
	//DHT_Setup();

	// wifi ini
	_delay_ms(1000);
	ESP8266_WIFIMode(STATION);
	_delay_ms(1500);
	ESP8266_ConnectionMode(SINGLE);	
	_delay_ms(1500);
	ESP8266_ApplicationMode(NORMAL);
	_delay_ms(1500);
	
	while(1) 
	{		
		if(gl_iMenuMode == 1 || gl_iMenuMode == 2 || gl_iMenuMode == 3)
		{
			if(iCount == 0)
			{
				ESP8266_Clear();
				ESP8266_WakeUp();			
			}
			else if(iCount == 3)
				ESP8266_JoinAccessPoint(gl_cUSSID, gl_cPASSWD);				
			else if(iCount == 11)
				ESP8266_Start(0, DOMAIN, TCPPORT);
			else if(iCount == 16)
			{ 	
				sprintf(cTemp, "GET /data/2.5/weather?id=%s&appid=aedf62832ca80d32c097db341184f56b", gl_cCityID);
				ESP8266_Send(cTemp);
				//ESP8266_Send("GET /data/2.5/weather?id=2782555&appid=aedf62832ca80d32c097db341184f56b");
			}
			else if(iCount == 20)
			{
				ESP8266_Read_AndParseData();
				//ESP8266_EnterDeepSleep(50);
			}
			else if(iCount == 23)
				ESP8266_Start(0, DOMAIN, TCPPORT);
			else if(iCount == 27)
			{
				sprintf(cTemp, "GET /data/2.5/forecast?id=%s&appid=aedf62832ca80d32c097db341184f56b&cnt=3", gl_cCityID);
				ESP8266_Send(cTemp);
				ESP8266_Clear();
			}
			else if(iCount == 33)
			{
				ESP8266_Read_AndParseForecastData();
			}	
			else if(iCount == 36)
				ESP8266_Start(0, DOMAIN_COINDESK, TCPPORT);
			else if(iCount == 41)
				ESP8266_Send("GET /v1/bpi/currentprice.json HTTP/1.1\r\nHost: api.coindesk.com\r\nConnection: close\r\n");
			else if(iCount == 47)
			{
				ESP8266_Read_AndParseCoinDeskData();
				ESP8266_EnterDeepSleep(50);
			}
			
			// int temp sensor: measure every 30s
			if(iCount%30 == 0)
			{
				cli();
				if(dht_GetTempUtil(&iTempInside, &iHumInside) < 0)
					fTist = 10.0;
				sei();
			}
			
			if(gl_iMenuMode == 1)
				drawClockDisplay(weather_pressure, weather_temp, weather_humidity, weather_speed, weather_icon, iTempInside, iHumInside, &uiCurrHour, &uiCurrMinute);
			else if(gl_iMenuMode == 2)
				drawForecastDisplay();
			else
				drawCurrencyDisplay();
				
			iCount++;
			if(iCount > 600)	// update weather every 15min 900s, 10min 600s
				iCount = 0;
				
			if(gl_iLightOn)
				gl_iLightOnTimer++;
			if(gl_iLightOnTimer > gl_iLightTimeOut)
			{	
				if(gl_iBuzzerON)	// don't turn lights of when buzzer is enabled
					gl_iLightOnTimer = 0;
				else
				{	
					light_off();
					gl_iMenuMode = 1;
					TIMSK0 &=~ 1<<OCIE0A;	// stop rodary timer
				}
			}
			if(gl_iBuzzerON)
				gl_iBuzzerON++;
			if(gl_iBuzzerON > 60) // turn timer buzzer off after 1min
				beepOff();
							
			if(gl_iBuzzerON || gl_iIsSleeping == 0 || gl_iLightOn) // only go to sleep mode when buzzer is off. otherwise buzzer will make weird noise
				_delay_ms(1000);
			else
			{			
				set_sleep_mode(SLEEP_MODE_PWR_SAVE);
				sleep_mode(); 
			}
		}
		else if(gl_iMenuMode == 0 && gl_iBreak == 0)
		{
			drawMainMenu();
			gl_iBreak = 1;
		}
		
		if(TIMSK0 & (1<<OCIE0A))
		{
			iTemp = encode_read4() * (-1);
			if(iTemp != 0)
				(*Rodarytick)((int)iTemp);
		}
	}
}

void switchMainPage(int iIncrease)
{
	gl_iMenuMode += iIncrease;

	if(gl_iMenuMode > 3)
		gl_iMenuMode = 1;
	else if(gl_iMenuMode < 1)
		gl_iMenuMode = 3;
}

ISR(INT0_vect)	// rodary turned. direction doesn't matter this is just for lights and buzzer off. menu will use encode_read4()
{
	if(gl_iBuzzerON)
		beepOff();
}

ISR(INT1_vect)	// rodary turned. direction doesn't matter this is just for lights and buzzer off. menu will use encode_read4()
{
	if(gl_iBuzzerON)
		beepOff();

	if(!(TIMSK0 & (1<<OCIE0A)))
	{
		TIMSK0 |= 1<<OCIE0A; // start rodary timer
		Rodarytick = &switchMainPage;
	}

	light_on();
}

ISR(INT2_vect)	// rodary pushed
{
	light_on();
	
	if(gl_iBuzzerON)
		beepOff();	
	else if(gl_iMenuMode == 1 || gl_iMenuMode == 2 || gl_iMenuMode == 3)
	{
		gl_iMenuMode = 0;
		gl_iBreak = 0;
		TIMSK0 |= 1<<OCIE0A; // start rodary timer
	}
	else
		(*Rodarypush)();
}

ISR(TIMER0_COMPA_vect)				// 2ms for manual movement
{
	static int8_t last;
	int8_t new, diff;

	new = 0;
	if( PHASE_A )
	new = 3;
	if( PHASE_B )
	new ^= 1;					// convert gray to binary
	diff = last - new;				// difference last - new
	if( diff & 1 ){				// bit 0 = value (1)
		last = new;					// store new as next last
		enc_delta += (diff & 2) - 1;		// bit 1 = direction (+/-)
	}
}

ISR(WDT_vect)	// watchdog reset timer
{
	WDTCSR = 1<<WDCE;
	WDTCSR = 1<<WDIE;
}

ISR(TIMER2_OVF_vect)	// buzzer timer
{
	if(gl_iBeep > 10)
	{		
		if(iCountBeep < 1000)
			DDRB ^= (1<<DDB6);
	
		if(iCountBeep > 1500)
			iCountBeep = 0;
		
		iCountBeep++;
		gl_iBeep = 0;
	}
	gl_iBeep++;
}

ISR (USART0_RX_vect)
{	
	RESPONSE_BUFFER[Counter] = UDR0;
	Counter++;
	if(Counter >= DEFAULT_BUFFER_SIZE){
		Counter = 0; pointer = 0;
	}
}
