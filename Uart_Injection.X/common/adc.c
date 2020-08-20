/* ADC converter functions source file adc.c
 *		
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Author        	Date      				Comments on this revision		
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Serge Hould		December 15 2016     	-v1.1
 *
 * 
  * The original file is located in :
 * 247-501-VA_Advanced Programming in Microprocessors\A16\MPLAB-X_projects\common
 ***************************************************************************/

#include "adc.h"

// initialize the ADC for single conversion, select Analog input pins
void initADC(void) 
{
    //AD1PCFGL = amask;    // select analog input pins
    AD1CON1 = 0b0000010011100000;   // auto convert after end of sampling  0b0000 0000 1110 0000=0x00E0  pour non-signé et 10 bits
																		//  0b0000 0101 1110 0000  pour signé et 12 bits
    AD1CSSL = 0;        // no scanning required 
    AD1CON3 = 0x1F02;   // max sample time = 31Tad, Tad = 2 x Tcy = 125ns >75ns
    AD1CON2 = 0;        // use MUXA, AVss and AVdd are used as Vref+/-
    AD1CON1bits.ADON = 1; // turn on the ADC
} //initADC


int readADC( int ch)
{
    AD1CHS  = ch;               // select analog input channel
    AD1CON1bits.SAMP = 1;       // start sampling, automatic conversion will follow
     while (!AD1CON1bits.DONE);   // wait to complete the conversion
    return ADC1BUF0;            // read the conversion result
} // readADC

