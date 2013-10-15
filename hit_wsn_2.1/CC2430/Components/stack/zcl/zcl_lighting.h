/**************************************************************************************************
  Filename:       zcl_lighting.h
  Revised:        $Date: 2006-04-03 16:27:20 -0700 (Mon, 03 Apr 2006) $
  Revision:       $Revision: 10362 $

  Description:    This file contains the ZCL Lighting library definitions.


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

#ifndef ZCL_LIGHTING_H
#define ZCL_LIGHTING_H

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 * INCLUDES
 */
#include "zcl.h"

/******************************************************************************
 * CONSTANTS
 */
					
/*****************************************/
/***  Color Control Cluster Attributes ***/
/*****************************************/
  // Color Information attributes set
#define ATTRID_LIGHTING_COLOR_CONTROL_CURRENT_HUE                        0x0000
#define ATTRID_LIGHTING_COLOR_CONTROL_CURRENT_SATURATION                 0x0001
#define ATTRID_LIGHTING_COLOR_CONTROL_REMAINING_TIME                     0x0002
  // Color Settings attributes set: currently none
  // commands:
#define COMMAND_LIGHTING_MOVE_TO_HUE                                     0x00
#define COMMAND_LIGHTING_MOVE_HUE                                        0x01
#define COMMAND_LIGHTING_STEP_HUE                                        0x02
#define COMMAND_LIGHTING_MOVE_TO_SATURATION                              0x03
#define COMMAND_LIGHTING_MOVE_SATURATION                                 0x04
#define COMMAND_LIGHTING_STEP_SATURATION                                 0x05
#define COMMAND_LIGHTING_MOVE_TO_HUE_AND_SATURATION                      0x06
  /***  Move To Hue Cmd payload: direction field values  ***/
#define LIGHTING_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE                 0x00
#define LIGHTING_MOVE_TO_HUE_DIRECTION_LONGEST_DISTANCE                  0x01
#define LIGHTING_MOVE_TO_HUE_DIRECTION_UP                                0x02
#define LIGHTING_MOVE_TO_HUE_DIRECTION_DOWN                              0x03
  /***  Move Hue Cmd payload: moveMode field values   ***/
#define LIGHTING_MOVE_HUE_STOP                                           0x00
#define LIGHTING_MOVE_HUE_UP                                             0x01
#define LIGHTING_MOVE_HUE_DOWN                                           0x03
  /***  Step Hue Cmd payload: stepMode field values ***/
#define LIGHTING_STEP_HUE_UP                                             0x01
#define LIGHTING_STEP_HUE_DOWN                                           0x03
  /***  Move Saturation Cmd payload: moveMode field values ***/
#define LIGHTING_MOVE_SATURATION_STOP                                    0x00
#define LIGHTING_MOVE_SATURATION_UP                                      0x01
#define LIGHTING_MOVE_SATURATION_DOWN                                    0x03
  /***  Step Saturation Cmd payload: stepMode field values ***/
#define LIGHTING_STEP_SATURATION_UP                                      0x01
#define LIGHTING_STEP_SATURATION_DOWN                                    0x03

/*****************************************************************************/
/***          Ballast Configuration Cluster Attributes                     ***/
/*****************************************************************************/
  // Ballast Information attribute set
#define ATTRID_LIGHTING_BALLAST_CONFIG_PHYSICAL_MIN_LEVEL                0x0000
#define ATTRID_LIGHTING_BALLAST_CONFIG_PHYSICAL_MAX_LEVEL                0x0001
#define ATTRID_LIGHTING_BALLAST_BALLAST_STATUS                           0x0002
/*** Ballast Status Attribute values (by bit number) ***/
#define LIGHTING_BALLAST_STATUS_NON_OPERATIONAL                          1 // bit 0 is set
#define LIGHTING_BALLAST_STATUS_LAMP_IS_NOT_IN_SOCKET                    2 // bit 1 is set
  // Ballast Settings attributes set
#define ATTRID_LIGHTING_BALLAST_MIN_LEVEL                                0x0010
#define ATTRID_LIGHTING_BALLAST_MAX_LEVEL                                0x0011
#define ATTRID_LIGHTING_BALLAST_POWER_ON_LEVEL                           0x0012
#define ATTRID_LIGHTING_BALLAST_POWER_ON_FADE_TIME                       0x0013
#define ATTRID_LIGHTING_BALLAST_INTRISTIC_BALLAST_FACTOR                 0x0014
#define ATTRID_LIGHTING_BALLAST_BALLAST_FACTOR_ADJUSTMENT                0x0015
  // Lamp Information attributes set
#define ATTRID_LIGHTING_BALLAST_LAMP_QUANTITY                            0x0020
  // Lamp Settings attributes set
#define ATTRID_LIGHTING_BALLAST_LAMP_TYPE                                0x0030
#define ATTRID_LIGHTING_BALLAST_LAMP_MANUFACTURER                        0x0031
#define ATTRID_LIGHTING_BALLAST_LAMP_RATED_HOURS                         0x0032
#define ATTRID_LIGHTING_BALLAST_LAMP_BURN_HOURS                          0x0033
#define ATTRID_LIGHTING_BALLAST_LAMP_ALARM_MODE                          0x0034
#define ATTRID_LIGHTING_BALLAST_LAMP_BURN_HOURS_TRIP_POINT               0x0035
/*** Lamp Alarm Mode attribute values  ***/
#define LIGHTING_BALLAST_LAMP_ALARM_MODE_BIT_0_NO_ALARM                  0
#define LIGHTING_BALLAST_LAMP_ALARM_MODE_BIT_0_ALARM                     1


/******************************************************************************/
/***            Logical Cluster ID - for mapping only                       ***/
/***           These are not to be used over-the-air                        ***/
/******************************************************************************/
#define ZCL_LIGHTING_LOGICAL_CLUSTER_ID_COLOR_CONTROL                    0x0030
#define ZCL_LIGHTING_LOGICAL_CLUSTER_ID_BALLAST_CONFIG                   0x0031


/*******************************************************************************
 * TYPEDEFS
 */

/*** ZCL Color Control Cluster: Move To Hue Cmd payload ***/
typedef struct
{
  uint8  hue;
  uint8  direction;
  uint16 transitionTime;
} zclCmdLightingMoveToHuePayload_t;

/*** ZCL Color Control Cluster: Move Hue Cmd payload ***/
typedef struct
{
  uint8 moveMode;
  uint8 rate;
} zclCmdLightingMoveHuePayload_t;

/*** ZCL Color Control Cluster: Step Hue Cmd payload ***/
typedef struct
{
  uint8 stepMode;
  uint8 transitionTime;
} zclCmdLightingStepHuePayload_t;

/*** ZCL Color Control Cluster: Move to Saturation Cmd payload ***/
typedef struct
{
  uint8  saturation;
  uint16 transitionTime;
} zclCmdLightingMoveToSaturationPayload_t;

/*** ZCL Color Control Cluster: Move Saturation Cmd payload ***/
typedef struct
{
  uint8 moveMode;
  uint8 rate;
} zclCmdLightingMoveSaturationPayload_t;

/*** ZCL Color Control Cluster: Step Saturation Cmd payload ***/
typedef struct
{
  uint8 stepMode;
  uint8 transitionTime;
} zclCmdLightingStepSaturationPayload_t;

/*** ZCL Color Control Cluster: Move To Hue and Saturation Cmd payload ***/
typedef struct
{
  uint8  hue;
  uint8  saturation;
  uint16 transitionTime;
} zclCmdLightingMoveToHueAndSaturationPayload_t;

/*** Structures used for callback functions ***/

typedef struct
{
  uint8  hue;            // target hue value
  uint8  direction;      // direction of change
  uint16 transitionTime; // tame taken to move to the target hue in 1/10 sec increments
} zclCCMoveToHue_t;

typedef struct
{
  uint8 moveMode; // LIGHTING_MOVE_HUE_STOP, LIGHTING_MOVE_HUE_UP, LIGHTING_MOVE_HUE_DOWN
  uint8 rate;     // the movement in steps per second, where step is a change in the device's hue of one unit
} zclCCMoveHue_t;

typedef struct
{
  uint8 stepMode;       // LIGHTING_STEP_HUE_UP, LIGHTING_STEP_HUE_DOWN
  uint8 transitionTime; // the movement in steps per 1/10 second
} zclCCStepHue_t;

typedef struct
{
  uint8 saturation;      // target saturation value
  uint16 transitionTime; // time taken move to the target saturation, in 1/10 second units
} zclCCMoveToSaturation_t;

typedef struct
{
  uint8 moveMode; // LIGHTING_MOVE_SATURATION_STOP, LIGHTING_MOVE_SATURATION_UP,
                  // LIGHTING_MOVE_SATURATION_DOWN
  uint8 rate;     // rate of movement in step/sec; step is the device's saturation of one unit
} zclCCMoveSaturation_t;

typedef struct
{
  uint8 stepMode;       // LIGHTING_STEP_SATURATION_UP, LIGHTING_STEP_SATURATION_DOWN
  uint8 transitionTime; // time to perform a single step in 1/10 of second
} zclCCStepSaturation_t;

typedef struct
{
  uint8 hue;             // a target hue
  uint8 saturation;      // a target saturation
  uint16 transitionTime; // time to move, equal of the value of the field in 1/10 seconds
} zclCCMoveToHueAndSaturation_t;
  
// This callback is called to process a Move To Hue command
// hue - target hue value
// direction
// transitionTime - tame taken to move to the target hue in 1/10 sec increments
typedef void (*zclLighting_ColorControl_MoveToHue_t)( zclCCMoveToHue_t *pCmd );

// This callback is called to process a Move Hue command
// moveMode - LIGHTING_MOVE_HUE_STOP, LIGHTING_MOVE_HUE_UP, LIGHTING_MOVE_HUE_DOWN
// rate - the movement in steps per second, where step is a change in the device's hue of one unit
typedef void (*zclLighting_ColorControl_MoveHue_t)( zclCCMoveHue_t *pCmd );

// This callback is called to process a Step Hue command
// stepMode -	LIGHTING_STEP_HUE_UP, LIGHTING_STEP_HUE_DOWN
// transitionTime - the movement in steps per 1/10 second
typedef void (*zclLighting_ColorControl_StepHue_t)( zclCCStepHue_t *pCmd );

// This callback is called to process a Move To Saturation command
// saturation - target saturation value
// transitionTime - time taken move to the target saturation, in 1/10 second units
typedef void (*zclLighting_ColorControl_MoveToSaturation_t)( zclCCMoveToSaturation_t *pCmd );

// This callback is called to process a Move Saturation command
// moveMode - LIGHTING_MOVE_SATURATION_STOP, LIGHTING_MOVE_SATURATION_UP, LIGHTING_MOVE_SATURATION_DOWN
// rate - rate of movement in step/sec; step is the device's saturation of one unit
typedef void (*zclLighting_ColorControl_MoveSaturation_t)( zclCCMoveSaturation_t *pCmd );

// This callback is called to process a Step Saturation command
// stepMode -  LIGHTING_STEP_SATURATION_UP, LIGHTING_STEP_SATURATION_DOWN
// transitionTime - time to perform a single step in 1/10 of second
typedef void (*zclLighting_ColorControl_StepSaturation_t)( zclCCStepSaturation_t *pCmd );

// This callback is called to process a Move to Hue and Saturation command
// hue - a target hue
// saturation - a target saturation
// transitionTime -  time to move, equal of the value of the field in 1/10 seconds
typedef void (*zclLighting_ColorControl_MoveToHueAndSaturation_t)( zclCCMoveToHueAndSaturation_t *pCmd );


// Register Callbacks table entry - enter function pointers for callbacks that
// the application would like to receive
typedef struct			
{
  zclLighting_ColorControl_MoveToHue_t                pfnColorControl_MoveToHue;
  zclLighting_ColorControl_MoveHue_t                  pfnColorControl_MoveHue;
  zclLighting_ColorControl_StepHue_t                  pfnColorControl_StepHue;
  zclLighting_ColorControl_MoveToSaturation_t         pfnColorControl_MoveToSaturation;
  zclLighting_ColorControl_MoveSaturation_t           pfnColorControl_MoveSaturation;
  zclLighting_ColorControl_StepSaturation_t           pfnColorControl_StepSaturation;
  zclLighting_ColorControl_MoveToHueAndSaturation_t   pfnColorControl_MoveToHueAndSaturation;

} zclLighting_AppCallbacks_t;


/******************************************************************************
 * FUNCTION MACROS
 */

/******************************************************************************
 * VARIABLES
 */

/******************************************************************************
 * FUNCTIONS
 */

/*
 * Register for callbacks from this cluster library
 */
extern ZStatus_t zclLighting_RegisterCmdCallbacks( uint8 endpoint, zclLighting_AppCallbacks_t *callbacks );


/*
 * Call to send out a Move To Hue Command
 *      hue - target hue value
 *      direction - direction of hue change
 *      transitionTime - tame taken to move to the target hue in 1/10 sec increments
 */
extern ZStatus_t zclLighting_ColorControl_Send_MoveToHueCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                              uint8 hue, uint8 direction, uint16 transitionTime,
                                              uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Call to send out a Move Hue Command
 *      moveMode - LIGHTING_MOVE_HUE_STOP, LIGHTING_MOVE_HUE_UP, LIGHTING_MOVE_HUE_DOWN
 *      rate - the movement in steps per second (step is a change in the device's hue 
 *             of one unit)
 */
extern ZStatus_t zclLighting_ColorControl_Send_MoveHueCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                           uint8 moveMode, uint8 rate, 
                                                           uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Call to send out a Step Hue Command
 *      stepMode - LIGHTING_STEP_HUE_UP, LIGHTING_STEP_HUE_DOWN
 *      amount - number of hue units to step
 *      transitionTime - the movement in steps per 1/10 second
 */
extern ZStatus_t zclLighting_ColorControl_Send_StepHueCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                          uint8 stepMode, uint8 transitionTime,
                                          uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Call to send out a Move To Saturation Command
 *      saturation - target saturation value
 *      transitionTime - time taken move to the target saturation, in 1/10 second units
 */
extern ZStatus_t zclLighting_ColorControl_Send_MoveToSaturationCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                uint8 saturation, uint16 transitionTime, 
                                                uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Call to send out a Move Saturation Command
 *      moveMode - LIGHTING_MOVE_SATURATION_STOP, LIGHTING_MOVE_SATURATION_UP, 
 *                 LIGHTING_MOVE_SATURATION_DOWN
 *      rate -  rate of movement in step per second; step is the device's 
 *              saturation of one unit
 */
extern ZStatus_t zclLighting_ColorControl_Send_MoveSaturationCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                                  uint8 moveMode, uint8 rate,
                                                                  uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Call to send out a Step Saturation Command
 *      stepMode -  LIGHTING_STEP_SATURATION_UP, LIGHTING_STEP_SATURATION_DOWN
 *      amount -  number of units to change the saturation level by
 *      transitionTime - time to perform a single step in 1/10 of second
 */
extern ZStatus_t zclLighting_ColorControl_Send_StepSaturationCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                 uint8 stepMode, uint8 transitionTime,
                                                 uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Call to send out a Move To Hue And Saturation  Command
 *      hue - 	target hue
 *      saturation -  target saturation
 *      transitionTime -  time to move, equal of the value of the field in 1/10 seconds
 */
extern ZStatus_t zclLighting_ColorControl_Send_MoveToHueAndSaturationCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                          uint8 hue, uint8 saturation, uint16 transitionTime, 
                                                          uint8 disableDefaultRsp, uint8 seqNum );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZCL_LIGHTING_H */
