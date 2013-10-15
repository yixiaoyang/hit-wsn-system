/**************************************************************************************************
  Filename:       zcl_lighting.c
  Revised:        $Date: 2006-04-03 16:28:25 -0700 (Mon, 03 Apr 2006) $
  Revision:       $Revision: 10363 $

  Description:    Zigbee Cluster Library -  Lighting


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
#include "zcl_lighting.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */
typedef struct zclLightingCBRec
{
  struct zclLightingCBRec     *next;
  uint8                       endpoint; // Used to link it into the endpoint descriptor
  zclLighting_AppCallbacks_t  *CBs;     // Pointer to Callback function
} zclLightingCBRec_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static zclLightingCBRec_t *zclLightingCBs = (zclLightingCBRec_t *)NULL;
static uint8 zclLightingPluginRegisted = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static ZStatus_t zclLighting_HdlIncoming( zclInHdlrMsg_t *pInHdlrMsg );
static ZStatus_t zclLighting_HdlInSpecificCommands( zclIncoming_t *pInMsg, uint16 logicalClusterID );
static zclLighting_AppCallbacks_t *zclLighting_FindCallbacks( uint8 endpoint );

static ZStatus_t zclLighting_ProcessInColorControlCmds( zclIncoming_t *pInMsg );

static void zclLighting_ProcessInCmd_ColorControl_MoveToHue( zclIncoming_t *pInMsg );
static void zclLighting_ProcessInCmd_ColorControl_MoveHue( zclIncoming_t *pInMsg );
static void zclLighting_ProcessInCmd_ColorControl_StepHue( zclIncoming_t *pInMsg );
static void zclLighting_ProcessInCmd_ColorControl_MoveToSaturation( zclIncoming_t *pInMsg );
static void zclLighting_ProcessInCmd_ColorControl_MoveSaturation( zclIncoming_t *pInMsg );
static void zclLighting_ProcessInCmd_ColorControl_StepSaturation( zclIncoming_t *pInMsg );
static void zclLighting_ProcessInCmd_ColorControl_MoveToHueAndSaturation( zclIncoming_t *pInMsg );


/*********************************************************************
 * @fn      zclLighting_RegisterCmdCallbacks
 *
 * @brief   Register an applications command callbacks
 *
 * @param   endpoint - application's endpoint
 * @param   callbacks - pointer to the callback record.
 *
 * @return  ZMemError if not able to allocate
 */
ZStatus_t zclLighting_RegisterCmdCallbacks( uint8 endpoint, zclLighting_AppCallbacks_t *callbacks )
{
  zclLightingCBRec_t *pNewItem;
  zclLightingCBRec_t *pLoop;

  // Register as a ZCL Plugin
  if ( !zclLightingPluginRegisted )
  {
    zcl_registerPlugin( ZCL_LIGHTING_LOGICAL_CLUSTER_ID_COLOR_CONTROL,
                        ZCL_LIGHTING_LOGICAL_CLUSTER_ID_BALLAST_CONFIG,
                        zclLighting_HdlIncoming );
    zclLightingPluginRegisted = TRUE;
  }

  // Fill in the new profile list
  pNewItem = osal_mem_alloc( sizeof( zclLightingCBRec_t ) );
  if ( pNewItem == NULL )
    return (ZMemError);

  pNewItem->next = (zclLightingCBRec_t *)NULL;
  pNewItem->endpoint = endpoint;
  pNewItem->CBs = callbacks;

  // Find spot in list
  if ( zclLightingCBs == NULL )
  {
    zclLightingCBs = pNewItem;
  }
  else
  {
    // Look for end of list
    pLoop = zclLightingCBs;
    while ( pLoop->next != NULL )
      pLoop = pLoop->next;

    // Put new item at end of list
    pLoop->next = pNewItem;
  }
  
  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveToHueCmd
 *
 * @brief   Call to send out a Move To Hue Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   hue - target hue value
 * @param   direction - direction of hue change
 * @param   transitionTime - tame taken to move to the target hue in 1/10 sec increments
 *
 * @return  ZStatus_t
 */
ZStatus_t zclLighting_ColorControl_Send_MoveToHueCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                      uint8 hue, uint8 direction, uint16 transitionTime, 
                                                      uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[4];
  
  buf[0] = hue;
  buf[1] = direction;
  buf[2] = LO_UINT16( transitionTime );
  buf[3] = HI_UINT16( transitionTime );

  return zcl_SendCommand( srcEP, dstAddr, ZCL_LIGHTING_LOGICAL_CLUSTER_ID_COLOR_CONTROL,
                          TRUE, COMMAND_LIGHTING_MOVE_TO_HUE, TRUE, 
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 4, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveHueCmd
 *
 * @brief   Call to send out a Move To Hue Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   moveMode - LIGHTING_MOVE_HUE_STOP, LIGHTING_MOVE_HUE_UP,
 *                     LIGHTING_MOVE_HUE_DOWN
 * @param   rate - the movement in steps per second, where step is
 *                 a change in the device's hue of one unit
 * @return  ZStatus_t
 */
ZStatus_t zclLighting_ColorControl_Send_MoveHueCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                    uint8 moveMode, uint8 rate, 
                                                    uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[2];
  
  buf[0] = moveMode;
  buf[1] = rate;

  return zcl_SendCommand( srcEP, dstAddr, ZCL_LIGHTING_LOGICAL_CLUSTER_ID_COLOR_CONTROL,
                          TRUE, COMMAND_LIGHTING_MOVE_HUE, TRUE, 
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 2, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_StepHueCmd
 *
 * @brief   Call to send out a Step Hue Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   stepMode -	LIGHTING_STEP_HUE_UP, LIGHTING_STEP_HUE_DOWN
 * @param   transitionTime - the movement in steps per 1/10 second
 *
 * @return  ZStatus_t
 */
ZStatus_t zclLighting_ColorControl_Send_StepHueCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                    uint8 stepMode, uint8 transitionTime, 
                                                    uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[2];
  
  buf[0] = stepMode;
  buf[1] = transitionTime;

  return zcl_SendCommand( srcEP, dstAddr, ZCL_LIGHTING_LOGICAL_CLUSTER_ID_COLOR_CONTROL,
                          TRUE, COMMAND_LIGHTING_STEP_HUE, TRUE, 
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 2, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveToSaturationCmd
 *
 * @brief   Call to send out a Move To Saturation Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   saturation - target saturation value
 * @param   transitionTime - time taken move to the target saturation,
 *                           in 1/10 second units
 *
 * @return  ZStatus_t
 */
ZStatus_t zclLighting_ColorControl_Send_MoveToSaturationCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                         uint8 saturation, uint16 transitionTime, 
                                         uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[3];
  
  buf[0] = saturation;
  buf[1] = LO_UINT16( transitionTime );
  buf[2] = HI_UINT16( transitionTime );

  return zcl_SendCommand( srcEP, dstAddr, ZCL_LIGHTING_LOGICAL_CLUSTER_ID_COLOR_CONTROL, 
                          TRUE, COMMAND_LIGHTING_MOVE_TO_SATURATION, TRUE, 
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 3, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveSaturationCmd
 *
 * @brief   Call to send out a Move Saturation Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   moveMode - LIGHTING_MOVE_SATURATION_STOP, LIGHTING_MOVE_SATURATION_UP,
 *                     LIGHTING_MOVE_SATURATION_DOWN
 * @param   rate - rate of movement in step/sec; step is the device's saturation of one unit
 *
 * @return  ZStatus_t
 */
ZStatus_t zclLighting_ColorControl_Send_MoveSaturationCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                           uint8 moveMode, uint8 rate, 
                                                           uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[2];
  
  buf[0] = moveMode;
  buf[1] = rate;

  return zcl_SendCommand( srcEP, dstAddr, ZCL_LIGHTING_LOGICAL_CLUSTER_ID_COLOR_CONTROL, 
                          TRUE, COMMAND_LIGHTING_MOVE_SATURATION, TRUE, 
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 2, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_StepSaturationCmd
 *
 * @brief   Call to send out a Step Saturation Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   stepMode -  LIGHTING_STEP_SATURATION_UP, LIGHTING_STEP_SATURATION_DOWN
 * @param   transitionTime - time to perform a single step in 1/10 of second
 *
 * @return  ZStatus_t
 */
ZStatus_t zclLighting_ColorControl_Send_StepSaturationCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                           uint8 stepMode, uint8 transitionTime, 
                                                           uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[2];
  
  buf[0] = stepMode;
  buf[1] = transitionTime;
  
  return zcl_SendCommand( srcEP, dstAddr, ZCL_LIGHTING_LOGICAL_CLUSTER_ID_COLOR_CONTROL,
                          TRUE, COMMAND_LIGHTING_STEP_SATURATION, TRUE, 
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 2, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveToHueAndSaturationCmd
 *
 * @brief   Call to send out a Move To Hue And Saturation Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   hue - a target hue
 * @param   saturation - a target saturation
 * @param   transitionTime -  time to move, equal of the value of the field in 1/10 seconds
 *
 * @return  ZStatus_t
 */
ZStatus_t zclLighting_ColorControl_Send_MoveToHueAndSaturationCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                   uint8 hue, uint8 saturation, uint16 transitionTime, 
                                                   uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[4];
  
  buf[0] = hue;
  buf[1] = saturation;
  buf[2] = LO_UINT16( transitionTime );
  buf[3] = HI_UINT16( transitionTime );

  return zcl_SendCommand( srcEP, dstAddr, ZCL_LIGHTING_LOGICAL_CLUSTER_ID_COLOR_CONTROL,
                          TRUE, COMMAND_LIGHTING_MOVE_TO_HUE_AND_SATURATION, TRUE, 
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 4, buf );
}

/*********************************************************************
 * @fn      zclLighting_FindCallbacks
 *
 * @brief   Find the callbacks for an endpoint
 *
 * @param   endpoint - 
 *
 * @return  pointer to the callbacks
 */
static zclLighting_AppCallbacks_t *zclLighting_FindCallbacks( uint8 endpoint )
{
  zclLightingCBRec_t *pCBs;
  
  pCBs = zclLightingCBs;
  while ( pCBs )
  {
    if ( pCBs->endpoint == endpoint )
      return ( pCBs->CBs );
  }
  return ( (zclLighting_AppCallbacks_t *)NULL );
}

/*********************************************************************
 * @fn      zclLighting_HdlIncoming
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library or Profile commands for attributes
 *          that aren't in the attribute list
 *
 * @param   pInMsg - pointer to the incoming message
 * @param   logicalClusterID - 
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclLighting_HdlIncoming( zclInHdlrMsg_t *pInHdlrMsg )
{
  ZStatus_t stat = ZSuccess;

  if ( zcl_ClusterCmd( pInHdlrMsg->msg->hdr.fc.type ) )
  {
    // Is this a manufacturer specific command?
    if ( pInHdlrMsg->msg->hdr.fc.manuSpecific == 0 ) 
    {
      stat = zclLighting_HdlInSpecificCommands( pInHdlrMsg->msg, pInHdlrMsg->logicalClusterID );
    }
    else
    {
      // We don't support any manufacturer specific command.
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
 * @fn      zclLighting_HdlInSpecificCommands
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library

 * @param   pInMsg - pointer to the incoming message
 * @param   logicalClusterID - 
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclLighting_HdlInSpecificCommands( zclIncoming_t *pInMsg, uint16 logicalClusterID )
{
  ZStatus_t stat = ZSuccess;

  switch ( logicalClusterID )				
  {
    case ZCL_LIGHTING_LOGICAL_CLUSTER_ID_COLOR_CONTROL:
      stat = zclLighting_ProcessInColorControlCmds( pInMsg );
      break;

    case ZCL_LIGHTING_LOGICAL_CLUSTER_ID_BALLAST_CONFIG:
      // no commands
    default:
      stat = ZFailure;
      break;
  }

  return ( stat );
}

/*********************************************************************
 * @fn      zclLighting_ProcessInColorControlCmds
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library on a command ID basis

 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclLighting_ProcessInColorControlCmds( zclIncoming_t *pInMsg )
{
  ZStatus_t stat = ZSuccess;

  switch ( pInMsg->hdr.commandID )				
  {
    case COMMAND_LIGHTING_MOVE_TO_HUE:
      zclLighting_ProcessInCmd_ColorControl_MoveToHue( pInMsg );
      break;

    case COMMAND_LIGHTING_MOVE_HUE:
      zclLighting_ProcessInCmd_ColorControl_MoveHue( pInMsg );
      break;

    case COMMAND_LIGHTING_STEP_HUE:
      zclLighting_ProcessInCmd_ColorControl_StepHue( pInMsg );
      break;

    case COMMAND_LIGHTING_MOVE_TO_SATURATION:
      zclLighting_ProcessInCmd_ColorControl_MoveToSaturation( pInMsg );
      break;

    case COMMAND_LIGHTING_MOVE_SATURATION:
      zclLighting_ProcessInCmd_ColorControl_MoveSaturation( pInMsg );
      break;

    case COMMAND_LIGHTING_STEP_SATURATION:
      zclLighting_ProcessInCmd_ColorControl_StepSaturation( pInMsg );
      break;

    case COMMAND_LIGHTING_MOVE_TO_HUE_AND_SATURATION:
      zclLighting_ProcessInCmd_ColorControl_MoveToHueAndSaturation( pInMsg );
      break;

    default:
      stat = ZFailure;
      break;
  }

  return ( stat );
}

/*********************************************************************
 * @fn      zclLighting_ProcessInCmd_ColorControl_MoveToHue
 *
 * @brief   Process in the received Move To Hue Command.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 */
static void zclLighting_ProcessInCmd_ColorControl_MoveToHue( zclIncoming_t *pInMsg )
{
  zclLighting_AppCallbacks_t *pCBs;

  if  ( pInMsg->hdr.commandID != COMMAND_LIGHTING_MOVE_TO_HUE )
    return;   // Error ignore the command

  pCBs = zclLighting_FindCallbacks( pInMsg->msg->endPoint );
  if ( pCBs && pCBs->pfnColorControl_MoveToHue )
  {
    zclCCMoveToHue_t cmd;
    
    cmd.hue = pInMsg->pData[0];
    cmd.direction = pInMsg->pData[1];
    cmd.transitionTime = BUILD_UINT16( pInMsg->pData[2], pInMsg->pData[3] );
        
    pCBs->pfnColorControl_MoveToHue( &cmd );
  }
}

/*********************************************************************
 * @fn      zclLighting_ProcessInCmd_ColorControl_MoveHue
 *
 * @brief   Process in the received Move Hue Command.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 */
static void zclLighting_ProcessInCmd_ColorControl_MoveHue( zclIncoming_t *pInMsg )
{
  zclCCMoveHue_t cmd;
  zclLighting_AppCallbacks_t *pCBs;
  zclDefaultRspCmd_t defaultRspCmd;

  if  ( pInMsg->hdr.commandID != COMMAND_LIGHTING_MOVE_HUE )
    return;   // Error ignore the command

  cmd.moveMode = pInMsg->pData[0];
  cmd.rate = pInMsg->pData[1];

  if ( cmd.rate > 0 )
  {
    pCBs = zclLighting_FindCallbacks( pInMsg->msg->endPoint );
    if ( pCBs && pCBs->pfnColorControl_MoveHue )
      pCBs->pfnColorControl_MoveHue( &cmd );
  }
  else
  {
    // Send a Default Response command back
    defaultRspCmd.commandID = pInMsg->hdr.commandID;
    defaultRspCmd.statusCode = ZCL_STATUS_INVALID_FIELD;
    zcl_SendDefaultRspCmd( pInMsg->msg->endPoint, &(pInMsg->msg->srcAddr),
                           pInMsg->msg->clusterId, &defaultRspCmd,
                           ZCL_FRAME_SERVER_CLIENT_DIR, true, pInMsg->hdr.transSeqNum );
  }
}

/*********************************************************************
 * @fn      zclLighting_ProcessInCmd_ColorControl_StepHue
 *
 * @brief   Process in the received Step Hue Command.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 */
static void zclLighting_ProcessInCmd_ColorControl_StepHue( zclIncoming_t *pInMsg )
{
  zclLighting_AppCallbacks_t *pCBs;

  if  ( pInMsg->hdr.commandID != COMMAND_LIGHTING_STEP_HUE )
    return;   // Error ignore the command

  pCBs = zclLighting_FindCallbacks( pInMsg->msg->endPoint );
  if ( pCBs && pCBs->pfnColorControl_StepHue )
  {
    zclCCStepHue_t cmd;
    
    cmd.stepMode = pInMsg->pData[0];
    cmd.transitionTime = pInMsg->pData[1];
        
    pCBs->pfnColorControl_StepHue( &cmd );
  }
}

/*********************************************************************
 * @fn      zclLighting_ProcessInCmd_ColorControl_MoveToSaturation
 *
 * @brief   Process in the received Move to Saturation Command.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 */
static void zclLighting_ProcessInCmd_ColorControl_MoveToSaturation( zclIncoming_t *pInMsg )
{
  zclLighting_AppCallbacks_t *pCBs;

  if  ( pInMsg->hdr.commandID != COMMAND_LIGHTING_MOVE_TO_SATURATION )
    return;   // Error ignore the command

  pCBs = zclLighting_FindCallbacks( pInMsg->msg->endPoint );
  if ( pCBs && pCBs->pfnColorControl_MoveToSaturation )
  {
    zclCCMoveToSaturation_t cmd;
    
    cmd.saturation = pInMsg->pData[0];
    cmd.transitionTime = BUILD_UINT16( pInMsg->pData[1], pInMsg->pData[2] );
    
    pCBs->pfnColorControl_MoveToSaturation( &cmd );
  }
}

/*********************************************************************
 * @fn      zclLighting_ProcessInCmd_ColorControl_MoveSaturation
 *
 * @brief   Process in the received Move Saturation Command.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 */
static void zclLighting_ProcessInCmd_ColorControl_MoveSaturation( zclIncoming_t *pInMsg )
{
  zclCCMoveSaturation_t cmd;
  zclLighting_AppCallbacks_t *pCBs;
  zclDefaultRspCmd_t defaultRspCmd;

  if  ( pInMsg->hdr.commandID != COMMAND_LIGHTING_MOVE_SATURATION )
    return;   // Error ignore the command

  cmd.moveMode = pInMsg->pData[0];
  cmd.rate = pInMsg->pData[1];
  
  if ( cmd.rate > 0 )
  {
    pCBs = zclLighting_FindCallbacks( pInMsg->msg->endPoint );
    if ( pCBs && pCBs->pfnColorControl_MoveSaturation )
      pCBs->pfnColorControl_MoveSaturation( &cmd );
  }
  else
  {
    // Send a Default Response command back
    defaultRspCmd.commandID = pInMsg->hdr.commandID;
    defaultRspCmd.statusCode = ZCL_STATUS_INVALID_FIELD;
    zcl_SendDefaultRspCmd( pInMsg->msg->endPoint, &(pInMsg->msg->srcAddr),
                           pInMsg->msg->clusterId, &defaultRspCmd,
                           ZCL_FRAME_SERVER_CLIENT_DIR, true, pInMsg->hdr.transSeqNum );
  }
}

/*********************************************************************
 * @fn      zclLighting_ProcessInCmd_ColorControl_StepSaturation
 *
 * @brief   Process in the received Step Saturation Command.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 */
static void zclLighting_ProcessInCmd_ColorControl_StepSaturation( zclIncoming_t *pInMsg )
{
  zclLighting_AppCallbacks_t *pCBs;

  if  ( pInMsg->hdr.commandID != COMMAND_LIGHTING_STEP_SATURATION )
    return;   // Error ignore the command

  pCBs = zclLighting_FindCallbacks( pInMsg->msg->endPoint );
  if ( pCBs && pCBs->pfnColorControl_StepSaturation )
  { 
    zclCCStepSaturation_t cmd;
    
    cmd.stepMode = pInMsg->pData[0];
    cmd.transitionTime = pInMsg->pData[1];
    
    pCBs->pfnColorControl_StepSaturation( &cmd );
  }
}

/*********************************************************************
 * @fn      zclLighting_ProcessInCmd_ColorControl_MoveToHueAndSaturation
 *
 * @brief   Process in the received Move To Hue And Saturation Command.
 *
 * @param   pInMsg - pointer to the incoming message
 *
 */
static void zclLighting_ProcessInCmd_ColorControl_MoveToHueAndSaturation( zclIncoming_t *pInMsg )
{
  zclLighting_AppCallbacks_t *pCBs;

  if  ( pInMsg->hdr.commandID != COMMAND_LIGHTING_MOVE_TO_HUE_AND_SATURATION )
    return;   // Error ignore the command

  pCBs = zclLighting_FindCallbacks( pInMsg->msg->endPoint );
  if ( pCBs && pCBs->pfnColorControl_MoveToHueAndSaturation )
  {
    zclCCMoveToHueAndSaturation_t cmd;
    
    cmd.hue = pInMsg->pData[0];
    cmd.saturation = pInMsg->pData[1];   
    cmd.transitionTime = BUILD_UINT16( pInMsg->pData[2], pInMsg->pData[3] );
    
    pCBs->pfnColorControl_MoveToHueAndSaturation( &cmd );
  }
}


/****************************************************************************
****************************************************************************/
