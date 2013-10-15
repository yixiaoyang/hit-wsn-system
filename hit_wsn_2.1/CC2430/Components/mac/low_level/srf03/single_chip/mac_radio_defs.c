/**************************************************************************************************
  Filename:       mac_radio_defs.c
  Revised:        $Date: 2007-10-29 22:38:47 -0700 (Mon, 29 Oct 2007) $
  Revision:       $Revision: 15812 $

  Description:    Describe the purpose and contents of the file.


  Copyright 2006-2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, 
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
**************************************************************************************************/

/* ------------------------------------------------------------------------------------------------
 *                                             Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "mac_radio_defs.h"
#include "hal_types.h"
#include "hal_assert.h"


/* ------------------------------------------------------------------------------------------------
 *                                        Global Constants
 * ------------------------------------------------------------------------------------------------
 */
const uint8 CODE macRadioDefsTxPowerTable[] =
{
  /*   0 dBm */   0x5F,   /* characterized as -0.4 dBm in datasheet */
  /*  -1 dBm */   0x3F,   /* characterized as -0.9 dBm in datasheet */
  /*  -2 dBm */   0x3F,
  /*  -3 dBm */   0x1B,   /* characterized as -2.7 dBm in datasheet */
  /*  -4 dBm */   0x17,   /* characterized as -4.0 dBm in datasheet */
  /*  -5 dBm */   0x13,   
  /*  -6 dBm */   0x13,   /* characterized as -5.7 dBm in datasheet */
  /*  -7 dBm */   0x13,
  /*  -8 dBm */   0x0F,   /* characterized as -7.9 dBm in datasheet */
  /*  -9 dBm */   0x0F,
  /* -10 dBm */   0x0F,
  /* -11 dBm */   0x0B,   /* characterized as -10.8 dBm in datasheet */
  /* -12 dBm */   0x0B,
  /* -13 dBm */   0x0B,
  /* -14 dBm */   0x0B,
  /* -15 dBm */   0x07,   /* characterized as -15.4 dBm in datasheet */
  /* -16 dBm */   0x07,
  /* -17 dBm */   0x07,
  /* -18 dBm */   0x07,
  /* -19 dBm */   0x06,   /* characterized as -18.6 dBm in datasheet */
  /* -20 dBm */   0x06,
  /* -21 dBm */   0x06,
  /* -22 dBm */   0x06,
  /* -23 dBm */   0x06,
  /* -24 dBm */   0x06,
  /* -25 dBm */   0x03    /* characterized as -25.2 dBm in datasheet */
};


/**************************************************************************************************
 *                                  Compile Time Integrity Checks
 **************************************************************************************************
 */
HAL_ASSERT_SIZE(macRadioDefsTxPowerTable, MAC_RADIO_TX_POWER_MAX_MINUS_DBM+1);  /* array size mismatch */

#if (HAL_CPU_CLOCK_MHZ != 32)
#error "ERROR: The only tested/supported clock speed is 32 MHz."
#endif

#if (MAC_RADIO_RECEIVER_SENSITIVITY_DBM > MAC_SPEC_MIN_RECEIVER_SENSITIVITY)
#error "ERROR: Radio sensitivity does not meet specification."
#endif


/**************************************************************************************************
 */
