/**************************************************************************************************
  Filename:       GenericApp.c
  Revised:        $Date: 2007-10-27 17:16:54 -0700 (Sat, 27 Oct 2007) $
  Revision:       $Revision: 15793 $

  Description:    Generic Application (no Profile).


  Copyright 2004-2007 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
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

/*********************************************************************
  This application isn't intended to do anything useful, it is
  intended to be a simple example of an application's structure.

  This application sends "Hello World" to another "Generic"
  application every 15 seconds.  The application will also
  receive "Hello World" packets.

  The "Hello World" messages are sent/received as MSG type message.

  This applications doesn't have a profile, so it handles everything
  directly - itself.

  Key control:
    SW1:
    SW2:  initiates end device binding
    SW3:
    SW4:  initiates a match description request
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "GenericApp.h"
#include "DebugTrace.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "uart0.h"

//xiaoyang add 
#include <string.h>
#include <stdio.h>
#include "SPIMgr.h"
//#include <stdlib.h>
/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * MACROS
 */
//xiaoyang add
/* See the important comment by the otaBuf declaration below concerning the
 * mandatory use of this macro vice directly invoking osal_mem_free().
 */
#define FREE_OTABUF() { \
  if ( otaBuf ) \
  { \
    osal_mem_free( otaBuf ); \
  } \
  if ( otaBuf2 ) \
  { \
    SerialApp_SendData( otaBuf2, otaLen2 ); \
    otaBuf2 = NULL; \
  } \
  else \
  { \
    otaBuf = NULL; \
  } \
}

/*********************************************************************
 * CONSTANTS
 */

#if !defined( SERIAL_APP_PORT )
  #define SERIAL_APP_PORT  1
#endif

#if !defined( SERIAL_APP_BAUD )
  // CC2430 only allows 38.4k or 115.2k.
  #define SERIAL_APP_BAUD  HAL_UART_BR_38400
  //#define SERIAL_APP_BAUD  HAL_UART_BR_115200
#endif

// When the Rx buf space is less than this threshold, invoke the Rx callback.
#if !defined( SERIAL_APP_THRESH )
  #define SERIAL_APP_THRESH  48
#endif

#if !defined( SERIAL_APP_RX_MAX )
  #if (defined( HAL_UART_DMA )) && HAL_UART_DMA
    #define SERIAL_APP_RX_MAX  128
  #else
    /* The generic safe Rx minimum is 48, but if you know your PC App will not
     * continue to send more than a byte after receiving the ~CTS, lower max
     * here and safe min in _hal_uart.c to just 8.
     */
    #define SERIAL_APP_RX_MAX  64
  #endif
#endif

#if !defined( SERIAL_APP_TX_MAX )
  #if (defined( HAL_UART_DMA )) && HAL_UART_DMA
  #define SERIAL_APP_TX_MAX  128
  #else
    #define SERIAL_APP_TX_MAX  64
  #endif
#endif

// Millisecs of idle time after a byte is received before invoking Rx callback.
#if !defined( SERIAL_APP_IDLE )
  #define SERIAL_APP_IDLE  6
#endif

// This is the desired byte count per OTA message.
#if !defined( SERIAL_APP_RX_CNT )
  #if (defined( HAL_UART_DMA )) && HAL_UART_DMA
    #define SERIAL_APP_RX_CNT  80
  #else
    #define SERIAL_APP_RX_CNT  6
  #endif
#endif

// Loopback Rx bytes to Tx for thruput testing.
#if !defined( SERIAL_APP_LOOPBACK )
  #define SERIAL_APP_LOOPBACK  FALSE
#endif

#if SERIAL_APP_LOOPBACK
  #define SERIALAPP_TX_RTRY_EVT      0x0010
  #define SERIALAPP_TX_RTRY_TIMEOUT  250
#endif

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * DEBUG
 */
#define _m_debug    1

/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs.
const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] =
{
  GENERICAPP_CLUSTERID
};

const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
{
  GENERICAPP_ENDPOINT,              //  int Endpoint;
  GENERICAPP_PROFID,                //  uint16 AppProfId[2];
  GENERICAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  GENERICAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  GENERICAPP_FLAGS,                 //  int   AppFlags:4;
  GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)GenericApp_ClusterList,  //  byte *pAppInClusterList;
  GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)GenericApp_ClusterList   //  byte *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in GenericApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t GenericApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
byte GenericApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // GenericApp_Init() is called.
devStates_t GenericApp_NwkState;


byte GenericApp_TransID;  // This is the unique message ID (counter)

afAddrType_t GenericApp_DstAddr;



//------------------------------------------------------------------------------
//xiaoyang add, data_trans task
//------------------------------------------------------------------------------
byte data_trans_TaskID;


/*********************************************************************
 * LOCAL FUNCTIONS
 */
void GenericApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
void GenericApp_HandleKeys( byte shift, byte keys );
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void GenericApp_SendTheMessage( void );

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      GenericApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void GenericApp_Init( byte task_id )
{
  GenericApp_TaskID = task_id;
  GenericApp_NwkState = DEV_INIT;
  GenericApp_TransID = 0;
  //xiaoyang add
  //halUARTCfg_t uartConfig;
  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

  GenericApp_DstAddr.addrMode = (afAddrMode_t)AddrNotPresent;
  GenericApp_DstAddr.endPoint = 0;
  GenericApp_DstAddr.addr.shortAddr = 0;

  // Fill out the endpoint description.
  GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;
  GenericApp_epDesc.task_id = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &GenericApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( GenericApp_TaskID );

  // Update the display
#if defined ( LCD_SUPPORTED )
    HalLcdWriteString( "GenericApp", HAL_LCD_LINE_1 );
#endif
    /*
  //xiaoyang add
  uartConfig.configured           = TRUE;              // 2430 don't care.
  uartConfig.baudRate             = SERIAL_APP_BAUD;
  uartConfig.flowControl          = TRUE;
  uartConfig.flowControlThreshold = SERIAL_APP_THRESH;
  uartConfig.rx.maxBufSize        = SERIAL_APP_RX_MAX;
  uartConfig.tx.maxBufSize        = SERIAL_APP_TX_MAX;
  uartConfig.idleTimeout          = SERIAL_APP_IDLE;   // 2430 don't care.
  uartConfig.intEnable            = TRUE;              // 2430 don't care.
#if SERIAL_APP_LOOPBACK
  uartConfig.callBackFunc         = NULL;
#else
  uartConfig.callBackFunc         = NULL;
#endif
  HalUARTOpen (SERIAL_APP_PORT, &uartConfig);
  */
  //
  ZDO_RegisterForZDOMsg( GenericApp_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( GenericApp_TaskID, Match_Desc_rsp );
  
  //xiaoyang ADD@2011.5.26
  //initUART();
  HalUARTWrite(0,"]]\r\n",strlen("]]\r\n"));
}

/*********************************************************************
 * @fn      GenericApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  afDataConfirm_t *afDataConfirm;

  // Data Confirmation message fields
  byte sentEP;
  ZStatus_t sentStatus;
  byte sentTransID;       // This should match the value sent

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          GenericApp_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;

        case KEY_CHANGE:
          GenericApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        case AF_DATA_CONFIRM_CMD:
          // This message is received as a confirmation of a data packet sent.
          // The status is of ZStatus_t type [defined in ZComDef.h]
          // The message fields are defined in AF.h
          afDataConfirm = (afDataConfirm_t *)MSGpkt;
          sentEP = afDataConfirm->endpoint;
          sentStatus = afDataConfirm->hdr.status;
          sentTransID = afDataConfirm->transID;
          (void)sentEP;
          (void)sentTransID;

          // Action taken when confirmation is received.
          if ( sentStatus != ZSuccess )
          {
            // The data wasn't delivered -- Do something
          }
          break;

        case AF_INCOMING_MSG_CMD:
          GenericApp_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
          GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( (GenericApp_NwkState == DEV_ZB_COORD)
              || (GenericApp_NwkState == DEV_ROUTER)
              || (GenericApp_NwkState == DEV_END_DEVICE) )
          { 
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in GenericApp_Init()).
  if ( events & GENERICAPP_SEND_MSG_EVT )
  {
    // Send "the" message
    //GenericApp_SendTheMessage();

    // Setup to send message again
    //osal_start_timerEx( GenericApp_TaskID,
      //                  GENERICAPP_SEND_MSG_EVT,
        //                GENERICAPP_SEND_MSG_TIMEOUT );

    // return unprocessed events
    return (events ^ GENERICAPP_SEND_MSG_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      GenericApp_ProcessZDOMsgs()
 *
 * @brief   Process response messages
 *
 * @param   none
 *
 * @return  none
 */
void GenericApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
      {
        // Light LED
        HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
      }
#if defined(BLINK_LEDS)
      else
      {
        // Flash LED to show failure
        HalLedSet ( HAL_LED_4, HAL_LED_MODE_FLASH );
      }
#endif
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp )
        {
          if ( pRsp->status == ZSuccess && pRsp->cnt )
          {
            GenericApp_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            GenericApp_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            // Take the first endpoint, Can be changed to search through endpoints
            GenericApp_DstAddr.endPoint = pRsp->epList[0];

            // Light LED
            HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
          }
          osal_mem_free( pRsp );
        }
      }
      break;
  }
}

/*********************************************************************
 * @fn      GenericApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_4
 *                 HAL_KEY_SW_3
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void GenericApp_HandleKeys( byte shift, byte keys )
{
  zAddrType_t dstAddr;

  // Shift is used to make each button/switch dual purpose.
  if ( shift )
  {
    if ( keys & HAL_KEY_SW_1 )
    {
    }
    if ( keys & HAL_KEY_SW_2 )
    {
    }
    if ( keys & HAL_KEY_SW_3 )
    {
    }
    if ( keys & HAL_KEY_SW_4 )
    {
    }
  }
  else
  {
    if ( keys & HAL_KEY_SW_1 )
    {
#if _m_debug
      HalUARTWrite(0,"hal sw1 pressed!\r\n",strlen("hal sw1 pressed!\r\n"));
      //uart0_puts("hal sw1 pressed!\r\n",strlen("hal sw1 pressed!\r\n"));
#endif
    }

    if ( keys & HAL_KEY_SW_2 )
    {
      //xiaoyang add
#if _m_debug
       HalUARTWrite(0,"hal sw2 pressed!\r\n",strlen("hal sw2 pressed!\r\n"));
      //uart0_puts("hal sw2 pressed!\r\n",strlen("hal sw2 pressed!\r\n"));
#endif
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );

      // Initiate an End Device Bind Request for the mandatory endpoint
      dstAddr.addrMode = Addr16Bit;
      dstAddr.addr.shortAddr = 0x0000; // Coordinator
      ZDP_EndDeviceBindReq( &dstAddr, NLME_GetShortAddr(),
                            GenericApp_epDesc.endPoint,
                            GENERICAPP_PROFID,
                            GENERICAPP_MAX_CLUSTERS, (cId_t *)GenericApp_ClusterList,
                            GENERICAPP_MAX_CLUSTERS, (cId_t *)GenericApp_ClusterList,
                            FALSE );
    }
    
    //导航按键像下按
    if ( keys & HAL_KEY_SW_3 )
    {
#if _m_debug
      
      char buf[65]="";
      uint16 len = 0;
       HalUARTWrite(0,"1111222233334444\n",strlen("1111222233334444\n"));
       HalUARTWrite(0,"9999222233\r\n",strlen("9999222233\r\n"));
      /*
      HalUARTWrite(0,"#1111#",strlen("1111#\r\n"));
      HalUARTWrite(0,"2222#",strlen("2222#"));
      HalUARTWrite(0,"3333#",strlen("3333#\r\n"));
      buf[64] = '\0';
      HalUARTWrite(0,"4444#",strlen("4444#\r\n"));
      */
      //HalUARTWrite(0,"hal sw3 pressed!\r\n",strlen("hal sw1 pressed!\r\n"));
      //uart0_puts("[[hal sw3 pressed\n",strlen("[[hal sw3 pressed\n")+1);
      //len = uart0_getString (buf,64);
      //HalUARTRead(0,buf, 64);
      //buf[65] = '\0';
      //HalUARTWrite(0,buf,strlen(buf));
      
      //启动任务
      //osal_start_timerEx (data_trans_TaskID, 5, 250); //250ms后执行data_trans_process
#endif
    }

    //导航按键向左按
    if ( keys & HAL_KEY_SW_4 )
    {
#if _m_debug
       HalUARTWrite(0,"hal sw4 pressed!\r\n",strlen("hal sw4 pressed!\r\n"));
     // uart0_puts("hal sw4 pressed!\n\r\n",strlen("hal sw4 pressed!\n\r\n"));
#endif
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );

      // Initiate a Match Description Request (Service Discovery)
      dstAddr.addrMode = AddrBroadcast;
      dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
      ZDP_MatchDescReq( &dstAddr, NWK_BROADCAST_SHORTADDR,
                        GENERICAPP_PROFID,
                        GENERICAPP_MAX_CLUSTERS, (cId_t *)GenericApp_ClusterList,
                        GENERICAPP_MAX_CLUSTERS, (cId_t *)GenericApp_ClusterList,
                        FALSE );
    }
  }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      GenericApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
/*********here i add**************/
typedef struct{
  char temp[5];
  char humidity[5];
  char smoke[5];
  char co[5];
  char ch4[5];
  char address[5];
}info;

info point_info[10];

int point_num;

void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  int i;
  char srcadd[4];
  switch ( pkt->clusterId )
  {
    case GENERICAPP_CLUSTERID:
      // "the" message
/*
#if defined( LCD_SUPPORTED )
      HalLcdWriteScreen( (char*)pkt->cmd.Data, "rcvd" );
#elif defined( WIN32 )
      WPRINTSTR( pkt->cmd.Data );
#endif
      */
      //len = strlen((char *)pkt->cmd.Data);
      //uart0_puts((char*)pkt->cmd.Data, 37);
      if(pkt->cmd.Data[0] == 's')
      {
        srcadd[0] = pkt->cmd.Data[1];
        srcadd[1] = pkt->cmd.Data[2];
        srcadd[2] = pkt->cmd.Data[3];
        srcadd[3] = pkt->cmd.Data[4];
        //uart0_puts((char *)srcadd,4);
        for(i=0;i<point_num;i++)
        {
          //uart0_putc(i+'0');
          if(strcmp(srcadd,point_info[i].address) == 0)
          {
             point_info[i].temp[0] = pkt->cmd.Data[6];
             point_info[i].temp[1] = pkt->cmd.Data[7];
             point_info[i].temp[2] = pkt->cmd.Data[8];
             point_info[i].temp[3] = pkt->cmd.Data[9];
             point_info[i].temp[4] = '\0';
             point_info[i].humidity[0] = pkt->cmd.Data[12];
             point_info[i].humidity[1] = pkt->cmd.Data[13];
             point_info[i].humidity[2] = pkt->cmd.Data[14];
             point_info[i].humidity[3] = pkt->cmd.Data[15];
             point_info[i].humidity[4] = '\0';
             point_info[i].smoke[0] = pkt->cmd.Data[18];
             point_info[i].smoke[1] = pkt->cmd.Data[19];
             point_info[i].smoke[2] = pkt->cmd.Data[20];
             point_info[i].smoke[3] = pkt->cmd.Data[21];
             point_info[i].smoke[4] = '\0';
             point_info[i].co[0] = pkt->cmd.Data[24];
             point_info[i].co[1] = pkt->cmd.Data[25];
             point_info[i].co[2] = pkt->cmd.Data[26];
             point_info[i].co[3] = pkt->cmd.Data[27];
             point_info[i].co[4] = '\0';
             point_info[i].ch4[0] = pkt->cmd.Data[30];
             point_info[i].ch4[1] = pkt->cmd.Data[31];
             point_info[i].ch4[2] = pkt->cmd.Data[32];
             point_info[i].ch4[3] = pkt->cmd.Data[33];
             point_info[i].ch4[3] = '\0';
             break;
          }
        }
        if(i == point_num)
        {
          //uart0_putc(point_num+'0');
          point_info[point_num].address[0] = pkt->cmd.Data[1];
          point_info[point_num].address[1] = pkt->cmd.Data[2];
          point_info[point_num].address[2] = pkt->cmd.Data[3];
          point_info[point_num].address[3] = pkt->cmd.Data[4];
          point_info[point_num].address[4] = '\0';
          point_info[i].temp[0] = pkt->cmd.Data[6];
          point_info[i].temp[1] = pkt->cmd.Data[7];
          point_info[i].temp[2] = pkt->cmd.Data[8];
          point_info[i].temp[3] = pkt->cmd.Data[9];
          point_info[i].temp[4] = '\0';
          point_info[i].humidity[0] = pkt->cmd.Data[12];
             point_info[i].humidity[1] = pkt->cmd.Data[13];
             point_info[i].humidity[2] = pkt->cmd.Data[14];
             point_info[i].humidity[3] = pkt->cmd.Data[15];
             point_info[i].humidity[4] = '\0';
             point_info[i].smoke[0] = pkt->cmd.Data[18];
             point_info[i].smoke[1] = pkt->cmd.Data[19];
             point_info[i].smoke[2] = pkt->cmd.Data[20];
             point_info[i].smoke[3] = pkt->cmd.Data[21];
             point_info[i].smoke[4] = '\0';
             point_info[i].co[0] = pkt->cmd.Data[24];
             point_info[i].co[1] = pkt->cmd.Data[25];
             point_info[i].co[2] = pkt->cmd.Data[26];
             point_info[i].co[3] = pkt->cmd.Data[27];
             point_info[i].co[4] = '\0';
             point_info[i].ch4[0] = pkt->cmd.Data[30];
             point_info[i].ch4[1] = pkt->cmd.Data[31];
             point_info[i].ch4[2] = pkt->cmd.Data[32];
             point_info[i].ch4[3] = pkt->cmd.Data[33];
             point_info[i].ch4[4] = '\0';
             point_num++;
        }
      }
      else
      {
        //uart0_puts(, 31);
      }
      break;
  }
}

/*********************************************************************
 * @fn      GenericApp_SendTheMessage
 *
 * @brief   Send "the" message.
 *
 * @param   none
 *
 * @return  none
 */
void GenericApp_SendTheMessage( void )
{
  char theMessageData[] = "Hello World";

  if ( AF_DataRequest( &GenericApp_DstAddr, &GenericApp_epDesc,
                       GENERICAPP_CLUSTERID,
                       (byte)osal_strlen( theMessageData ) + 1,
                       (byte *)&theMessageData,
                       &GenericApp_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
    // Successfully requested to be sent.
  }
  else
  {
    // Error occurred in request to send.
  }
}

/*********************************************************************
*********************************************************************/
/*********************************************************************
 * TYPEDEFS
 */
#ifndef UART0_GET_STR
#define UART0_GET_STR   1
typedef struct
{
  uint8 *rxBuf;
  uint8 rxHead;
  uint8 rxTail;
  uint8 rxMax;
  uint8 rxCnt;
  uint8 rxTick;
  uint8 rxHigh;

  uint8 *txBuf;
#if HAL_UART_BIG_TX_BUF
  uint16 txHead;
  uint16 txTail;
  uint16 txMax;
  uint16 txCnt;
#else
  uint8 txHead;
  uint8 txTail;
  uint8 txMax;
  uint8 txCnt;
#endif
  uint8 txTick;

  uint8 flag;

  halUARTCBack_t rxCB;
} uartCfg_t;

#if ( HAL_UART_DEBUG )
  #define HAL_UART_ASSERT( expr)        HAL_ASSERT( expr )
#else
  #define HAL_UART_ASSERT( expr )
#endif

#define UART_CFG_U1F  0x80  // USART1 flag bit.
#define UART_CFG_DMA  0x40  // Port is using DMA.
#define UART_CFG_FLW  0x20  // Port is using flow control.
#define UART_CFG_SP4  0x10
#define UART_CFG_SP3  0x08
#define UART_CFG_SP2  0x04
#define UART_CFG_RXF  0x02  // Rx flow is disabled.
#define UART_CFG_TXF  0x01  // Tx is in process.

extern uartCfg_t *cfg0;
/*xiaoyang added*/
uint16 uart0_getString(char* buf,uint16 max_len)
{
  //uint16 HalUARTRead( uint8 port, uint8 *buf, uint16 len );
  uartCfg_t *cfg = NULL;
  uint16 cnt = 0;

  cfg = cfg0;
  HAL_UART_ASSERT( cfg );
  while ( (cfg->rxTail != cfg->rxHead) &&(cnt < max_len) )
  {
    *buf = cfg->rxBuf[cfg->rxTail];
    if ( cfg->rxTail == cfg->rxMax )
    {
      cfg->rxTail = 0;
    }
    else
    {
      cfg->rxTail++;
    }
    //xiaoyang add
    if(*buf == '\0'){
        buf++;
        *buf ='\0' ;
        return cnt;
    }
    buf++;
    cnt++;
  }

#if HAL_UART_DMA
  #if HAL_UART_ISR
  if ( cfg->flag & UART_CFG_DMA )
  #endif
  {
    /* If there is no flow control on a DMA-driven UART, the Rx Head & Tail
     * pointers must be reset to zero after every read in order to preserve the
     * full length of the Rx buffer. This implies that every Read must read all
     * of the Rx bytes available, or the pointers will not be reset and the
     * next incoming packet may not fit in the Rx buffer space remaining - thus
     * the end portion of the incoming packet that does not fit would be lost.
     */
    if ( !(cfg->flag & UART_CFG_FLW) )
    {
      // This is a trick to trigger the DMA abort and restart logic in pollDMA.
      cfg->flag |= UART_CFG_RXF;
    }
  }
#endif

  return cnt;
}
#endif

//------------------------------------------------------------------------------
//xiaoyang add, data_trans task
//------------------------------------------------------------------------------
void data_trans_Init( uint8 task_id ) //任务初始化函数
{
    data_trans_TaskID = task_id;//用data_trans_TaskID全局变量记住自己的任务号，
    SPIMgr_RegisterTaskID( task_id );//注册该任务为串口任务，其实就是App_TaskID = data_trans_TaskID;
}

uint16 data_trans_process( uint8 task_id, uint16 events ) //任务处理函数
{
  int i,len;
  char buf[33]="";
  char srcaddr[5];
  char send_info[40];
  //在串口调试助手中你发什么就收到什么
  len = HalUARTRead(SPI_MGR_DEFAULT_PORT,buf,32 );//读串口数据到buf
  
  if(len >= 10)
  {
    //uart0_puts("len>10",6);
    if(buf[0] == '[' && buf[1] == '[' && buf[2] == 'R' && buf[3] == 'D' && buf[4] == '#')
    {
      //test code
      //HalUARTWrite(0,"#11.1:22.2:33.3:44.4:55.5#\n",strlen("#11.1:22.2:33.3:44.4:55.5#\n"));
      //test code
      
      //收到"[[RD#1234#"
      srcaddr[0] = buf[5];
      srcaddr[1] = buf[6];
      srcaddr[2] = buf[7];
      srcaddr[3] = buf[8];
      srcaddr[4] = '\0';
      
      for(i=0;i<point_num;i++)
      {
        //匹配地址串
        if(strcmp(srcaddr,point_info[i].address)==0)
        {
          sprintf(send_info,"#%s:%s:%s:%s:%s#\n",point_info[i].temp,point_info[i].humidity,point_info[i].smoke,point_info[i].co,point_info[i].ch4);
          //uart0_puts(send_info,28);
          HalUARTWrite(0,send_info,strlen(send_info));
        }
      }
      
      //uart0_puts("]]",2);
      HalUARTWrite(0,"]]\n",strlen("]]\n"));
    }
      
  }
  else if(len >= 4)
  {
    //if(strcmp(buf,"[[RA")==0)
    if( (buf[0] == '[') && (buf[1] == '[') && (buf[2] == 'R') && (buf[3] == 'A') )
    {
      //test code
      /*for(i=0;i<5;i++)
      {
        sprintf(send_info,"#%d#\n",1000+i);
        //uart0_puts(send_info,7);
        HalUARTWrite(0,send_info,strlen(send_info));
      }
      HalUARTWrite(0,"]]\r\n",strlen("]]\r\n"));
      //test code
      */
      for(i=0;i<point_num;i++)
      {
        sprintf(send_info,"#%s#\n\0",point_info[i].address);
        //uart0_puts(send_info,7);
        HalUARTWrite(0,send_info,strlen(send_info));
      }
      HalUARTWrite(0,"]]\r\n",strlen("]]\r\n"));

    }
  }
#if defined(ZAPP_P1)
  SPIMgr_AppFlowControl(SPI_MGR_ZAPP_RX_READY);//恢复流，不然不能接收新数据
#endif
  return 0;   
}

//------------------------------------------------------------------------------
// xiaoyang add @2011-5-25
//------------------------------------------------------------------------------

/******************************************************************************
 * CONSTANTS
 */
//BUFFER buffer;

/******************************************************************************
* @fn  initUART
*
* @brief
*      Initializes components for the UART application example.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void UART_SETUP(char uart,int baudRate,char options)  
{
    if ((options) & FLOW_CONTROL_ENABLE){      
        if(PERCFG & 0x01){ /* Alt 2        */
           P1SEL |= 0x3C;                    
        } else {           /* Alt 1        */
           P0SEL |= 0x3C;                    
        }                                                                         
    }                                          
    else{                    
        if(PERCFG & 0x01){ /* Alt 2        */
           P1SEL |= 0x30;                    
        } else {           /* Alt 1        */
           P0SEL |= 0x0C;                    
        }                                                                                                    
    }   
    
    U0GCR = BAUD_E((baudRate), CLKSPD);
    U0BAUD = BAUD_M(baudRate);
    U0UCR = ((options) | 0x80);
    U0CSR = 0x80;
                             
    if((options) & TRANSFER_MSB_FIRST){        
       U0GCR |= 0x20;                   
    }
}
/*
void initUART(void)
{
   // Setup for UART0
   IO_PER_LOC_USART0_AT_PORT0_PIN2345();

   //设置波特率和传输模式
   UART_SETUP(0, 9600, HIGH_STOP);
   
   UTX0IF = 1;
}
*/
// Global interrupt enables
#define INT_GLOBAL_ENABLE(on) EA=(!!on)

void UART0ISRopen(void)
{
  U0UCR|=0x80;
  IEN0 |= 0x04;//开串口接收中断 'URX0IE = 1',
  INT_GLOBAL_ENABLE(TRUE);
}

void initUART(void)
{
   // Setup for UART0
  // IO_PER_LOC_UART0_AT_PORT0_PIN2345();
   IO_PER_LOC_USART0_AT_PORT0_PIN2345();
   //SET_MAIN_CLOCK_SOURCE(CRYSTAL);
   UART_SETUP(0, 9600, HIGH_STOP);
   U0CSR = 0xC5;//允许接收
   UTX0IF=1;
   URX0IF=1;
   
   UART0ISRopen();
}

#pragma vector=URX0_VECTOR                //URX0_VECTOR
__interrupt void URX0_IRQ(void)
{
}

//-----------------------------------------------------------------------------
// sned str or char via uart0
//-----------------------------------------------------------------------------

int uart0_putc(int c)
{
   if (c == '\n')  {
      while (!UTX0IF);
      UTX0IF = 0;
      U0DBUF = 0x0d;       //output CR 
   }

   while (!UTX0IF);
   UTX0IF = 0;
   return (U0DBUF = c);
}


void uart0_puts(char* str,int len)
{
    int i = 0;
    if(str == NULL)
        return ;
    for(i = 0; i < len; i++)
    {
        uart0_putc(*(str+i));
    }
}