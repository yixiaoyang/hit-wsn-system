/**************************************************************************************************
  Filename:       zcl_hvac.c
  Revised:        $Date: 2006-04-03 16:28:25 -0700 (Mon, 03 Apr 2006) $
  Revision:       $Revision: 10363 $

  Description:    Zigbee Cluster Library - HVAC


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


/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "zcl.h"
#include "zcl_general.h"
#include "zcl_hvac.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */
typedef struct zclHVACCBRec
{
  struct zclHVACCBRec     *next;
  uint8                   endpoint; // Used to link it into the endpoint descriptor
  zclHVAC_AppCallbacks_t  *CBs;     // Pointer to Callback function
} zclHVACCBRec_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static zclHVACCBRec_t *zclHVACCBs = (zclHVACCBRec_t *)NULL;
static uint8 zclHVACPluginRegisted = FALSE;


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static ZStatus_t zclHVAC_HdlIncoming( zclInHdlrMsg_t *pInHdlrMsg );
static ZStatus_t zclHVAC_HdlInSpecificCommands( zclIncoming_t *pInMsg, uint16 logicalClusterID );
static zclHVAC_AppCallbacks_t *zclHVAC_FindCallbacks( uint8 endpoint );

static ZStatus_t zclHVAC_ProcessInPumpCmds( zclIncoming_t *pInMsg );
static ZStatus_t zclHVAC_ProcessInThermostatCmds( zclIncoming_t *pInMsg );

/*********************************************************************
 * @fn      zclHVAC_RegisterCmdCallbacks
 *
 * @brief   Register an applications command callbacks
 *
 * @param   endpoint - application's endpoint
 * @param   callbacks - pointer to the callback record.
 *
 * @return  ZMemError if not able to allocate
 */
ZStatus_t zclHVAC_RegisterCmdCallbacks( uint8 endpoint, zclHVAC_AppCallbacks_t *callbacks )
{
  zclHVACCBRec_t *pNewItem;
  zclHVACCBRec_t *pLoop;

  // Register as a ZCL Plugin
  if ( !zclHVACPluginRegisted )
  {
    zcl_registerPlugin( ZCL_HVAC_LOGICAL_CLUSTER_ID_PUMP_CONFIG_CONTROL,
                        ZCL_HVAC_LOGICAL_CLUSTER_ID_USER_INTERFACE_CONFIG,
                        zclHVAC_HdlIncoming );
    zclHVACPluginRegisted = TRUE;
  }

  // Fill in the new profile list
  pNewItem = osal_mem_alloc( sizeof( zclHVACCBRec_t ) );
  if ( pNewItem == NULL )
    return (ZMemError);

  pNewItem->next = (zclHVACCBRec_t *)NULL;
  pNewItem->endpoint = endpoint;
  pNewItem->CBs = callbacks;

  // Find spot in list
  if ( zclHVACCBs == NULL )
  {
    zclHVACCBs = pNewItem;
  }
  else
  {
    // Look for end of list
    pLoop = zclHVACCBs;
    while ( pLoop->next != NULL )
      pLoop = pLoop->next;

    // Put new item at end of list
    pLoop->next = pNewItem;
  }
  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclHVAC_SendSetpointRaiseLower
 *
 * @brief   Call to send out a Setpoint Raise/Lower Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   mode - which setpoint is to be configured
 * @param   amount - amount setpoint(s) are to be increased (or decreased) by
 * @param   seqNum - transaction sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclHVAC_SendSetpointRaiseLower( uint8 srcEP, afAddrType_t *dstAddr,
                                          uint8 mode, uint8 amount, 
                                          uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[2];

  buf[0] = mode;
  buf[1] = amount;

  return zcl_SendCommand( srcEP, dstAddr, ZCL_HVAC_LOGICAL_CLUSTER_ID_PUMP_CONFIG_CONTROL,
                          TRUE, COMMAND_THERMOSTAT_SETPOINT_RAISE_LOWER, TRUE, 
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 2, buf );
}

/*********************************************************************
 * @fn      zclHVAC_FindCallbacks
 *
 * @brief   Find the callbacks for an endpoint
 *
 * @param   endpoint
 *
 * @return  pointer to the callbacks
 */
static zclHVAC_AppCallbacks_t *zclHVAC_FindCallbacks( uint8 endpoint )
{
  zclHVACCBRec_t *pCBs;
  pCBs = zclHVACCBs;
  while ( pCBs )
  {
    if ( pCBs->endpoint == endpoint )
      return ( pCBs->CBs );
  }
  return ( (zclHVAC_AppCallbacks_t *)NULL );
}

/*********************************************************************
 * @fn      zclHVAC_HdlIncoming
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library or Profile commands for attributes
 *          that aren't in the attribute list
 *
 * @param   pInMsg - pointer to the incoming message
 * @param   logicalClusterID
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclHVAC_HdlIncoming( zclInHdlrMsg_t *pInHdlrMsg )
{
  ZStatus_t stat = ZSuccess;

  if ( zcl_ClusterCmd( pInHdlrMsg->msg->hdr.fc.type ) )
  {
    // Is this a manufacturer specific command?
    if ( pInHdlrMsg->msg->hdr.fc.manuSpecific == 0 ) 
    {
      stat = zclHVAC_HdlInSpecificCommands( pInHdlrMsg->msg, pInHdlrMsg->logicalClusterID );
    }
    else
    {
      // We don't support any manufacturer specific command -- ignore it.
      stat = ZFailure;
    }
  }
  else
  {
    // Handle all the normal (Read, Write...) commands -- should never get here
    stat = ZFailure;
  }
  return ( stat );
}

/*********************************************************************
 * @fn      zclHVAC_HdlInSpecificCommands
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library

 * @param   pInMsg - pointer to the incoming message
 * @param   logicalClusterID
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclHVAC_HdlInSpecificCommands( zclIncoming_t *pInMsg, uint16 logicalClusterID )
{
  ZStatus_t stat = ZSuccess;

  switch ( logicalClusterID )				
  {
    case ZCL_HVAC_LOGICAL_CLUSTER_ID_PUMP_CONFIG_CONTROL:
      stat = zclHVAC_ProcessInPumpCmds( pInMsg );
      break;

    case ZCL_HVAC_LOGICAL_CLUSTER_ID_THERMOSTAT:
      stat = zclHVAC_ProcessInThermostatCmds( pInMsg );
      break;
 
    default:
      stat = ZFailure;
      break;
  }

  return ( stat );
}

/*********************************************************************
 * @fn      zclHVAC_ProcessInPumpCmds
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library on a command ID basis

 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclHVAC_ProcessInPumpCmds( zclIncoming_t *pInMsg )
{
  ZStatus_t stat = ZFailure;

  // there are no specific command for this cluster yet.
  // instead of suppressing a compiler warnings( for a code porting reasons )
  // fake unused call here and keep the code skeleton intact
  if ( stat != ZFailure )
    zclHVAC_FindCallbacks( 0 );

  return ( stat );
}

/*********************************************************************
 * @fn      zclHVAC_ProcessInThermostatCmds
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library on a command ID basis

 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclHVAC_ProcessInThermostatCmds( zclIncoming_t *pInMsg )
{
  zclHVAC_AppCallbacks_t *pCBs;

  if  ( pInMsg->hdr.commandID != COMMAND_THERMOSTAT_SETPOINT_RAISE_LOWER )
    return (ZFailure);   // Error ignore the command

  pCBs = zclHVAC_FindCallbacks( pInMsg->msg->endPoint );
  if ( pCBs && pCBs->pfnHVAC_SetpointRaiseLower )
  {
    zclCmdThermostatSetpointRaiseLowerPayload_t cmd;
    
    cmd.mode = pInMsg->pData[0];
    cmd.amount = pInMsg->pData[1];
    
    pCBs->pfnHVAC_SetpointRaiseLower( &cmd );
  }
  
  return ( ZSuccess );
}

/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */


/****************************************************************************
****************************************************************************/

