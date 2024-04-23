#ifndef KS0108_SETTINGS_H_INCLUDED
#define KS0108_SETTINGS_H_INCLUDED
/*
||
||  Filename:	 		KS0108_Settings.h
||  Title: 			    KS0108 Driver Settings
||  Author: 			Efthymios Koktsidis
||	Email:				efthymios.ks@gmail.com
||  Compiler:		 	AVR-GCC
||	Description:
||	Settings for the KS0108 driver. 
||
*/

//----- Configuration -------------//
//Chip Enable Pin
#define GLCD_Active_Low		0

//GLCD pins					PORT, PIN
#define GLCD_D0				A, 7
#define GLCD_D1				A, 6
#define GLCD_D2				A, 5
#define GLCD_D3				A, 4
#define GLCD_D4				A, 3
#define GLCD_D5				A, 2
#define GLCD_D6				A, 1
#define GLCD_D7				A, 0

#define GLCD_DI				D, 5
#define GLCD_RW				C, 6
#define GLCD_EN				C, 7
#define GLCD_CS1			D, 6
#define GLCD_CS2			D, 7
#define GLCD_RST			B, 1
//---------------------------------//
#endif