#ifndef UART0_H
#define UART0_H

#ifdef __cplusplus
extern "C"
{
#endif

  
#include "ZComDef.h"
 
//
// Options for UART_SETUP macro
#define FLOW_CONTROL_ENABLE         0x40
#define FLOW_CONTROL_DISABLE        0x00
#define EVEN_PARITY                 0x20
#define ODD_PARITY                  0x00
#define NINE_BIT_TRANSFER           0x10
#define EIGHT_BIT_TRANSFER          0x00
#define PARITY_ENABLE               0x08
#define PARITY_DISABLE              0x00
#define TWO_STOP_BITS               0x04
#define ONE_STOP_BITS               0x00
#define HIGH_STOP                   0x02
#define LOW_STOP                    0x00
#define HIGH_START                  0x01
#define TRANSFER_MSB_FIRST          0x80
#define TRANSFER_MSB_LAST           0x00

// Where _source_ is one of
#define CRYSTAL 0x00
#define RC      0x01

// Macro for getting the clock division factor
#define CLKSPD  ( CLKCON & 0x01 )

/******************************************************************************
*******************  USART-UART specific functions/macros   *******************
******************************************************************************/
// The macros in this section simplify UART operation.
#define BAUD_E(baud, clkDivPow) (     \
    (baud==2400)   ?  6  +clkDivPow : \
    (baud==4800)   ?  7  +clkDivPow : \
    (baud==9600)   ?  8  +clkDivPow : \
    (baud==14400)  ?  8  +clkDivPow : \
    (baud==19200)  ?  9  +clkDivPow : \
    (baud==28800)  ?  9  +clkDivPow : \
    (baud==38400)  ?  10 +clkDivPow : \
    (baud==57600)  ?  10 +clkDivPow : \
    (baud==76800)  ?  11 +clkDivPow : \
    (baud==115200) ?  11 +clkDivPow : \
    (baud==153600) ?  12 +clkDivPow : \
    (baud==230400) ?  12 +clkDivPow : \
    (baud==307200) ?  13 +clkDivPow : \
    0  )


#define BAUD_M(baud) (      \
    (baud==2400)   ?  59  : \
    (baud==4800)   ?  59  : \
    (baud==9600)   ?  59  : \
    (baud==14400)  ?  216 : \
    (baud==19200)  ?  59  : \
    (baud==28800)  ?  216 : \
    (baud==38400)  ?  59  : \
    (baud==57600)  ?  216 : \
    (baud==76800)  ?  59  : \
    (baud==115200) ?  216 : \
    (baud==153600) ?  59  : \
    (baud==230400) ?  216 : \
    (baud==307200) ?  59  : \
  0)

/**********************************************************************************/
//typedef by your own
/******************************************************************************
 * TYPEDEFS for UART
 */

#define BUFFER_SIZE 256
typedef struct{
   uint8 pointer;
   char text[BUFFER_SIZE];
} BUFFER;

//Key define
#define ENTER      0x0D
#define BACK_SPACE 0x08
#define ESC        0x1B

#define IO_PER_LOC_USART0_AT_PORT0_PIN2345() do { PERCFG = (PERCFG&~0x01)|0x00; } while (0)

char uartGetkey (void);
void initUART(void);
void UART_SETUP(char uart,int baudRate,char options);
int uart0_putc(int c);
void uart0_puts(char* str,int len);
//------------------------------------------------------------------------------
  
#ifdef __cplusplus
}
#endif

#endif /* GENERICAPP_H */
