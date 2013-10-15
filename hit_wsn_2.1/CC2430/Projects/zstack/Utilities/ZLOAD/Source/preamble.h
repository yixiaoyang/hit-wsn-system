/**************************************************************************************************
  Filename:       preamble.h
  Revised:        $Date: 2007-10-27 17:16:54 -0700 (Sat, 27 Oct 2007) $
  Revision:       $Revision: 15793 $

  Description:    This file provides the Customer access point for specification
                  of image identification including Version, Manufacturer ID and
                  Product ID, each 2 bytes. There is an entry for each of End
                  Device, Router, and Coordinator. This file is included by the
                  source file ZLOAD_App.c and results in the identifying infomation
                  being placed at a known offset in the binary image that results
                  from the build.


  Copyright 2005-2007 Texas Instruments Incorporated. All rights reserved.

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

#ifndef PREAMBLE_H
#define PREAMBLE_H

#if defined(ZDO_COORDINATOR)
  // COORDINATOR
  #define IMAGE_VERSION    ((uint16) 0x3043)
  #define MANUFACTURER_ID  ((uint16) 0xF8F8)
  #define PRODUCT_ID       ((uint16) 0x00AD)

#elif defined(RTR_NWK)
  //ROUTER
  #define IMAGE_VERSION    ((uint16) 0x3252)
  #define MANUFACTURER_ID  ((uint16) 0xF8F8)
  #define PRODUCT_ID       ((uint16) 0x00AD)

#else
  //END DEVICE
  #define IMAGE_VERSION    ((uint16) 0x3045)
  #define MANUFACTURER_ID  ((uint16) 0xF8F8)
  #define PRODUCT_ID       ((uint16) 0x00AD)
#endif


#endif