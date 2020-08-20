/* ADC converter source header file adc.h
 *		
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Author        	Date      			Comments on this revision		
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Serge Hould		December 15 2016     								-v1.1
 *
  * The original file is located in :
 * 247-501-VA_Advanced Programming in Microprocessors\A16\MPLAB-X_projects\common
 *
 ****************************************************************************/
 
#ifndef __ADC_H_
#define __ADC_H_

#include <xc.h>

//#include "p24Fxxxx.h"
void initADC(void);
int readADC( int ch);

#endif

