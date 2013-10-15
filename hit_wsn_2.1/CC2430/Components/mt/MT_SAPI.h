/**************************************************************************************************
  Filename:       MT_SAPI.h
  Revised:        $Date: 2007-10-28 18:43:04 -0700 (Sun, 28 Oct 2007) $
  Revision:       $Revision: 15800 $

  Description:    MonitorTest functions for the Simple API.


  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

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

#ifndef MT_SAPI_H
#define MT_SAPI_H

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "MTEL.h"
#include "sapi.h"

#include "OnBoard.h"

/*********************************************************************
 * MACROS
 */

#define SAPICB_CHECK(cbi) (_sapiCallbackSub & ( 1 << ( cbi & 0x0F ) ) )

/*********************************************************************
 * CONSTANTS
 */

// SAPI MT Command Identifiers
#define SPI_CMD_SAPI_SYS_RESET              0x0C00
#define SPI_CMD_SAPI_START_REQ              0x0C01
#define SPI_CMD_SAPI_BIND_DEVICE            0x0C02
#define SPI_CMD_SAPI_ALLOW_BIND             0x0C03
#define SPI_CMD_SAPI_SEND_DATA              0x0C04
#define SPI_CMD_SAPI_READ_CFG               0x0C05
#define SPI_CMD_SAPI_WRITE_CFG              0x0C06
#define SPI_CMD_SAPI_GET_DEV_INFO           0x0C07
#define SPI_CMD_SAPI_FIND_DEV               0x0C08
#define SPI_CMD_SAPI_PMT_JOIN               0x0C09

// SAPI MT Callback Identifiers
#define SPI_SAPI_CB_TYPE                    0x0C80

#define SPI_CB_SAPI_START_CNF               0x0C80
#define SPI_CB_SAPI_SEND_DATA_CNF           0x0C81
#define SPI_CB_SAPI_BIND_CNF                0x0C82
#define SPI_CB_SAPI_FIND_DEV_CNF            0x0C83
#define SPI_CB_SAPI_RCV_DATA_IND            0x0C84
#define SPI_CB_SAPI_READ_CFG_RSP            0x0C85
#define SPI_CB_SAPI_DEV_INFO_RSP            0x0C86
#define SPI_CB_SAPI_ALLOW_BIND_CNF          0x0C87

// SAPI MT Message Lengths
#define SPI_CB_SAPI_START_CNF_LEN           1
#define SPI_CB_SAPI_SEND_DATA_CNF_LEN       2
#define SPI_CB_SAPI_BIND_CNF_LEN            3
#define SPI_CB_SAPI_DISC_NET_CNF_LEN        1
#define SPI_CB_SAPI_FIND_DEV_CNF_LEN        11
#define SPI_CB_SAPI_RCV_DATA_IND_LEN        6
#define SPI_CB_SAPI_ALLOW_BIND_CNF_LEN      2

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

#if defined ( MT_SAPI_CB_FUNC )
extern uint16 _sapiCallbackSub;
#endif

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

#if defined ( MT_SAPI_FUNC )

uint8 MT_sapiCommandProcessing( uint16 cmd_id , byte len , byte *pData );

#endif  // MT_SAPI_FUNC

#if defined ( MT_SAPI_CB_FUNC )

extern void zb_MTCallbackStartConfirm( uint8 status );

extern void zb_MTCallbackSendDataConfirm( uint8 handle, uint8 status );

extern void zb_MTCallbackBindConfirm( uint16 commandId, uint8 status );

extern void zb_MTCallbackFindDeviceConfirm( uint8 searchType,
                                        uint8 *searchKey, uint8 *result );

extern void zb_MTCallbackReceiveDataIndication( uint16 source,
                              uint16 command, uint16 len, uint8 *pData  );

extern void zb_MTCallbackAllowBindConfirm( uint16 source );

#endif // MT_SAPI_CB_FUNC

/*********************************************************************
*********************************************************************/
#endif
