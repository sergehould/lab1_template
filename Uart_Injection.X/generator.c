/******************************************************************************
 * 	FileName:     generator.c
 *
 *
 *	Description: Generates a square wave at 2 different frequencies.
 *
 *    
 * REVISION HISTORY:
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Author        	Date      	Comments on this revision
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * SH				Sept. 2019	V1.0
 *
 * 
 * 
 * 		
 *******************************************************************************/

#include "generator.h"

/*
 *  Generator output
 * 
*/
void _ISR_NO_PSV  _T3Interrupt(void)
{
	_T3IF = 0; // Clear Timer3 Interrupt
    /* Toggles pin RD1  */
    _LATD1 ^=1;
}

/*
 *  Function that sets timer 3 for 3kHz or 100 Hz 
 * 
*/
void initT3_3kHz( void)
{
	T3CONbits.TON = 0; 
	T3CONbits.TCS = 0; // Select internal instruction cycle clock
	T3CONbits.TGATE = 0; // Disable Gated Timer mode
	T3CONbits.TCKPS = 0b00; 
	TMR3 = 0x00; 
	PR3=2666;  // 16 000 000/(3000Hz*2) = 2666  for two toggles per cycle
    _T3IP = 0x01; // DON'T CHANGE THIS SETTING
	_T3IF = 0; 
	IEC0bits.T3IE = 1; 
    T3CONbits.TON = 1; 
} 

void initT3_50Hz( void)
{
	T3CONbits.TON = 0; 
	T3CONbits.TCS = 0; // Select internal instruction cycle clock
	T3CONbits.TGATE = 0; // Disable Gated Timer mode
	T3CONbits.TCKPS = 0b01; // divide by 8
	TMR3 = 0x00; 
	PR3=20000; // (16M/8)/50Hz*2 = 20000
    _T3IP = 0x01; // DON'T CHANGE THIS SETTING
	_T3IF = 0; 
	IEC0bits.T3IE = 1; 
    T3CONbits.TON = 1; 
} 
