/**************************************************************************************************
  Filename:       ZLOAD_IF.h
  Revised:        $Date: 2007-10-27 17:16:54 -0700 (Sat, 27 Oct 2007) $
  Revision:       $Revision: 15793 $

  Description:    ZLOAD header declaring prototypes


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
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
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

#ifndef ZLOAD_IF_H
#define ZLOAD_IF_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * MACROS
 */

#ifdef __ICC8051__
  #define  OAD_MAKE_IMAGE_ID()   \
    const __code __root unsigned short imgId[] @ "OAD_IMAGE_ID" = {IMAGE_VERSION, MANUFACTURER_ID, PRODUCT_ID}
#else
  #define  OAD_MAKE_IMAGE_ID()   \
    const __root __farflash unsigned short imgId[] @ "OAD_IMAGE_ID" = {IMAGE_VERSION, MANUFACTURER_ID, PRODUCT_ID}
#endif

/*********************************************************************
 * DEFINES
 */

// support to select callback event for which subscription is desired
// this information is transmitted as a bit map.
#define  ZLCB_EVENT_OADBEGIN_CLIENT     ((unsigned short)0x0001)
#define  ZLCB_EVENT_OADEND_CLIENT       ((unsigned short)0x0002)
#define  ZLCB_EVENT_OADBEGIN_SERVER     ((unsigned short)0x0004)
#define  ZLCB_EVENT_OADEND_SERVER       ((unsigned short)0x0008)
#define  ZLCB_EVENT_CODE_ENABLE_RESET   ((unsigned short)0x0010)
#define  ZLCB_EVENT_ALL                 (ZLCB_EVENT_OADBEGIN_CLIENT   | \
                                         ZLCB_EVENT_OADEND_CLIENT     | \
                                         ZLCB_EVENT_OADBEGIN_SERVER   | \
                                         ZLCB_EVENT_OADEND_SERVER     | \
                                         ZLCB_EVENT_CODE_ENABLE_RESET   \
                                        )


/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the ZLOAD Application
 */
extern void ZLOADApp_Init( byte task_id );

/*
 * Task Event Processor for the ZLOAD Application
 */
extern UINT16 ZLOADApp_ProcessEvent( byte task_id, unsigned short events );


/*
 * API for user to register pre-reset callback
 */
extern void ZLOADApp_RegisterOADEventCallback(void(*pCBFunction)(unsigned short), unsigned short eventMask);

#endif  // ZLOAD_IF_H