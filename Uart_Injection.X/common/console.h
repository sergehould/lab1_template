/**********************************************************************
*	Serge Hould
* 	
*		REVISION HISTORY: 
* Author:			Date:				Description:
* SH                19 Aug. 2016        Delay modified for the profiling part of 
*                                      the function Generator lab using Explorer 16 at fcy = 16MHz. 
*                                      Two delays possible by defining SLOW or FAST.
*                                       See comments.								
* Serge Hould		16 Nov. 2017		Remove wait()and pmp_Init(void)functions (since it is now static)
* SH               19 Nov 2019         Merge Uart2 and pmp_lcd.  Use a one call back function for both consoles
* SH               9 March 2020     	Except for fprinf2()and LCDInit(), removed all other LCD functions (private)
* 					
* The original pmp_lcd.c file is located in :
* 247-501-VA_Advanced Programming in Microprocessors\A16\MPLAB-X_projects\common
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef __CONSOLE2_H_
#define __CONSOLE2_H_


#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#include "p24Fxxxx.h"
#include "pmp.h"

//#include "../common/pmp.h"

enum my_fp {
 _UART1,
 _UART2,
 _LCD
};

#define 	SLOW	// for slow LCD
//#define 	FAST	// for fast LCD

#ifdef		SLOW		// for slow LCD, delays modified for the profiling part of the function Generator 
						// lab using Explorer 16 at fcy = 16MHz
// Define a fast instruction execution time in terms of loop time
// typically > 43us
#define	LCD_F_INSTR		1700 //1.04 mS measured, Explorer 16 at fcy = 16MHz

// Define a slow instruction execution time in terms of loop time
// typically > 1.35ms
#define	LCD_S_INSTR		2000//1.22mS measured, Explorer 16 at fcy = 16MHz

// Define the startup time for the LCD in terms of loop time
// typically > 30ms
#define	LCD_STARTUP		1400000   //14000  
#endif

#ifdef		FAST		// for fast LCD, delays optimized for Explorer 16 at fcy 16MHz

// Define a fast instruction execution time in terms of loop time
// typically > 43us
#define	LCD_F_INSTR		70  //42uS measured, Explorer 16 at fcy = 16MHz

// Define a slow instruction execution time in terms of loop time
// typically > 1.35ms
#define	LCD_S_INSTR		150//76 uS measured, Explorer 16 at fcy = 16MHz    

// Define the startup time for the LCD in terms of loop time
// typically > 30ms
#define	LCD_STARTUP		2000
#endif


void LCDInit(void);

/** UART2 Driver Hardware Flags

  @Summary
    Specifies the status of the hardware receive or transmit

  @Description
    This type specifies the status of the hardware receive or transmit.
    More than one of these values may be OR'd together to create a complete
    status value.  To test a value of this type, the bit of interest must be
    AND'ed with value and checked to see if the result is non-zero.
*/
typedef enum
{
    /* Indicates that Receive buffer has data, at least one more character can be read */
    UART2_RX_DATA_AVAILABLE
        /*DOM-IGNORE-BEGIN*/  = (1 << 0) /*DOM-IGNORE-END*/,
    
    /* Indicates that Receive buffer has overflowed */
    UART2_RX_OVERRUN_ERROR
        /*DOM-IGNORE-BEGIN*/  = (1 << 1) /*DOM-IGNORE-END*/,

    /* Indicates that Framing error has been detected for the current character */
    UART2_FRAMING_ERROR
        /*DOM-IGNORE-BEGIN*/  = (1 << 2) /*DOM-IGNORE-END*/,

    /* Indicates that Parity error has been detected for the current character */
    UART2_PARITY_ERROR
        /*DOM-IGNORE-BEGIN*/  = (1 << 3) /*DOM-IGNORE-END*/,

    /* Indicates that Receiver is Idle */
    UART2_RECEIVER_IDLE
        /*DOM-IGNORE-BEGIN*/  = (1 << 4) /*DOM-IGNORE-END*/,

    /* Indicates that the last transmission has completed */
    UART2_TX_COMPLETE
        /*DOM-IGNORE-BEGIN*/  = (1 << 8) /*DOM-IGNORE-END*/,

    /* Indicates that Transmit buffer is full */
    UART2_TX_FULL
        /*DOM-IGNORE-BEGIN*/  = (1 << 9) /*DOM-IGNORE-END*/

}UART2_STATUS;

void UART2_Initialize(void);
void initUart2( void);
void initUart1( void);
void initUart2_wInt( void);
int putc2(char c);
int putc2_noHard(char c);
char getc2( void);
void puts2( char *str );
void outUint8(unsigned char u8_x);
void putI8(unsigned char u8_x);
/**
  Section: Macro Declarations
*/

#define UART2_DataReady  (U2STAbits.URXDA == 1)

/**
  Section: UART2 APIs
*/


/**
 * @brief Check if the UART2 transmitter is empty
 *
 * @return The status of UART2 TX empty checking.
 * @retval false the UART2 transmitter is not empty
 * @retval true the UART2 transmitter is empty
 */
bool UART2_is_tx_ready(void);

/**
 * @brief Check if the UART2 receiver is not empty
 *
 * @return The status of UART2 RX empty checking.
 * @retval false the UART2 receiver is empty
 * @retval true the UART2 receiver is not empty
 */
bool UART2_is_rx_ready(void);

/**
 * @brief Check if UART2 data is transmitted
 *
 * @return Receiver ready status
 * @retval false  Data is not completely shifted out of the shift register
 * @retval true   Data completely shifted out if the USART shift register
 */
bool UART2_is_tx_done(void);

/**
  @Summary
    Read a byte of data from the UART2.

  @Description
    This routine reads a byte of data from the UART2.

  @Preconditions
    UART2_Initialize() function should have been called
    before calling this function. This is a blocking function.
    So the recieve status should be checked to see
    if the receiver is not empty before calling this function.

  @Param
    None

  @Returns
    A data byte received by the driver.
*/
uint8_t UART2_Read(void);

 /**
  @Summary
    Writes a byte of data to the UART2.

  @Description
    This routine writes a byte of data to the UART2.

  @Preconditions
    UART2_Initialize() function should have been called
    before calling this function. The transfer status should be checked to see
    if transmitter is not busy before calling this function.

  @Param
    txData  - Data byte to write to the UART2

  @Returns
    None
*/
void UART2_Write(uint8_t txData);

/**
  @Summary
    Returns the transmitter and receiver status

  @Description
    This returns the transmitter and receiver status. The returned status may 
    contain a value with more than one of the bits
    specified in the UART2_STATUS enumeration set.  
    The caller should perform an "AND" with the bit of interest and verify if the
    result is non-zero (as shown in the example) to verify the desired status
    bit.

  @Preconditions
    UART2_Initializer function should have been called 
    before calling this function

  @Param
    None.

  @Returns
    A UART2_STATUS value describing the current status 
    of the transfer.

  @Example
    <code>
        while(!(UART2_StatusGet & UART2_TX_COMPLETE ))
        {
           // Wait for the tranmission to complete
        }
    </code>
*/

UART2_STATUS UART2_StatusGet (void );

/*Call back*/
int  fprintf2(int, char *);


#endif
