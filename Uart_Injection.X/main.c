/******************************************************************************
 * 	FileName:     main.c
 *
 *	Description: to complete
 *
 *    
 * REVISION HISTORY:
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Author        	Date      	Comments on this revision
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * to complete
 *
 * 
 * 
 * 		
 *******************************************************************************/

#include "common/Tick.h"
#include "common/console.h"
#include "generator.h"

int main( void)
{
    OSCILLATOR_Initialize();
	DWORD  timeStamp = 0;
    initIO();
   // initT3_3kHz();        // initializes generator at 3 kHz
    initT3_50Hz();         // initializes generator at 100 Hz

    TickInit();			// initializes the tick function   
    // LCD test
    //LCDInit();
    //fprintf2(_LCD, "Hello LCD\n");
    
    // Uart1 test
    initUart1();
    fprintf2(_UART1, "Hello UART1\n");
    
    while( 1){
        if(TickGet()- timeStamp >= (TICKS_PER_SECOND>>3)){
            _LATA6 = ~_LATA6 ;
            timeStamp =TickGet(); 
            fprintf2(_UART1, "looping\n"); 
        }
	}//while(1))

}// main

