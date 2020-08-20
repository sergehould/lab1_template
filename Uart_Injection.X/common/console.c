/***********************************************************************************************
 *
 * Combined LCD and UART Drivers for PIC24.
 *
 ***********************************************************************************************
 * FileName:  console.c      
 *
 * * REVISION HISTORY for console.c:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author        	Date      			Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Serge Hould		16 Nov. 2017		Add comments. Make local functions static.		v2.0.0
 * SH				20 Nov 2019			Merge pmp_lcd and COMM2 libraries				v3.0.0
 * SH               22 Nov. 2019        Change name to console.c. Add Uart1.			v3.1.0
 * SH               6 Decembre 2019     Remove void UART2_Initialize(void), 			v3.1.1
 *                                      sets both UARTs default BAUD rate at 115200
 * SH               9 March 2020     	Except for fprinf2(), LCDInit(), made all LCD functions static	v3.1.2
 
 * * REVISION HISTORY for COMM2.c:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author        	Date      	Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Serge Hould	March 2010	First version of source file, tested on the target  - v1
* Serge Hould	December 2016	Add header										- v1.1
*								Add with function without flow control
*								Add an ISR 
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 *
 * The original file is located in :
 * C:\serge\Vanier\courses\247-511-VA Microcontroller Applications\Projects\common
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "console.h"
#include <string.h>
//#include <xc.h>
#define 	_ISR_NO_PSV 	__attribute__((__interrupt__, no_auto_psv))

/******* LCD section *****************/
unsigned long _uLCDloops;
static void Wait(unsigned int B);
static void pmp_Init(void);

static void LCDPos2(unsigned char row);
static void LCDPos1(unsigned char row);
static void LCDHome(void);
static void LCDL1Home(void);
static void LCDL2Home(void);
static void LCDClear(void);
static void LCDPut(char A);
static void DisplayMSG (char *);
static void LCDPutString( char *array);

static void pmp_Init(void)
{
	unsigned int mode,control,port,addrs,interrupt;
	
    control = BIT_PMP_ON | BIT_RD_WR_ON | BIT_RD_WR_HI ;
	mode = BIT_MODE_MASTER_1 | BIT_WAITB_4_TCY  | BIT_WAITM_15_TCY | BIT_WAITE_4_TCY ;
	port = BIT_P0;
	addrs = 0x0000;
	interrupt = 0x0000;
	
	PMPClose();
	PMPOpen(control,mode,port,addrs,interrupt);
}		

/***************************************************************
Name:	void LCDInit(void)
Description: Initializes the LCD using pmp bus.

****************************************************************/
void LCDInit(void)
{	
    pmp_Init();

	_uLCDloops = LCD_STARTUP;
	Wait(_uLCDloops);

	_uLCDloops = LCD_F_INSTR;
	PMDIN1 = 0b00111000;			// Set the default function
	Wait(_uLCDloops);

	_uLCDloops = LCD_STARTUP;
	Wait(_uLCDloops);

	_uLCDloops = LCD_F_INSTR;
	PMDIN1 = 0b00001100;
	Wait(_uLCDloops);

	_uLCDloops = LCD_STARTUP;
	Wait(_uLCDloops);

	_uLCDloops = LCD_S_INSTR;
	PMDIN1 = 0b00000001;			// Clear the display
	Wait(_uLCDloops);

	_uLCDloops = LCD_STARTUP;
	Wait(_uLCDloops);

	_uLCDloops = LCD_S_INSTR;
	PMDIN1 = 0b00000110;			// Set the entry mode
     
	Wait(_uLCDloops);

	LCDClear();
	LCDHome();
}

/***************************************************************
Name:	void LCDHome(void)
Description: Sets the position home.

****************************************************************/
static void LCDHome(void)
{
	_uLCDloops = LCD_S_INSTR;
//	PMADDR = 0x0000;
    PMPSetAddress(0x0000); 
	PMDIN1 = 0b00000010;
	while(_uLCDloops)
	_uLCDloops--;
}

/***************************************************************
Name:	void LCDL1Home(void)
Description: Sets the position home on line1

****************************************************************/
static void LCDL1Home(void)
{
	_uLCDloops = LCD_S_INSTR;
//	PMADDR = 0x0000;
    PMPSetAddress(0x0000); 
	PMDIN1 = 0b10000000;
	while(_uLCDloops)
	_uLCDloops--;
}
/***************************************************************
Name:	void LCDL2Home(void)
Description: Sets the position home on line2.

****************************************************************/
static void LCDL2Home(void)
{
	_uLCDloops = LCD_S_INSTR;
	//PMADDR = 0x0000;
	PMPSetAddress(0x0000); 
	PMDIN1 = 0b11000000;
	while(_uLCDloops)
	_uLCDloops--;
}

/***************************************************************
Name:	void LCDClear(void)
Description: Clears the whole LCD

****************************************************************/
static void LCDClear(void)
{
	_uLCDloops = LCD_S_INSTR;
	//PMADDR = 0x0000;
	PMPSetAddress(0x0000); 
	PMDIN1 = 0b00000001;
	while(_uLCDloops)
	_uLCDloops--;
}
/***************************************************************
Name:	void LCDPut(char A)
Description: Put a character at the current position.

****************************************************************/
static void LCDPut(char A)
{
	_uLCDloops = LCD_F_INSTR;
//	PMADDR = 0x0001;
    PMPSetAddress(0x0001); 
	PMDIN1 = A;
	while(_uLCDloops)
	_uLCDloops--;
	Nop();
	Nop();
	Nop();
	Nop();
}


/***************************************************************
Name:	void Wait(unsigned int B)
Description: Short delay

****************************************************************/
static void Wait(unsigned int B)
{
	while(B)
	B--;
}

/***************************************************************
Name:	void DisplayMSG( char *array)
Description: Dump a string to the current position. If it 
				reaches the end of line1, it will continue on
				line2.

****************************************************************/
static void DisplayMSG( char *array)
{
  unsigned char i=0,line=1;	
	LCDL1Home();	
	 while (*array)           // Continue display characters from STRING untill NULL character appears.
	 {
	  LCDPut(*array++);  // Display selected character from the STRING.
	  if (i>19 && line==1)
	  {
	   LCDL2Home();
	   line++;
	  }
	   i++;	        
     }
}

/***************************************************************
Name:	void LCDPutString( char *array)
Description: Dump a string to the current position. Does not take 
				into account the end of the line.

****************************************************************/
static void LCDPutString( char *array)
{
	 while (*array)           // Continue display characters from STRING untill NULL character appears.
	 {
	  LCDPut(*array++);  // Display selected character from the STRING.
     }
}


/***************************************************************
Name:	void LCDPos2(unsigned char row)
Description: Position the cursor to a specific position on line2.

****************************************************************/
static void LCDPos2(unsigned char row)
{
    unsigned char temp;
	_uLCDloops = LCD_S_INSTR;
//	PMADDR = 0x0000;
    PMPSetAddress(0x0000); 
  //  PMDIN1 = 0b11001010 ;
    temp = 0b11000000 | row;
	PMDIN1 = temp;
	while(_uLCDloops)
	_uLCDloops--;
}

/***************************************************************
Name:	void LCDPos1(unsigned char row)
Description: Position the cursor to a specific position on line1.

****************************************************************/
static void LCDPos1(unsigned char row)
{
    unsigned char temp;
	_uLCDloops = LCD_S_INSTR;
//	PMADDR = 0x0000;
    PMPSetAddress(0x0000); 
  //  PMDIN1 = 0b11001010 ;
    temp = 0b10000000 | row;
	PMDIN1 = temp;
	while(_uLCDloops)
	_uLCDloops--;
}

/*********** Uart2 section **************************************************/
//
//void UART2_Initialize(void)
//{
///**    
//     Set the UART2 module to the options selected in the user interface.
//     Make sure to set LAT bit corresponding to TxPin as high before UART initialization
//*/
//    // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; RXINV disabled; UEN TX_RX; 
//    U2MODE = (0x8008 & ~(1<<15));  // disabling UARTEN bit   
//    // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
//    U2STA = 0x0000;
//    // BaudRate = 9600; Frequency = 16000000 Hz; BRG 416; 
//    U2BRG = 0x01A0;
//    
//    U2MODEbits.UARTEN = 1;  // enabling UARTEN bit
//    U2STAbits.UTXEN = 1; 
//   
//}

//
// I/O definitions for the Explorer16 using hardware flow control
#define CTS    	_RF12              // Cleart To Send, input, HW handshake
#define RTS     _RF13               // Request To Send, output, HW handshake
#define TRTS    TRISFbits.TRISF13   // tris control for RTS pin

/* U2BRG (BRATE)
U2BRG = (Fosc / 8 / baudrate) -1 ; for BREGH=1
In our case this translates to the following expression:
 */

/* U2BRG (BRATE)
U2BRG = (Fosc / 32 / baudrate) -1 ; for BREGH=0
In our case this translates to the following expression:

*/
// timing and baud rate calculations
//#define BRATE   33        // 115200 baud (BREGH=1) for PIC24  at 32MHz  BRATE=33 
#define BRATE   	417        // 9600 baud (BREGH=1) for PIC24  at 32MHz  BRATE=417 
#define U_ENABLE 	0x8008      // enable the UART peripheral (BREGH=1)
#define U_TX    	0x0400      // enable transmission
   
/**********************************
 Initialize the UART2 serial port
**********************************/
void initUart2( void)
{
   U2BRG    = BRATE;    
   U2MODE    = U_ENABLE;
   U2STA    = U_TX;
   TRTS    = 0;        // make RTS output
   RTS     = 1;        // set RTS default status
} // initUart

/**********************************
 initialize the UART2 serial port 
 with interrupt.  
 See ISR at the end of this document
 **********************************/
void initUart2_wInt( void)
{
    /* Calcul de U2BRG (BRATE)*/
    //U2BRG = (Fcy/ 4 / baudrate) -1 ; for BREGH=0
   U2BRG    = BRATE ;    // 16M/4/9600 = 417
   U2MODE    = U_ENABLE ;     // enable the UART peripheral (BREGH=1)
   U2STA    = U_TX;      // enable transmission
   _U2RXIF=0;  
   _U2RXIP=1;  // Interrutp priority 1
   _U2RXIE=1;  // if interrupt driven RX only

} // initUart
/****************************************
Send a singe character to the UART2 
serial port.

input: 
	Parameters:
		char c 	character to be sent
output:
	return:
		int		return the character sent.
*****************************************/
int putc2(char c)
{
   while ( CTS);              // wait for !CTS, clear to send
   while ( U2STAbits.UTXBF);   // wait while Tx buffer full
   U2TXREG = c;
   return c;
} 

/****************************************
Same as putc2() but  w/o hardware control
*****************************************/
int putc2_noHard(char c)
{
   while ( U2STAbits.UTXBF);   // wait while Tx buffer is still full
   U2TXREG = c;
   return c;
}

/****************************************
*****************************************/
// wait for a new character to arrive to the UART2 serial port
char getc2( void)
{
    RTS = 0;            // assert Request To Send !RTS
   while ( !U2STAbits.URXDA);   // wait for a new character to arrive
   return U2RXREG;      // read the character from the receive buffer
   RTS = 1;
}// 


   /*******************************************************************************
   Function: puts2( char *str )

   Precondition:
      initUart must be called prior to calling this routine.

   Overview:
      This function prints a string of characters to the UART.

   Input: Pointer to a null terminated character string.

   Output: None.

   *******************************************************************************/
   void puts2( char *str )
   {
      unsigned char c;

      while( (c = *str++) )
         putc2(c);
   }

   
   // to erase because putI8 supercedes it
void outUint8(unsigned char u8_x) {
  unsigned char u8_c;
   putc2('0');
   putc2('X');
  u8_c = (u8_x>>4)& 0xf;
  if (u8_c > 9) putc2('A'+u8_c-10);
  else putc2('0'+u8_c);
  //LSDigit
  u8_c= u8_x & 0xf;
  if (u8_c > 9) putc2('A'+u8_c-10);
  else putc2('0'+u8_c);
}

/****************************************
Precondition:
    initUart2() must be called prior to calling 
	this routine.

	Overview: send an 8 bit integer value to 	
	the UART2 serial port
	Example: putI8(55);
*****************************************/
void putI8(unsigned char u8_x) {
  unsigned char u8_c;
   putc2('0');
   putc2('X');
  u8_c = (u8_x>>4)& 0xf;
  if (u8_c > 9) putc2('A'+u8_c-10);
  else putc2('0'+u8_c);
  //LSDigit
  u8_c= u8_x & 0xf;
  if (u8_c > 9) putc2('A'+u8_c-10);
  else putc2('0'+u8_c);
}


bool UART2_is_tx_ready(void)
{
    return (IFS1bits.U2TXIF && U2STAbits.UTXEN);
}

bool UART2_is_rx_ready(void)
{
    return IFS1bits.U2RXIF;
}

bool UART2_is_tx_done(void)
{
    return U2STAbits.TRMT;
}

uint8_t UART2_Read(void)
{
    while(!(U2STAbits.URXDA == 1))
    {
        
    }

    if ((U2STAbits.OERR == 1))
    {
        U2STAbits.OERR = 0;
    }

    

    return U2RXREG;
}

void UART2_Write(uint8_t txData)
{
    while(U2STAbits.UTXBF == 1)
    {
        
    }

    U2TXREG = txData;    // Write the data byte to the USART.
}

UART2_STATUS UART2_StatusGet (void)
{
    return U2STA;
}

/****************************************
	ISR for Uart2 rx
*****************************************/

//void _ISR_NO_PSV _U2RXInterrupt( void )
//{
//    char cChar;
//
//	IFS1bits.U2RXIF = 0;
//	while( U2STAbits.URXDA )
//	{
//		cChar = U2RXREG;
//	}
//
//}

   
/***************** Uart1 section************************************/


void UART1_Initialize(void)
{
/**    
     Set the UART1 module to the options selected in the user interface.
     Make sure to set LAT bit corresponding to TxPin as high before UART initialization
*/
    // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; RXINV disabled; UEN TX_RX; 
    // Data Bits = 8; Parity = None; Stop Bits = 1;
    U1MODE = (0x8008 & ~(1<<15));  // disabling UARTEN bit
    // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
    U1STA = 0x00;

    U1BRG = BRATE;
    
    U1MODEbits.UARTEN = 1;   // enabling UART ON bit
    U1STAbits.UTXEN = 1;
}

void initUart1(void){
    UART1_Initialize();
}

uint8_t UART1_Read(void)
{
    while(!(U1STAbits.URXDA == 1))
    {
        
    }

    if ((U1STAbits.OERR == 1))
    {
        U1STAbits.OERR = 0;
    }
    
    return U1RXREG;
}

void UART1_Write(uint8_t txData)
{
    while(U1STAbits.UTXBF == 1)
    {
        
    }

    U1TXREG = txData;    // Write the data byte to the USART.
}

bool UART1_IsRxReady(void)
{
    return U1STAbits.URXDA;
}

bool UART1_IsTxReady(void)
{
    return (U1STAbits.TRMT && U1STAbits.UTXEN );
}

bool UART1_IsTxDone(void)
{
    return U1STAbits.TRMT;
}

int __attribute__((__section__(".libc.write"))) write(int handle, void *buffer, unsigned int len) 
{
    unsigned int i;

    for (i = len; i; --i)
    {
        UART1_Write(*(char*)buffer++);
    }
    return(len);
}

/*******************************************************************************

  !!! Deprecated API !!!
  !!! These functions will not be supported in future releases !!!

*******************************************************************************/

uint16_t __attribute__((deprecated)) UART1_StatusGet (void)
{
    return U1STA;
}

void __attribute__((deprecated)) UART1_Enable(void)
{
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
}

void __attribute__((deprecated)) UART1_Disable(void)
{
    U1MODEbits.UARTEN = 0;
    U1STAbits.UTXEN = 0;
}

/***** end of UART1 section ******************/


/*******************************************************************************
   Function: int  fprintf2(int mode, char *buffer){

   Precondition:
      initUartx must be called prior to calling this routine.

   Overview:
        This function prints a string of characters to the selected console.
        Possible choices are _UART1, _UART2, _LCD

   Input: 
        mode: select the console _UART1, _UART2 or  _LCD
        buffer: Pointer to a character string to be outputted

   Output: returns the number of characters transmitted to the console

   *******************************************************************************/

int  fprintf2(int mode, char *buffer){
    int len =0;
    unsigned int i;
    char c;
    len = strlen(buffer);

    switch(mode){
        case _UART1:
        /* Uart1 */
            
            for (i = len; i; --i){
                UART1_Write(*(char*)buffer++);
            }
            //return(len);
            break;
        /* Uart2 */
        case _UART2:
            while(U2STAbits.TRMT == 0);  
                for (i = len; i; --i)
                {
                    while(U2STAbits.TRMT == 0);
                    U2TXREG = *(char*)buffer++;        
                }
                //return(len);
            break;
        /* LCD */
        case _LCD:
            LCDL1Home();
            for (i = len; i; --i){
                c = *(char*)buffer++;
                if( c== '\n')LCDL2Home();
                else {     
                    LCDPut(c);
                }
            }
            break;
    }// switch case 
     return(len);
}
