/**************************************************************************************************
  Filename:       zcl.c
  Revised:        $Date: 2007-12-13 15:02:48 -0800 (Thu, 13 Dec 2007) $
  Revision:       $Revision: 16070 $

  Description:    This file contains the Zigbee Cluster Library Foundation functions.


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
#include "AF.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_ha.h"
#include "zcl_general.h"

/*********************************************************************
 * MACROS
 */
/*** Frame Control ***/
#define zcl_FCType( a )               ( (a) & ZCL_FRAME_CONTROL_TYPE )
#define zcl_FCManuSpecific( a )       ( (a) & ZCL_FRAME_CONTROL_MANU_SPECIFIC )
#define zcl_FCDirection( a )          ( (a) & ZCL_FRAME_CONTROL_DIRECTION )
#define zcl_FCDisableDefaultRsp( a )  ( (a) & ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RSP )

/*** Attribute Access Control ***/
#define zcl_AccessCtrlRead( a )       ( (a) & ACCESS_CONTROL_READ )
#define zcl_AccessCtrlWrite( a )      ( (a) & ACCESS_CONTROL_WRITE )
#define zcl_AccessCtrlCmd( a )        ( (a) & ACCESS_CONTROL_CMD )

#define zclParseCmd( a, b )           zclCmdTable[(a)].pfnParseInProfile( (b) )
#define zclProcessCmd( a, b )         zclCmdTable[(a)].pfnProcessInProfile( (b) )

/*** Table Indexing ***/
#define NEXT_READ_RSP( ptr, len )     (zclReadRspStatus_t *)( (uint8 *)(ptr) + \
                                           sizeof (zclReadRspStatus_t) + (len) )
#define NEXT_WRITE( ptr, len )        (zclWriteRec_t *)( (uint8 *)(ptr) + \
                                           sizeof (zclWriteRec_t) + (len) )
#define NEXT_REPORT( ptr, len )       (zclReport_t *)( (uint8 *)(ptr) + \
                                           sizeof (zclReport_t) + (len) )
#define NEXT_CFG_REPORT( ptr, len )   (zclCfgReportRec_t *)( (uint8 *)(ptr) + \
                                           sizeof (zclCfgReportRec_t) + (len) )
#define NEXT_REPORT_RSP( ptr, len )   (zclReportCfgRspRec_t *) ( (uint8 *)(ptr) + \
                                           sizeof (zclReportCfgRspRec_t) + (len) )

// Commands that have corresponding responses
#define CMD_HAS_RSP( cmd )            ( (cmd) == ZCL_CMD_READ            || \
                                        (cmd) == ZCL_CMD_WRITE           || \
                                        (cmd) == ZCL_CMD_WRITE_UNDIVIDED || \
                                        (cmd) == ZCL_CMD_CONFIG_REPORT   || \
                                        (cmd) == ZCL_CMD_READ_REPORT_CFG || \
                                        (cmd) == ZCL_CMD_DISCOVER        || \
                                        (cmd) == ZCL_CMD_DEFAULT_RSP ) // exception

// There is no attribute in the Mandatory Reportable Attribute list for now
#define zcl_MandatoryReportableAttribute( a ) ( a == NULL )

/*********************************************************************
 * CONSTANTS
 */
// Used by Configure Reporting Command
#define SEND_ATTR_REPORTS             0x00
#define EXPECT_ATTR_REPORTS           0x01

#define ZCL_MIN_REPORTING_INTERVAL    5

/*********************************************************************
 * TYPEDEFS
 */
typedef struct zclLibPlugin
{
  struct zclLibPlugin *next;
  uint16              startLogCluster;    // starting logical cluster ID
  uint16              endLogCluster;      // ending logical cluster ID
  zclInHdlr_t         pfnIncomingHdlr;    // function to handle incoming message
} zclLibPlugin_t;

// Record to store a profile's cluster conversion table
typedef struct zclProfileClusterConvertRec
{
  struct zclProfileClusterConvertRec *next;
  uint16                             profileID;
  uint16                             numClusters;
  GENERIC zclConvertClusterRec_t     *list;
} zclProfileClusterConvertRec_t;

// Attribute record list item
typedef struct zclAttrRecsList
{
  struct zclAttrRecsList *next;
  uint8                  endpoint;      // Used to link it into the endpoint descriptor
  uint8                  numAttributes; // Number of the following records
  CONST zclAttrRec_t     *attrs;        // attribute records
} zclAttrRecsList;

typedef void *(*zclParseInProfileCmd_t)( zclParseCmd_t *pCmd );
typedef uint8 (*zclProcessInProfileCmd_t)( zclIncoming_t *pInMsg );

typedef struct
{
  zclParseInProfileCmd_t   pfnParseInProfile;
  zclProcessInProfileCmd_t pfnProcessInProfile;
} zclCmdItems_t;


/*********************************************************************
 * GLOBAL VARIABLES
 */
uint8 zcl_TaskID;

// The Application should register its attribute data validation function
zclValidateAttrData_t zcl_ValidateAttrDataCB = NULL;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static zclLibPlugin_t *plugins;
static zclProfileClusterConvertRec_t *profileClusterList;
static zclAttrRecsList *attrList;
static byte zcl_TransID = 0;  // This is the unique message ID (counter)

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void zclProcessMessageMSG( afIncomingMSGPacket_t *pkt );
static uint8 *zclBuildHdr( zclFrameHdr_t *hdr, uint8 *pData );
static uint8 zclCalcHdrSize( zclFrameHdr_t *hdr );
static uint16 zclConvertClusterID( uint16 clusterID, uint16 profileID,
                                   uint8 convertToLogical );
static zclLibPlugin_t *zclFindPlugin( uint16 realclusterID, uint16 profileID );

static uint8 zcl_DeviceOperational( uint8 srcEP, uint16 realClusterID, uint8 frameType, uint8 cmd );

static uint8 zclGetAttrDataLength( uint8  dataType, uint8 *pData);
static uint8 zclGetDataTypeLength( uint8 dataType );

#if defined(ZCL_READ) || defined(ZCL_WRITE) || defined(ZCL_REPORT)
static void zclSerializeData( uint8 dataType, void *attrData, uint8 *buf );
#endif // ZCL_READ || ZCL_WRITE || ZCL_REPORT

#ifdef ZCL_READ
static void *zclParseInReadRspCmd( zclParseCmd_t *pCmd );
static uint8 zclProcessInReadCmd( zclIncoming_t *pInMsg );
static uint8 zclProcessInReadRspCmd( zclIncoming_t *pInMsg );
#endif // ZCL_READ

#ifdef ZCL_WRITE
static uint8 zclWriteAttrData( zclAttrRec_t *pAttr, zclWriteRec_t *pWriteRec );
static void *zclParseInWriteRspCmd( zclParseCmd_t *pCmd );
static uint8 zclProcessInWriteCmd( zclIncoming_t *pInMsg );
static uint8 zclProcessInWriteUndividedCmd( zclIncoming_t *pInMsg );
static uint8 zclProcessInWriteRspCmd( zclIncoming_t *pInMsg );
#endif // ZCL_WRITE

#ifdef ZCL_REPORT
static uint8 zclAnalogDataType( uint8 dataType );
static void *zclParseInConfigReportRspCmd( zclParseCmd_t *pCmd );
static void *zclParseInReadReportCfgRspCmd( zclParseCmd_t *pCmd );
static uint8 zclProcessInConfigReportCmd( zclIncoming_t *pInMsg );
static uint8 zclProcessInConfigReportRspCmd( zclIncoming_t *pInMsg );
static uint8 zclProcessInReadReportCfgCmd( zclIncoming_t *pInMsg );
static uint8 zclProcessInReadReportCfgRspCmd( zclIncoming_t *pInMsg );
static uint8 zclProcessInReportCmd( zclIncoming_t *pInMsg );
#endif // ZCL_REPORT

static void *zclParseInDefaultRspCmd( zclParseCmd_t *pCmd );
static uint8 zclProcessInDefaultRspCmd( zclIncoming_t *pInMsg );

#ifdef ZCL_DISCOVER
static uint8 zclFindNextAttrRec( uint8 endpoint, uint16 realClusterID, uint16 *attrId, zclAttrRec_t *pAttr );
static void *zclParseInDiscRspCmd( zclParseCmd_t *pCmd );
static uint8 zclProcessInDiscCmd( zclIncoming_t *pInMsg );
static uint8 zclProcessInDiscRspCmd( zclIncoming_t *pInMsg );
#endif // ZCL_DISCOVER

/*********************************************************************
 * Parse Profile Command Function Table
 */
static CONST zclCmdItems_t zclCmdTable[] =
{
#ifdef ZCL_READ
  /* ZCL_CMD_READ */                { zclParseInReadCmd,             zclProcessInReadCmd             },
  /* ZCL_CMD_READ_RSP */            { zclParseInReadRspCmd,          zclProcessInReadRspCmd          },
#else
  /* ZCL_CMD_READ */                { NULL,                          NULL                            },
  /* ZCL_CMD_READ_RSP */            { NULL,                          NULL                            },
#endif // ZCL_READ

#ifdef ZCL_WRITE
  /* ZCL_CMD_WRITE */               { zclParseInWriteCmd,            zclProcessInWriteCmd            },
  /* ZCL_CMD_WRITE_UNDIVIDED */     { zclParseInWriteCmd,            zclProcessInWriteUndividedCmd   },
  /* ZCL_CMD_WRITE_RSP */           { zclParseInWriteRspCmd,         zclProcessInWriteRspCmd         },
  /* ZCL_CMD_WRITE_NO_RSP */        { zclParseInWriteCmd,            zclProcessInWriteCmd            },
#else
  /* ZCL_CMD_WRITE */               { NULL,                          NULL                            },
  /* ZCL_CMD_WRITE_UNDIVIDED */     { NULL,                          NULL                            },
  /* ZCL_CMD_WRITE_RSP */           { NULL,                          NULL                            },
  /* ZCL_CMD_WRITE_NO_RSP */        { NULL,                          NULL                            },
#endif // ZCL_WRITE

#ifdef ZCL_REPORT
  /* ZCL_CMD_CONFIG_REPORT */       { zclParseInConfigReportCmd,     zclProcessInConfigReportCmd     },
  /* ZCL_CMD_CONFIG_REPORT_RSP */   { zclParseInConfigReportRspCmd,  zclProcessInConfigReportRspCmd  },
  /* ZCL_CMD_READ_REPORT_CFG */     { zclParseInReadReportCfgCmd,    zclProcessInReadReportCfgCmd    },
  /* ZCL_CMD_READ_REPORT_CFG_RSP */ { zclParseInReadReportCfgRspCmd, zclProcessInReadReportCfgRspCmd },
  /* ZCL_CMD_REPORT */              { zclParseInReportCmd,           zclProcessInReportCmd           },
#else
  /* ZCL_CMD_CONFIG_REPORT */       { NULL,                          NULL                            },
  /* ZCL_CMD_CONFIG_REPORT_RSP */   { NULL,                          NULL                            },
  /* ZCL_CMD_READ_REPORT_CFG */     { NULL,                          NULL                            },
  /* ZCL_CMD_READ_REPORT_CFG_RSP */ { NULL,                          NULL                            },
  /* ZCL_CMD_REPORT */              { NULL,                          NULL                            },
#endif // ZCL_REPORT

  /* ZCL_CMD_DEFAULT_RSP */         { zclParseInDefaultRspCmd,       zclProcessInDefaultRspCmd       },
  
#ifdef ZCL_DISCOVER  
  /* ZCL_CMD_DISCOVER */            { zclParseInDiscCmd,             zclProcessInDiscCmd             },
  /* ZCL_CMD_DISCOVER_RSP */        { zclParseInDiscRspCmd,          zclProcessInDiscRspCmd          }
#else
  /* ZCL_CMD_DISCOVER */            { NULL,                          NULL                            },
  /* ZCL_CMD_DISCOVER_RSP */        { NULL,                          NULL                            }
#endif // ZCL_DISCOVER
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn          zcl_Init
 *
 * @brief       Initialization function for the zcl layer.
 *
 * @param       task_id - ZCL task id
 *
 * @return      none
 */
void zcl_Init( uint8 task_id )
{
  zcl_TaskID = task_id;

  plugins = (zclLibPlugin_t  *)NULL;
  profileClusterList = (zclProfileClusterConvertRec_t *)NULL;
  attrList = (zclAttrRecsList *)NULL;
}

/*********************************************************************
 * @fn          zcl_event_loop
 *
 * @brief       Event Loop Processor for zcl.
 *
 * @param       task_id - task id
 * @param       events - event bitmap
 *
 * @return      none
 */
uint16 zcl_event_loop( uint8 task_id, uint16 events )
{
  byte *msgPtr;

  if ( events & SYS_EVENT_MSG )
  {
    msgPtr = osal_msg_receive( zcl_TaskID );
    while ( msgPtr )
    {
      if ( *msgPtr == AF_INCOMING_MSG_CMD )
          zclProcessMessageMSG( (afIncomingMSGPacket_t *)msgPtr );
        
#if 0
      switch ( *msgPtr )
      {
        case AF_DATA_CONFIRM_CMD:
          break;

        case AF_INCOMING_MSG_CMD:
          // Process the incoming message
          zclProcessMessageMSG( (afIncomingMSGPacket_t *)msgPtr );
          break;

        case ZDO_NEW_DSTADDR:
          // Not used
          break;

        default:
          break;
      }
#endif
      // Release the memory
      osal_msg_deallocate( msgPtr );

      // Next
      msgPtr = osal_msg_receive( zcl_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * @fn          zcl_registerClusterConvertTable
 *
 * @brief       Add a profile's cluster conversion table
 *
 * @param       profileId - profile's ID
 * @param       numClusters - number of clusters in list
 * @param       clusterList - Profile's Cluster Conversion List
 *
 * @return      ZSuccess if OK
 */
ZStatus_t zcl_registerClusterConvertTable( uint16 profileId,
              uint16 numClusters,
              zclConvertClusterRec_t GENERIC *clusterList )
{
  zclProfileClusterConvertRec_t *pNewItem;
  zclProfileClusterConvertRec_t *pLoop;

  // Fill in the new profile list
  pNewItem = osal_mem_alloc( sizeof( zclProfileClusterConvertRec_t ) );
  if ( pNewItem == NULL )
    return (ZMemError);

  pNewItem->next = (zclProfileClusterConvertRec_t *)NULL;
  pNewItem->profileID = profileId;
  pNewItem->numClusters = numClusters;
  pNewItem->list = clusterList;

  // Find spot in list
  if (  profileClusterList == NULL )
  {
    profileClusterList = pNewItem;
  }
  else
  {
    // Look for end of list
    pLoop = profileClusterList;
    while ( pLoop->next != NULL )
      pLoop = pLoop->next;

    // Put new item at end of list
    pLoop->next = pNewItem;
  }

  return ( ZSuccess );
}

/*********************************************************************
 * @fn          zcl_registerPlugin
 *
 * @brief       Add a Cluster Library handler
 *
 * @param       startLogCluster - starting logical cluster ID
 * @param       endLogCluster - ending logical cluster ID
 * @param       pfnHdlr - function pointer to incoming message handler
 *
 * @return      ZSuccess if OK
 */
ZStatus_t zcl_registerPlugin( uint16 startLogCluster,
          uint16 endLogCluster, zclInHdlr_t pfnIncomingHdlr )
{
  zclLibPlugin_t *pNewItem;
  zclLibPlugin_t *pLoop;

  // Fill in the new profile list
  pNewItem = osal_mem_alloc( sizeof( zclLibPlugin_t ) );
  if ( pNewItem == NULL )
    return (ZMemError);

  // Fill in the plugin record.
  pNewItem->next = (zclLibPlugin_t *)NULL;
  pNewItem->startLogCluster = startLogCluster;
  pNewItem->endLogCluster = endLogCluster;
  pNewItem->pfnIncomingHdlr = pfnIncomingHdlr;

  // Find spot in list
  if (  plugins == NULL )
  {
    plugins = pNewItem;
  }
  else
  {
    // Look for end of list
    pLoop = plugins;
    while ( pLoop->next != NULL )
      pLoop = pLoop->next;

    // Put new item at end of list
    pLoop->next = pNewItem;
  }

  return ( ZSuccess );
}

/*********************************************************************
 * @fn          zcl_registerAttrList
 *
 * @brief       Register an Attribute List with ZCL Foundation
 *
 * @param       endpoint the attribute list belongs to
 * @param       numAttr - number of attributes in list
 * @param       newAttrList - array of Attribute records.
 *                            NOTE: THE ATTRIBUTE IDs (FOR A CLUSTER) MUST BE IN
 *                            ASCENDING ORDER. OTHERWISE, THE DISCOVERY RESPONSE
 *                            COMMAND WILL NOT HAVE THE RIGHT ATTRIBUTE INFO
 *
 * @return      ZSuccess if OK
 */
ZStatus_t zcl_registerAttrList( uint8 endpoint, uint8 numAttr, CONST zclAttrRec_t newAttrList[] )
{
  zclAttrRecsList *pNewItem;
  zclAttrRecsList *pLoop;

  // Fill in the new profile list
  pNewItem = osal_mem_alloc( sizeof( zclAttrRecsList ) );
  if ( pNewItem == NULL )
    return (ZMemError);

  pNewItem->next = (zclAttrRecsList *)NULL;
  pNewItem->endpoint = endpoint;
  pNewItem->numAttributes = numAttr;
  pNewItem->attrs = newAttrList;

  // Find spot in list
  if ( attrList == NULL )
  {
    attrList = pNewItem;
  }
  else
  {
    // Look for end of list
    pLoop = attrList;
    while ( pLoop->next != NULL )
      pLoop = pLoop->next;

    // Put new item at end of list
    pLoop->next = pNewItem;
  }

  return ( ZSuccess );
}

/*********************************************************************
 * @fn          zcl_registerValidateAttrData
 *
 * @brief       Add a validation function for attribute data
 *
 * @param       pfnValidateAttrData - function pointer to validate routine
 *
 * @return      ZSuccess if OK
 */
ZStatus_t zcl_registerValidateAttrData( zclValidateAttrData_t pfnValidateAttrData )
{
  zcl_ValidateAttrDataCB = pfnValidateAttrData;
  
  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zcl_DeviceOperational
 *
 * @brief   Used to see whether or not the device can send or respond 
 *          to application level commands.
 *
 * @param   srcEP - source endpoint
 * @param   realClusterID - real Cluster ID
 * @param   frameType - command type
 * @param   cmd - command ID
 *
 * @return  TRUE if device is operational, FLASE otherwise
 */
static uint8 zcl_DeviceOperational( uint8 srcEP, uint16 realClusterID, 
                                    uint8 frameType, uint8 cmd )
{
  zclAttrRec_t attrRec;
  uint8 deviceEnabled = DEVICE_ENABLED; // default value

  // If the device is Disabled (DeviceEnabled attribute is set to Disabled), it 
  // cannot send or respond to application level commands, other than commands
  // to read or write attributes. Note that the Identify cluster cannot be 
  // disabled, and remains functional regardless of this setting.
  if ( zcl_ProfileCmd( frameType ) && cmd <= ZCL_CMD_WRITE_NO_RSP )
    return ( TRUE );
  
  if ( realClusterID == ZCL_HA_CLUSTER_ID_GEN_IDENTIFY )
    return ( TRUE );
  
  // Is device enabled?
  if ( zclFindAttrRec( srcEP, ZCL_HA_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_DEVICE_ENABLED, &attrRec ) )
    zclReadAttrData( &deviceEnabled, &attrRec );
  
  return ( deviceEnabled == DEVICE_ENABLED ? TRUE : FALSE );
}

/*********************************************************************
 * @fn      zcl_SendCommand
 *
 * @brief   Used to send Profile and Cluster Specific Command messages.
 *
 *          NOTE: The calling application is responsible for incrementing 
 *                the Sequence Number.
 *
 * @param   srcEp - source endpoint
 * @param   destAddr - destination address 
 * @param   clusterID - cluster ID
 * @param   logicalCluster - whether the cluster ID is a logical cluster ID
 * @param   cmd - command ID
 * @param   specific - whether the command is Cluster Specific
 * @param   direction - client/server direction of the command
 * @param   disableDefaultRsp - disable Default Response command
 * @param   manuCode - manufacturer code for proprietary extensions to a profile
 * @param   seqNumber - identification number for the transaction
 * @param   cmdFormatLen - length of the command to be sent
 * @param   cmdFormat - command to be sent
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendCommand( uint8 srcEP, afAddrType_t *destAddr,
                           uint16 clusterID, uint8 logicalCluster,
                           uint8 cmd, uint8 specific, uint8 direction,
                           uint8 disableDefaultRsp, uint16 manuCode, uint8 seqNum,
                           uint8 cmdFormatLen, uint8 *cmdFormat )
{
  endPointDesc_t *epDesc;
  uint16 realClusterID;
  uint16 logicalClusterID;
  zclFrameHdr_t hdr;
  uint8 *msgBuf;
  uint8 msgLen;
  uint8 *pBuf;
  afAddrType_t dstAddr;
  ZStatus_t status;

  osal_memcpy( &dstAddr, destAddr, sizeof ( afAddrType_t ) );

  epDesc = afFindEndPointDesc( srcEP );
  if ( epDesc == NULL )
    return ( ZInvalidParameter ); // EMBEDDED RETURN

  if ( logicalCluster )
  {
    logicalClusterID = clusterID;
    realClusterID = zclConvertClusterID( clusterID, epDesc->simpleDesc->AppProfId, FALSE );
  }
  else
  {
    realClusterID = clusterID;
    logicalClusterID = zclConvertClusterID( clusterID, epDesc->simpleDesc->AppProfId, TRUE );
  }

  if ( realClusterID == ZCL_INVALID_CLUSTER_ID || logicalClusterID == ZCL_INVALID_CLUSTER_ID )
    return ( ZInvalidParameter ); // EMBEDDED RETURN

  osal_memset( &hdr, 0, sizeof( zclFrameHdr_t ) );

  // Not Profile wide command (like READ, WRITE)
  if ( specific )
    hdr.fc.type = ZCL_FRAME_TYPE_SPECIFIC_CMD;
  else
    hdr.fc.type = ZCL_FRAME_TYPE_PROFILE_CMD;

  if ( zcl_DeviceOperational( srcEP, realClusterID, hdr.fc.type, cmd ) == FALSE )
    return ( ZFailure ); // EMBEDDED RETURN
  
  // Fill in the Maufacturer Code
  if ( manuCode != 0 )
  {
    hdr.fc.manuSpecific = 1;
    hdr.manuCode = manuCode;
  }
  
  // Set the Command Direction
  if ( direction )
    hdr.fc.direction = ZCL_FRAME_SERVER_CLIENT_DIR;
  else
    hdr.fc.direction = ZCL_FRAME_CLIENT_SERVER_DIR;

  // Set the Disable Default Response field
  if ( disableDefaultRsp )
    hdr.fc.disableDefaultRsp = 1;
  else
    hdr.fc.disableDefaultRsp = 0;
                     
  // Fill in the Transaction Sequence Number
  hdr.transSeqNum = seqNum;
  
  // Fill in the command
  hdr.commandID = cmd;
  
  // calculate the needed buffer size
  msgLen = zclCalcHdrSize( &hdr );
  msgLen += cmdFormatLen;

  // Allocate the buffer needed
  msgBuf = osal_mem_alloc( msgLen );
  if ( msgBuf )
  {
    // Fill in the ZCL Header
    pBuf = zclBuildHdr( &hdr, msgBuf );

    // Fill in the command frame
    osal_memcpy( pBuf, cmdFormat, cmdFormatLen );

    status = AF_DataRequest( &dstAddr, epDesc, realClusterID, msgLen, msgBuf, 
                             &zcl_TransID, AF_MSG_ACK_REQUEST, AF_DEFAULT_RADIUS );  
    osal_mem_free ( msgBuf );
  }
  else
    status = ZMemError;

  return ( status );
}

#ifdef ZCL_READ
/*********************************************************************
 * @fn      zcl_SendRead
 *
 * @brief   Send a Read command
 *
 * @param   srcEP - Application's endpoint
 * @param   dstAddr - destination address
 * @param   realClusterID - real cluster ID
 * @param   readCmd - read command to be sent
 * @param   direction - direction of the command
 * @param   seqNum - transaction sequence number
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendRead( uint8 srcEP, afAddrType_t *dstAddr,
                        uint16 realClusterID, zclReadCmd_t *readCmd,
                        uint8 direction, uint8 disableDefaultRsp, uint8 seqNum)
{
  uint8 dataLen;
  uint8 *buf;
  uint8 *pBuf;
  ZStatus_t status;

  dataLen = readCmd->numAttr * 2; // Attribute ID

  buf = osal_mem_alloc( dataLen );
  if ( buf )
  {
    uint8 i;

    // Load the buffer - serially
    pBuf = buf;   
    for (i = 0; i < readCmd->numAttr; i++)
    {
      *pBuf++ = LO_UINT16( readCmd->attrID[i] );
      *pBuf++ = HI_UINT16( readCmd->attrID[i] );
    }
  
    status = zcl_SendCommand( srcEP, dstAddr, realClusterID, FALSE, ZCL_CMD_READ, 
                              FALSE, direction, disableDefaultRsp, 0, seqNum, dataLen, buf );  
    osal_mem_free( buf );
  }
  else
    status = ZMemError;

  return ( status );
}

/*********************************************************************
 * @fn      zcl_SendReadRsp
 *
 * @brief   Send a Read Response command.
 *
 * @param   srcEP - Application's endpoint
 * @param   dstAddr - destination address
 * @param   realClusterID - real cluster ID
 * @param   readRspCmd - read response command to be sent
 * @param   direction - direction of the command
 * @param   seqNum - transaction sequence number
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendReadRsp( uint8 srcEP, afAddrType_t *dstAddr,
                           uint16 realClusterID, zclReadRspCmd_t *readRspCmd,
                           uint8 direction, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 *buf;
  uint8 *pBuf;
  zclReadRspStatus_t *statusRec;
  uint8 dataLen;
  uint8 len = 0;
  uint8 i;
  ZStatus_t status;
  
  // calculate the size of the command
  statusRec = readRspCmd->attrList;
  for ( i = 0; i < readRspCmd->numAttr; i++ )
  {
    len += 2 + 1; // Attribute ID + Status

    if ( statusRec->status == ZCL_STATUS_SUCCESS )
    {
      len++; // Attribute Data Type
      dataLen = zclGetAttrDataLength( statusRec->dataType, statusRec->data);
      len += dataLen; // Attribute Data
    }
    else
      dataLen = 0;
    statusRec = NEXT_READ_RSP(statusRec, dataLen);
  }

  buf = osal_mem_alloc( len );
  if ( buf )
  {
    // Load the buffer - serially
    pBuf = buf;
    statusRec = readRspCmd->attrList;
    for ( i = 0; i < readRspCmd->numAttr; i++ )
    {
      *pBuf++ = LO_UINT16( statusRec->attrID );
      *pBuf++ = HI_UINT16( statusRec->attrID );
      *pBuf++ = statusRec->status;

      if ( statusRec->status == ZCL_STATUS_SUCCESS )
      {
        *pBuf++ = statusRec->dataType;
        zclSerializeData( statusRec->dataType, statusRec->data, pBuf );
        dataLen = zclGetAttrDataLength( statusRec->dataType, statusRec->data );
        pBuf += dataLen; // move pass attribute data
      }
      else
        dataLen = 0;
      statusRec = NEXT_READ_RSP(statusRec, dataLen);
    } // for loop

    status = zcl_SendCommand( srcEP, dstAddr, realClusterID, FALSE, ZCL_CMD_READ_RSP,
                              FALSE, direction, disableDefaultRsp, 0, seqNum, len, buf );
    osal_mem_free( buf );
  }
  else
    status = ZMemError;

  return ( status );
}
#endif // ZCL_READ

#ifdef ZCL_WRITE
/*********************************************************************
 * @fn      sendWriteRequest
 *
 * @brief   Send a Write command
 *
 * @param   dstAddr - destination address
 * @param   realClusterID - real cluster ID
 * @param   writeCmd - write command to be sent
 * @param   cmd - ZCL_CMD_WRITE, ZCL_CMD_WRITE_UNDIVIDED or ZCL_CMD_WRITE_NO_RSP
 * @param   direction - direction of the command
 * @param   seqNum - transaction sequence number
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendWriteRequest( uint8 srcEP, afAddrType_t *dstAddr, uint16 realClusterID, 
                                zclWriteCmd_t *writeCmd, uint8 cmd, uint8 direction, 
                                uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 *buf;
  uint8 *pBuf;
  zclWriteRec_t *statusRec;
  uint8 attrDataLen;
  uint8 dataLen = 0;
  uint8 i;
  ZStatus_t status;
  
  statusRec = writeCmd->attrList;
  for ( i = 0; i < writeCmd->numAttr; i++ )
  {
    attrDataLen = zclGetAttrDataLength( statusRec->dataType, statusRec->attrData );
    dataLen += 2 + 1 + attrDataLen; // Attribute ID + Attribute Type + Attribute Data
    statusRec = NEXT_WRITE( statusRec, attrDataLen );
  }

  buf = osal_mem_alloc( dataLen );
  if ( buf )
  {
    // Load the buffer - serially
    pBuf = buf;
    statusRec = writeCmd->attrList;
    for ( i = 0; i < writeCmd->numAttr; i++ )
    { 
      *pBuf++ = LO_UINT16( statusRec->attrID );
      *pBuf++ = HI_UINT16( statusRec->attrID );
      *pBuf++ = statusRec->dataType;
      zclSerializeData( statusRec->dataType, statusRec->attrData, pBuf );
      
      attrDataLen = zclGetAttrDataLength( statusRec->dataType, statusRec->attrData );
      pBuf += attrDataLen; // move pass attribute data
      statusRec = NEXT_WRITE( statusRec, attrDataLen );
    }

    status = zcl_SendCommand( srcEP, dstAddr, realClusterID, FALSE, cmd, FALSE, 
                              direction, disableDefaultRsp, 0, seqNum, dataLen, buf );
    osal_mem_free( buf );
  }
  else
    status = ZMemError;

  return ( status);
}

/*********************************************************************
 * @fn      zcl_SendWriteRsp
 *
 * @brief   Send a Write Response command
 *
 * @param   dstAddr - destination address
 * @param   realClusterID - real cluster ID
 * @param   wrtieRspCmd - write response command to be sent
 * @param   direction - direction of the command
 * @param   seqNum - transaction sequence number
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendWriteRsp( uint8 srcEP, afAddrType_t *dstAddr,
                            uint16 realClusterID, zclWriteRspCmd_t *writeRspCmd,
                            uint8 direction, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 dataLen;
  uint8 *buf;
  uint8 *pBuf;
  uint8 i;
  ZStatus_t status;
  
  dataLen =  sizeof( zclWriteRspStatus_t ) * writeRspCmd->numAttr;

  buf = osal_mem_alloc( dataLen );
  if ( buf )
  {
    // Load the buffer - serially
    pBuf = buf;
    for ( i = 0; i < writeRspCmd->numAttr; i++ )
    { 
      *pBuf++ = writeRspCmd->attrList[i].status;
      *pBuf++ = LO_UINT16( writeRspCmd->attrList[i].attrID );
      *pBuf++ = HI_UINT16( writeRspCmd->attrList[i].attrID );
    }
    
    // If there's only a single status record and its status field is set to 
    // SUCCESS then omit the attribute ID field.
    if ( writeRspCmd->numAttr == 1 && writeRspCmd->attrList[0].status == ZCL_STATUS_SUCCESS )
      dataLen = 1;
      
    status = zcl_SendCommand( srcEP, dstAddr, realClusterID, FALSE, ZCL_CMD_WRITE_RSP,
                              FALSE, direction, disableDefaultRsp, 0, seqNum, dataLen, buf );
    osal_mem_free( buf );
  }
  else
    status = ZMemError;

  return ( status );
}
#endif // ZCL_WRITE

#ifdef ZCL_REPORT
/*********************************************************************
 * @fn      zcl_SendConfigReportCmd
 *
 * @brief   Send a Configure Reporting command
 *
 * @param   dstAddr - destination address
 * @param   realClusterID - real cluster ID
 * @param   cfgReportCmd - configure reporting command to be sent
 * @param   direction - direction of the command
 * @param   seqNum - transaction sequence number
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendConfigReportCmd( uint8 srcEP, afAddrType_t *dstAddr,
                          uint16 realClusterID, zclCfgReportCmd_t *cfgReportCmd,
                          uint8 direction, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 *buf;
  uint8 *pBuf;
  uint8 dataLen = 0;
  zclCfgReportRec_t *reportRec;
  uint8 reportChangeLen; // length of Reportable Change field
  uint8 i;
  ZStatus_t status;
  
  // Find out the data length
  reportRec = cfgReportCmd->attrList;
  for ( i = 0; i < cfgReportCmd->numAttr; i++ )
  {
    dataLen += 1 + 2; // Direction + Attribute ID
    reportChangeLen = 0;
    
    if ( reportRec->direction == SEND_ATTR_REPORTS )
    {
      dataLen += 1 + 2 + 2; // Data Type + Min + Max Reporting Intervals
      
      // Find out the size of the Reportable Change field (for Analog data types)
      if ( zclAnalogDataType( reportRec->dataType ) )
      {
        reportChangeLen = zclGetDataTypeLength( reportRec->dataType );
        dataLen += reportChangeLen;
      }
    }
    else
    {
      dataLen += 2; // Timeout Period
    }
    
    reportRec = NEXT_CFG_REPORT( reportRec, reportChangeLen );
  }
  
  buf = osal_mem_alloc( dataLen );
  if ( buf )
  {
    // Load the buffer - serially
    pBuf = buf;
    reportRec = cfgReportCmd->attrList;
    for ( i = 0; i < cfgReportCmd->numAttr; i++ )
    {
      reportChangeLen = 0;
      
      *pBuf++ = reportRec->direction;
      *pBuf++ = LO_UINT16( reportRec->attrID );
      *pBuf++ = HI_UINT16( reportRec->attrID );
      
      if ( reportRec->direction == SEND_ATTR_REPORTS )
      {
        *pBuf++ = reportRec->dataType;
        *pBuf++ = LO_UINT16( reportRec->minReportInt );
        *pBuf++ = HI_UINT16( reportRec->minReportInt );
        *pBuf++ = LO_UINT16( reportRec->maxReportInt );
        *pBuf++ = HI_UINT16( reportRec->maxReportInt );

        if ( zclAnalogDataType( reportRec->dataType ) )
        {
          zclSerializeData( reportRec->dataType, reportRec->reportableChange, pBuf );
          reportChangeLen = zclGetDataTypeLength( reportRec->dataType );
          pBuf += reportChangeLen;
        }
      }
      else
      {
        *pBuf++ = LO_UINT16( reportRec->timeoutPeriod );
        *pBuf++ = HI_UINT16( reportRec->timeoutPeriod );
      } 
      
      reportRec = NEXT_CFG_REPORT( reportRec, reportChangeLen );
    } // for loop
    
    status = zcl_SendCommand( srcEP, dstAddr, realClusterID, FALSE, ZCL_CMD_CONFIG_REPORT,
                              FALSE, direction, disableDefaultRsp, 0, seqNum, dataLen, buf );
    osal_mem_free( buf );
  }
  else
    status = ZMemError;
  
  return ( status );
}

/*********************************************************************
 * @fn      zcl_SendConfigReportRspCmd
 *
 * @brief   Send a Configure Reporting Response command
 *
 * @param   dstAddr - destination address
 * @param   realClusterID - real cluster ID
 * @param   cfgReportRspCmd - configure reporting response command to be sent
 * @param   direction - direction of the command
 * @param   seqNum - transaction sequence number
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendConfigReportRspCmd( uint8 srcEP, afAddrType_t *dstAddr,
                    uint16 realClusterID, zclCfgReportRspCmd_t *cfgReportRspCmd,
                    uint8 direction, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 dataLen;
  uint8 *buf;
  uint8 *pBuf;
  uint8 i;
  ZStatus_t status;
  
  // Atrribute list (Status and Attribute ID)
  dataLen = cfgReportRspCmd->numAttr * ( 1 + 2 ); 
  
  buf = osal_mem_alloc( dataLen );
  if ( buf )
  {
    // Load the buffer - serially
    pBuf = buf; 
    for ( i = 0; i < cfgReportRspCmd->numAttr; i++ )
    {
      *pBuf++ = cfgReportRspCmd->attrList[i].status;
      *pBuf++ = cfgReportRspCmd->attrList[i].direction;
      *pBuf++ = LO_UINT16( cfgReportRspCmd->attrList[i].attrID );
      *pBuf++ = HI_UINT16( cfgReportRspCmd->attrList[i].attrID );
    }
    
    // If there's only a single status record and its status field is set to 
    // SUCCESS then omit the attribute ID field.
    if ( cfgReportRspCmd->numAttr == 1 && cfgReportRspCmd->attrList[0].status == ZCL_STATUS_SUCCESS )
      dataLen = 1;
    
    status = zcl_SendCommand( srcEP, dstAddr, realClusterID, FALSE,
                              ZCL_CMD_CONFIG_REPORT_RSP, FALSE, direction, 
                              disableDefaultRsp, 0, seqNum, dataLen, buf );
    osal_mem_free( buf );
  }
  else
    status = ZMemError;
  
  return ( status );
}

/*********************************************************************
 * @fn      zcl_SendReadReportCfgCmd
 *
 * @brief   Send a Read Reporting Configuration command
 *
 * @param   dstAddr - destination address
 * @param   realClusterID - real cluster ID
 * @param   readReportCfgCmd - read reporting configuration command to be sent
 * @param   direction - direction of the command
 * @param   seqNum - transaction sequence number
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendReadReportCfgCmd( uint8 srcEP, afAddrType_t *dstAddr,
                  uint16 realClusterID, zclReadReportCfgCmd_t *readReportCfgCmd,
                  uint8 direction, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 dataLen;
  uint8 *buf;
  uint8 *pBuf;
  uint8 i;
  ZStatus_t status;
 
  dataLen = readReportCfgCmd->numAttr * (1 + 2); // Direction + Atrribute ID
  
  buf = osal_mem_alloc( dataLen );
  if ( buf )
  {
    // Load the buffer - serially
    pBuf = buf;
    for ( i = 0; i < readReportCfgCmd->numAttr; i++ )
    {
      *pBuf++ = readReportCfgCmd->attrList[i].direction;
      *pBuf++ = LO_UINT16( readReportCfgCmd->attrList[i].attrID );
      *pBuf++ = HI_UINT16( readReportCfgCmd->attrList[i].attrID );
    }
    
    status = zcl_SendCommand( srcEP, dstAddr, realClusterID, FALSE, ZCL_CMD_READ_REPORT_CFG,
                              FALSE, direction, disableDefaultRsp, 0, seqNum, dataLen, buf );
    osal_mem_free( buf );
  }
  else
    status = ZMemError;
  
  return ( status );
}

/*********************************************************************
 * @fn      zcl_SendReadReportCfgRspCmd
 *
 * @brief   Send a Read Reporting Configuration Response command
 *
 * @param   dstAddr - destination address
 * @param   realClusterID - real cluster ID
 * @param   readReportCfgRspCmd - read reporting configuration response command to be sent
 * @param   direction - direction of the command
 * @param   seqNum - transaction sequence number
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendReadReportCfgRspCmd( uint8 srcEP, afAddrType_t *dstAddr,
             uint16 realClusterID, zclReadReportCfgRspCmd_t *readReportCfgRspCmd,
             uint8 direction, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 *buf;
  uint8 *pBuf;
  uint8 dataLen = 0;
  zclReportCfgRspRec_t *reportRspRec;
  uint8 reportChangeLen;
  zclAttrRec_t *pAttr;
  uint8 i;
  ZStatus_t status;

  // Find out the data length
  reportRspRec = readReportCfgRspCmd->attrList;
  for ( i = 0; i < readReportCfgRspCmd->numAttr; i++ )
  {
    reportChangeLen = 0;
    dataLen += 1 + 1 + 2 ; // Status, Direction and Atrribute ID
    
    if ( reportRspRec->status == ZCL_STATUS_SUCCESS )
    {
      if ( reportRspRec->direction == SEND_ATTR_REPORTS )
      {
        dataLen += 1 + 2 + 2; // Data Type + Min + Max Reporting Intervals
      
        if ( zclAnalogDataType( reportRspRec->dataType ) )
        {
          reportChangeLen = zclGetDataTypeLength( reportRspRec->dataType );
          dataLen += reportChangeLen; // Reportable Change field
        }
      }
      else
      {
        dataLen += 2; // Timeout Period
      }
    }
    
    reportRspRec = NEXT_REPORT_RSP( reportRspRec, reportChangeLen );
  }
  
  buf = osal_mem_alloc( dataLen );
  if ( buf )
  {
    // Load the buffer - serially
    pBuf = buf;
    reportRspRec = readReportCfgRspCmd->attrList;
    for ( i = 0; i < readReportCfgRspCmd->numAttr; i++ )
    {
      reportChangeLen = 0;
      *pBuf++ = reportRspRec->status;
      *pBuf++ = reportRspRec->direction;
      *pBuf++ = LO_UINT16( reportRspRec->attrID );
      *pBuf++ = HI_UINT16( reportRspRec->attrID );
     
      if ( reportRspRec->status == ZCL_STATUS_SUCCESS )
      {
        if ( reportRspRec->direction == SEND_ATTR_REPORTS )
        {
          *pBuf++ = reportRspRec->dataType;
          *pBuf++ = LO_UINT16( reportRspRec->minReportInt );
          *pBuf++ = HI_UINT16( reportRspRec->minReportInt );
          *pBuf++ = LO_UINT16( reportRspRec->maxReportInt );
          *pBuf++ = HI_UINT16( reportRspRec->maxReportInt );

          if ( zclAnalogDataType( pAttr->attr.dataType ) )
          {
            zclSerializeData( pAttr->attr.dataType, 
                              reportRspRec->reportableChange, pBuf );
            reportChangeLen = zclGetDataTypeLength( pAttr->attr.dataType );
            pBuf += reportChangeLen;
          }
        }
        else
        {
          *pBuf++ = LO_UINT16( reportRspRec->timeoutPeriod );
          *pBuf++ = HI_UINT16( reportRspRec->timeoutPeriod );
        }
      }
      
      reportRspRec = NEXT_REPORT_RSP( reportRspRec, reportChangeLen );
    }
   
    status = zcl_SendCommand( srcEP, dstAddr, realClusterID, FALSE,
                              ZCL_CMD_READ_REPORT_CFG_RSP, FALSE,
                              direction, disableDefaultRsp, 0, seqNum, dataLen, buf );
    osal_mem_free( buf );
  }
  else
    status = ZMemError;
  
  return ( status );
}

/*********************************************************************
 * @fn      zcl_SendReportCmd
 *
 * @brief   Send a Report command
 *
 * @param   dstAddr - destination address
 * @param   realClusterID - real cluster ID
 * @param   reportCmd - report command to be sent
 * @param   direction - direction of the command
 * @param   seqNum - transaction sequence number
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendReportCmd( uint8 srcEP, afAddrType_t *dstAddr,
                             uint16 realClusterID, zclReportCmd_t *reportCmd,
                             uint8 direction, uint8 disableDefaultRsp, uint8 seqNum )
{
  zclReport_t *reportRec;
  uint8 attrDataLen;
  uint8 dataLen = 0;
  uint8 *buf;
  uint8 *pBuf;
  uint8 i;
  ZStatus_t status;
  
  // calculate the size of the command
  reportRec = reportCmd->attrList;
  for ( i = 0; i < reportCmd->numAttr; i++ )
  {
    dataLen += 2 + 1; // Attribute ID + data type

    attrDataLen = zclGetAttrDataLength( reportRec->dataType, reportRec->attrData );
    dataLen += attrDataLen; // Attribute Data
    
    reportRec = NEXT_REPORT( reportRec, attrDataLen );
  }
  
  buf = osal_mem_alloc( dataLen );
  if ( buf )
  {
    // Load the buffer - serially
    pBuf = buf;
    reportRec = reportCmd->attrList;
    for ( i = 0; i < reportCmd->numAttr; i++ )
    {
      *pBuf++ = LO_UINT16( reportRec->attrID );
      *pBuf++ = HI_UINT16( reportRec->attrID );
      *pBuf++ = reportRec->dataType;

      zclSerializeData( reportRec->dataType, reportRec->attrData, pBuf );
      attrDataLen = zclGetAttrDataLength( reportRec->dataType, reportRec->attrData );
      pBuf += attrDataLen; // move pass attribute data
      
      reportRec = NEXT_REPORT( reportRec, attrDataLen );
    }
 
    status = zcl_SendCommand( srcEP, dstAddr, realClusterID, FALSE, ZCL_CMD_REPORT,
                              FALSE, direction, disableDefaultRsp, 0, seqNum, dataLen, buf );
    osal_mem_free( buf );
  }
  else
    status = ZMemError;
  
  return ( status );
}
#endif // ZCL_REPORT
       
/*********************************************************************
 * @fn      zcl_SendDefaultRspCmd
 *
 * @brief   Send a Default Response command
 *
 * @param   dstAddr - destination address
 * @param   realClusterID - real cluster ID
 * @param   defaultRspCmd - default response command to be sent
 * @param   direction - direction of the command
 * @param   seqNum - transaction sequence number
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendDefaultRspCmd( uint8 srcEP, afAddrType_t *dstAddr,
                            uint16 realClusterID, zclDefaultRspCmd_t *defaultRspCmd,
                            uint8 direction, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[2]; // Command ID and Status;

  // Load the buffer - serially
  buf[0] = defaultRspCmd->commandID;
  buf[1] = defaultRspCmd->statusCode;

  return ( zcl_SendCommand( srcEP, dstAddr, realClusterID, FALSE, ZCL_CMD_DEFAULT_RSP,
                            FALSE, direction, disableDefaultRsp, 0, seqNum, 2, buf ) ); 
}

#ifdef ZCL_DISCOVER
/*********************************************************************
 * @fn      zcl_SendDiscoverCmd
 *
 * @brief   Send a Discover command
 *
 * @param   dstAddr - destination address
 * @param   realClusterID - real cluster ID
 * @param   discoverCmd - discover command to be sent
 * @param   direction - direction of the command
 * @param   seqNum - transaction sequence number
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendDiscoverCmd( uint8 srcEP, afAddrType_t *dstAddr,
                            uint16 realClusterID, zclDiscoverCmd_t *discoverCmd,
                            uint8 direction, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 dataLen = 2 + 1; // Start Attribute ID and Max Attribute IDs
  uint8 *buf;
  uint8 *pBuf;
  ZStatus_t status;
  
  buf = osal_mem_alloc( dataLen );
  if ( buf )
  {
    // Load the buffer - serially
    pBuf = buf;
    *pBuf++ = LO_UINT16(discoverCmd->startAttr);
    *pBuf++ = HI_UINT16(discoverCmd->startAttr);
    *pBuf++ = discoverCmd->maxAttrIDs;
    
    status = zcl_SendCommand( srcEP, dstAddr, realClusterID, FALSE, ZCL_CMD_DISCOVER,
                              FALSE, direction, disableDefaultRsp, 0, seqNum, dataLen, buf );
    osal_mem_free( buf );
  }
  else
    status = ZMemError;
  
  return ( status );
}

/*********************************************************************
 * @fn      zcl_SendDiscoverRspCmd
 *
 * @brief   Send a Discover Response command
 *
 * @param   dstAddr - destination address
 * @param   realClusterID - real cluster ID
 * @param   reportRspCmd - report response command to be sent
 * @param   direction - direction of the command
 * @param   seqNum - transaction sequence number
 *
 * @return  ZSuccess if OK
 */
ZStatus_t zcl_SendDiscoverRspCmd( uint8 srcEP, afAddrType_t *dstAddr,
                      uint16 realClusterID, zclDiscoverRspCmd_t *discoverRspCmd,
                      uint8 direction, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 dataLen = 1; // Discovery complete
  uint8 *buf;
  uint8 *pBuf;
  uint8 i;
  ZStatus_t status;
  
  // calculate the size of the command
  dataLen += discoverRspCmd->numAttr * (2 + 1); // Attribute ID and Data Type
  
  buf = osal_mem_alloc( dataLen );
  if ( buf )
  {
    // Load the buffer - serially
    pBuf = buf;
    *pBuf++ = discoverRspCmd->discComplete;    
    for ( i = 0; i < discoverRspCmd->numAttr; i++ )
    {
      *pBuf++ = LO_UINT16(discoverRspCmd->attrList[i].attrID);
      *pBuf++ = HI_UINT16(discoverRspCmd->attrList[i].attrID);
      *pBuf++ = discoverRspCmd->attrList[i].dataType;
    }
    
    status = zcl_SendCommand( srcEP, dstAddr, realClusterID, FALSE, ZCL_CMD_DISCOVER_RSP,
                              FALSE, direction, disableDefaultRsp, 0, seqNum, dataLen, buf );
    osal_mem_free( buf );
  }
  else
    status = ZMemError;
  
  return ( status );
}
#endif // ZCL_DISCOVER

/*********************************************************************
 * PRIVATE FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      zclProcessMessageMSG
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   pkt - incoming message
 *
 * @return  none
 */
static void zclProcessMessageMSG( afIncomingMSGPacket_t *pkt )
{
  endPointDesc_t *epDesc;
  uint16 logicalClusterID;
  zclIncoming_t inMsg;
  zclLibPlugin_t *pInPlugin;
  zclDefaultRspCmd_t defautlRspCmd;
  ZStatus_t status = ZFailure;

  if ( pkt->cmd.DataLength == 0 )
    return;   // Error, ignore the message

  // Initialize
  inMsg.msg = pkt;
  inMsg.attrCmd = NULL;
  inMsg.pData = NULL;
  inMsg.pDataLen = 0;

  inMsg.pData = zclParseHdr( &(inMsg.hdr), pkt->cmd.Data );
  inMsg.pDataLen = pkt->cmd.DataLength;
  inMsg.pDataLen -= (uint8)(inMsg.pData - pkt->cmd.Data);

  // Find the wanted endpoint
  epDesc = afFindEndPointDesc( pkt->endPoint );
  if ( epDesc == NULL )
    return;   // Error, ignore the message

  logicalClusterID = zclConvertClusterID( pkt->clusterId, epDesc->simpleDesc->AppProfId, TRUE );
  if ( logicalClusterID == ZCL_INVALID_CLUSTER_ID )
    return;   // Error, ignore the message

  if ( zcl_DeviceOperational( pkt->endPoint, pkt->clusterId, 
                              inMsg.hdr.fc.type, inMsg.hdr.commandID ) == FALSE )
  {
    return; // Error, ignore the message
  }
 
  // Is this a foundation type message
  if ( zcl_ProfileCmd( inMsg.hdr.fc.type ) )
  {
    if ( inMsg.hdr.fc.manuSpecific )
    {
      // We don't support any manufacturer specific command
      status = ZCL_STATUS_UNSUP_MANU_GENERAL_COMMAND;
    }
    else if ( ( inMsg.hdr.commandID <= ZCL_CMD_MAX ) && 
              ( zclCmdTable[inMsg.hdr.commandID].pfnParseInProfile != NULL ) )
    {
      zclParseCmd_t parseCmd;
      
      parseCmd.endpoint = pkt->endPoint;
      parseCmd.dataLen = inMsg.pDataLen;
      parseCmd.pData = inMsg.pData;
      
      // Parse the command, remember that the return value is a pointer to allocated memory
      inMsg.attrCmd = zclParseCmd( inMsg.hdr.commandID, &parseCmd );
      if ( (inMsg.attrCmd != NULL) && (zclCmdTable[inMsg.hdr.commandID].pfnProcessInProfile != NULL) )
      { 
        // Process the command
        if ( zclProcessCmd( inMsg.hdr.commandID, &inMsg ) == FALSE )
        {
          // Couldn't find attribute in the table.
        }
      }
       
      // Free the buffer
      if ( inMsg.attrCmd )
        osal_mem_free( inMsg.attrCmd );
      
      if ( CMD_HAS_RSP( inMsg.hdr.commandID ) )
        return; // We're done
      
      status = ZSuccess;
    }
    else
    {
      // Unsupported message
      status = ZCL_STATUS_UNSUP_GENERAL_COMMAND;
    }
  }
  else
  {
    // Nope, must be specific to the cluster ID

    // Find the appropriate plugin
    pInPlugin = zclFindPlugin( pkt->clusterId, epDesc->simpleDesc->AppProfId );
    if ( pInPlugin && pInPlugin->pfnIncomingHdlr )
    {
      zclInHdlrMsg_t inHdlrMsg;
      
      inHdlrMsg.msg = &inMsg;
      inHdlrMsg.logicalClusterID = logicalClusterID;
      
      status = pInPlugin->pfnIncomingHdlr( &inHdlrMsg );
      if ( status == ZCL_STATUS_CMD_HAS_RSP )
        return; // We're done
    }

    if ( status != ZSuccess )
    {
      // Unsupported message
      if ( inMsg.hdr.fc.manuSpecific )
        status = ZCL_STATUS_UNSUP_MANU_CLUSTER_COMMAND;
      else
        status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
    }
  }
  
  if ( UNICAST_MSG( inMsg.msg ) && inMsg.hdr.fc.disableDefaultRsp == 0 )
  {
    // Send a Default Response command back
    defautlRspCmd.statusCode = status;
    defautlRspCmd.commandID = inMsg.hdr.commandID;
    zcl_SendDefaultRspCmd( inMsg.msg->endPoint, &(inMsg.msg->srcAddr),
                           inMsg.msg->clusterId, &defautlRspCmd, 
                           ZCL_FRAME_SERVER_CLIENT_DIR, true, inMsg.hdr.transSeqNum );
  }
}

/*********************************************************************
 * @fn      zclParseHdr
 *
 * @brief   Parse header of the ZCL format
 *
 * @param   hdr - place to put the frame control information
 * @param   pData - incoming buffer to parse
 *
 * @return  pointer past the header
 */
uint8 *zclParseHdr( zclFrameHdr_t *hdr, uint8 *pData )
{
  // Clear the header
  osal_memset( (uint8 *)hdr, 0, sizeof ( zclFrameHdr_t ) );

  // Parse the Frame Control
  hdr->fc.type = zcl_FCType( *pData );
  hdr->fc.manuSpecific = zcl_FCManuSpecific( *pData ) ? 1 : 0;
  if ( zcl_FCDirection( *pData ) )
    hdr->fc.direction = ZCL_FRAME_SERVER_CLIENT_DIR;
  else
    hdr->fc.direction = ZCL_FRAME_CLIENT_SERVER_DIR;
  
  hdr->fc.disableDefaultRsp = zcl_FCDisableDefaultRsp( *pData ) ? 1 : 0;
  pData++;  // move past the frame control field

  // parse the manfacturer code
  if ( hdr->fc.manuSpecific )
  {
    hdr->manuCode = BUILD_UINT16( pData[0], pData[1] );
    pData += 2;
  }

  // parse the Transaction Sequence Number
  hdr->transSeqNum = *pData++;

  // parse the Cluster's command ID
  hdr->commandID = *pData++;

  // Should point to the frame payload
  return ( pData );
}

/*********************************************************************
 * @fn      zclBuildHdr
 *
 * @brief   Build header of the ZCL format
 *
 * @param   hdr - outgoing header information
 * @param   pData - outgoing header space
 *
 * @return  pointer past the header
 */
static uint8 *zclBuildHdr( zclFrameHdr_t *hdr, uint8 *pData )
{
  // Build the Frame Control byte
  *pData = hdr->fc.type;
  *pData |= hdr->fc.manuSpecific << 2;
  *pData |= hdr->fc.direction << 3;
  *pData |= hdr->fc.disableDefaultRsp << 4;
  pData++;  // move past the frame control field

  // Add the manfacturer code
  if ( hdr->fc.manuSpecific )
  {
    *pData++ = LO_UINT16( hdr->manuCode );
    *pData++ = HI_UINT16( hdr->manuCode );
  }

  // Add the Transaction Sequence Number
  *pData++ = hdr->transSeqNum;
  
  // Add the Cluster's command ID
  *pData++ = hdr->commandID;

  // Should point to the frame payload
  return ( pData );
}

/*********************************************************************
 * @fn      zclCalcHdrSize
 *
 * @brief   Calculate the number of bytes needed for an outgoing
 *          ZCL header.
 *
 * @param   hdr - outgoing header information
 *
 * @return  returns the number of bytes needed
 */
static uint8 zclCalcHdrSize( zclFrameHdr_t *hdr )
{
  uint8 needed = (1 + 1 + 1); // frame control + transaction seq num + cmd ID

  // Add the manfacturer code
  if ( hdr->fc.manuSpecific )
    needed += 2;

  return ( needed );
}

/*********************************************************************
 * @fn      zclConvertClusterID
 *
 * @brief   Convert to a real or locigal Cluster ID
 *
 * @param   ClusterID - cluster ID to convert from
 * @param   profileID - profile ID
 * @param   convertToLogical - TRUE to convert to logical cluster ID
 *                             FALSE to convert to real cluster ID
 *
 * @return  converted cluster ID, ZCL_INVALID_CLUSTER_ID if not found
 */
static uint16 zclConvertClusterID( uint16 clusterID, uint16 profileID,
                                   uint8 convertToLogical )
{
  uint8 x;
  zclProfileClusterConvertRec_t *pLoop;

  pLoop = profileClusterList;

  while ( pLoop )
  {
    if ( pLoop->profileID == profileID )
    {
      for ( x = 0; x < pLoop->numClusters; x++ )
      {
        if ( convertToLogical && pLoop->list[x].actualCluster == clusterID )
          return ( pLoop->list[x].logicalCluster );
        if ( !convertToLogical && pLoop->list[x].logicalCluster == clusterID )
          return ( pLoop->list[x].actualCluster );
      }
    }
    pLoop = pLoop->next;
  }

  return ( (uint16)ZCL_INVALID_CLUSTER_ID );
}

/*********************************************************************
 * @fn      zclFindPlugin
 *
 * @brief   Find the right plugin for a REAL cluster ID
 *
 * @param   realClusterID - cluster ID to look for
 * @param   profileID - profile ID
 * 
 * @return  converted cluster ID, ZCL_INVALID_CLUSTER_ID if not found
 */
static zclLibPlugin_t *zclFindPlugin( uint16 realClusterID, uint16 profileID )
{
  zclLibPlugin_t *pLoop;
  uint16 logicalClusterID;

  logicalClusterID = zclConvertClusterID( realClusterID, profileID, TRUE );
  if ( logicalClusterID != ZCL_INVALID_CLUSTER_ID )
  {
    pLoop = plugins;
    while ( pLoop )
    {
      if ( logicalClusterID >= pLoop->startLogCluster && logicalClusterID <= pLoop->endLogCluster )
        return ( pLoop );
      pLoop = pLoop->next;
    }
  }
  return ( (zclLibPlugin_t *)NULL );
}

/*********************************************************************
 * @fn      zclFindAttrRec
 *
 * @brief   Find the attribute record that matchs the parameters
 *
 * @param   endpoint - Application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   attrId - attribute looking for
 *
 * @return  pointer to attribute record, NULL if not found
 */
uint8 zclFindAttrRec( uint8 endpoint, uint16 realClusterID, uint16 attrId, zclAttrRec_t *pAttr )
{
  uint8 x;
  zclAttrRecsList *pLoop;

  pLoop = attrList;

  while ( pLoop )
  {
    if ( pLoop->endpoint == endpoint )
    {
      for ( x = 0; x < pLoop->numAttributes; x++ )
      {
        if ( pLoop->attrs[x].clusterID == realClusterID && pLoop->attrs[x].attr.attrId == attrId )
        {
          *pAttr = pLoop->attrs[x];
          return ( TRUE ); // EMBEDDED RETURN
        }
      }
    }
    pLoop = pLoop->next;
  }

  return ( FALSE );
}

#ifdef ZCL_DISCOVER
/*********************************************************************
 * @fn      zclFindNextAttrRec
 *
 * @brief   Find the attribute (or next) record that matchs the parameters
 *
 * @param   endpoint - Application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   attr - attribute looking for
 *
 * @return  pointer to attribute record, NULL if not found
 */
static uint8 zclFindNextAttrRec( uint8 endpoint, uint16 realClusterID,
                                 uint16 *attrId, zclAttrRec_t *pAttr )
{
  uint16 x;
  zclAttrRecsList *pLoop;

  pLoop = attrList;

  while ( pLoop )
  {
    if ( pLoop->endpoint == endpoint )
    {
      for ( x = 0; x < pLoop->numAttributes; x++ )
      {
        if ( pLoop->attrs[x].clusterID == realClusterID && pLoop->attrs[x].attr.attrId >= *attrId )
        {
          *pAttr = pLoop->attrs[x];
          
          // Update attribute ID
          *attrId = pAttr->attr.attrId; 
          return ( TRUE ); // EMBEDDED RETURN
        }
      }
    }
    pLoop = pLoop->next;
  }

  return ( FALSE );
}
#endif // ZCL_DISCOVER

#if defined(ZCL_READ) || defined(ZCL_WRITE) || defined(ZCL_REPORT)
/*********************************************************************
 * @fn      zclSerializeData
 *
 * @brief   Builds a buffer from the attribute data to sent out over
 *          the air.
 *
 * @param   dataType - data types defined in zcl.h
 * @param   attrData - pointer to the attribute data
 * @param   buf - where to put the serialized data
 *
 * @return  none
 */
static void zclSerializeData( uint8 dataType, void *attrData, uint8 *buf )
{
  uint8 *pStr;
  uint8 len;

  switch ( dataType )
  {
    case ZCL_DATATYPE_DATA8:
    case ZCL_DATATYPE_BOOLEAN:
    case ZCL_DATATYPE_BITMAP8:
    case ZCL_DATATYPE_INT8:
    case ZCL_DATATYPE_UINT8:
    case ZCL_DATATYPE_ENUM8:
      *buf = *((uint8 *)attrData);
       break;

    case ZCL_DATATYPE_DATA16:
    case ZCL_DATATYPE_BITMAP16:
    case ZCL_DATATYPE_UINT16:
    case ZCL_DATATYPE_INT16: 
    case ZCL_DATATYPE_ENUM16:
    case ZCL_DATATYPE_SEMI_PREC:
    case ZCL_DATATYPE_CLUSTER_ID:
    case ZCL_DATATYPE_ATTR_ID:
      *buf++ = LO_UINT16( *((uint16*)attrData) );
      *buf++ = HI_UINT16( *((uint16*)attrData) );
      break;

    case ZCL_DATATYPE_DATA24:
    case ZCL_DATATYPE_BITMAP24: 
    case ZCL_DATATYPE_UINT24:
    case ZCL_DATATYPE_INT24:
      *buf++ = BREAK_UINT32( *((uint32*)attrData), 0 );
      *buf++ = BREAK_UINT32( *((uint32*)attrData), 1 );
      *buf++ = BREAK_UINT32( *((uint32*)attrData), 2 );
      break;
      
    case ZCL_DATATYPE_DATA32:
    case ZCL_DATATYPE_BITMAP32:
    case ZCL_DATATYPE_UINT32:
    case ZCL_DATATYPE_INT32:
    case ZCL_DATATYPE_SINGLE_PREC:
    case ZCL_DATATYPE_TOD:
    case ZCL_DATATYPE_DATE:
    case ZCL_DATATYPE_BAC_OID:
      *buf++ = BREAK_UINT32( *((uint32*)attrData), 0 );
      *buf++ = BREAK_UINT32( *((uint32*)attrData), 1 );
      *buf++ = BREAK_UINT32( *((uint32*)attrData), 2 );
      *buf++ = BREAK_UINT32( *((uint32*)attrData), 3 );
      break;

    // case ZCL_DATATYPE_DOUBLE_PREC:
       // size of 8 octects
       // break;
      
    case ZCL_DATATYPE_IEEE_ADDR:
      pStr = (uint8*)attrData;
      osal_memcpy( buf, pStr, 8 );
      break;
      
    case ZCL_DATATYPE_CHAR_STR:
    case ZCL_DATATYPE_OCTET_STR:
      pStr = (uint8*)attrData;
      len = *pStr++;
      *buf++ = len;
      osal_memcpy( buf, pStr, len );
      break;
      
    case ZCL_DATATYPE_NO_DATA:
    case ZCL_DATATYPE_UNKNOWN:
      // Fall through

    default:
      break;
  }
}
#endif // ZCL_READ || ZCL_WRITE || ZCL_REPORT

#ifdef ZCL_REPORT
/*********************************************************************
 * @fn      zclAnalogDataType
 *
 * @brief   Checks to see if Data Type is Analog
 *
 * @param   dataType - data type
 *
 * @return  TRUE if data type is analog
 */
static uint8 zclAnalogDataType( uint8 dataType )
{
  uint8 analog;
  
  switch ( dataType )
  {
    case ZCL_DATATYPE_UINT8:
    case ZCL_DATATYPE_UINT16:
    case ZCL_DATATYPE_UINT24:
    case ZCL_DATATYPE_UINT32:
    case ZCL_DATATYPE_INT8:
    case ZCL_DATATYPE_INT16:
    case ZCL_DATATYPE_INT24:
    case ZCL_DATATYPE_INT32:
    case ZCL_DATATYPE_SEMI_PREC:
    case ZCL_DATATYPE_SINGLE_PREC:
    case ZCL_DATATYPE_DOUBLE_PREC:
    case ZCL_DATATYPE_TOD:
    case ZCL_DATATYPE_DATE:
      analog = TRUE;
      break;
      
    default:
      analog = FALSE;
      break;
  }
  
  return ( analog );
}

/*********************************************************************
 * @fn      zcl_BuildAnalogData
 *
 * @brief   Build an analog arribute out of sequential bytes.
 *
 * @param   dataType - type of data
 * @param   pData - pointer to data
 * @param   pBuf - where to put the data
 *
 * @return  none
 */
static void zcl_BuildAnalogData( uint8 dataType, uint8 *pData, uint8 *pBuf)
{
  switch ( dataType )
  {
    case ZCL_DATATYPE_UINT8:
    case ZCL_DATATYPE_INT8:
      *pData = *pBuf;
      break;

    case ZCL_DATATYPE_UINT16:
    case ZCL_DATATYPE_INT16:
    case ZCL_DATATYPE_SEMI_PREC:
      *((uint16*)pData) = BUILD_UINT16( pBuf[0], pBuf[1] ); 
      break;
 
    case ZCL_DATATYPE_UINT24:
    case ZCL_DATATYPE_INT24:
      *((uint32*)pData) = osal_build_uint32( pBuf, 3 );
      break;
      
    case ZCL_DATATYPE_UINT32:
    case ZCL_DATATYPE_INT32:
    case ZCL_DATATYPE_SINGLE_PREC:
    case ZCL_DATATYPE_TOD:
    case ZCL_DATATYPE_DATE:
      *((uint32*)pData) = osal_build_uint32( pBuf, 4 );
      break;
      
    case ZCL_DATATYPE_DOUBLE_PREC:
      //???? 8 octects
      *pData = 0;
      break;
 
    default:
      break;
  }
}
#endif // ZCL_REPORT

/*********************************************************************
 * @fn      zclGetDataTypeLength
 *
 * @brief   Return the length of the datatype in length. 
 *          NOTE: Should not be called for ZCL_DATATYPE_OCTECT_STR or 
 *                ZCL_DATATYPE_CHAR_STR data types.
 *
 * @param   dataType - data type
 *
 * @return  length of data
 */
static uint8 zclGetDataTypeLength( uint8 dataType )
{
  uint8 len;
  
  switch ( dataType )
  {
    case ZCL_DATATYPE_DATA8:
    case ZCL_DATATYPE_BOOLEAN:
    case ZCL_DATATYPE_BITMAP8:
    case ZCL_DATATYPE_INT8:
    case ZCL_DATATYPE_UINT8:
    case ZCL_DATATYPE_ENUM8:
      len = 1;
      break;
      
    case ZCL_DATATYPE_DATA16:
    case ZCL_DATATYPE_BITMAP16:
    case ZCL_DATATYPE_UINT16:
    case ZCL_DATATYPE_INT16: 
    case ZCL_DATATYPE_ENUM16:
    case ZCL_DATATYPE_SEMI_PREC:
    case ZCL_DATATYPE_CLUSTER_ID:
    case ZCL_DATATYPE_ATTR_ID:
      len = 2;
      break;
      
    case ZCL_DATATYPE_DATA24:
    case ZCL_DATATYPE_BITMAP24: 
    case ZCL_DATATYPE_UINT24:
    case ZCL_DATATYPE_INT24:
      len = 3;
      break;
      
    case ZCL_DATATYPE_DATA32:
    case ZCL_DATATYPE_BITMAP32:
    case ZCL_DATATYPE_UINT32:
    case ZCL_DATATYPE_INT32:
    case ZCL_DATATYPE_SINGLE_PREC:
    case ZCL_DATATYPE_TOD:
    case ZCL_DATATYPE_DATE:
    case ZCL_DATATYPE_BAC_OID:
      len = 4;
      break;                       
                
   case ZCL_DATATYPE_DOUBLE_PREC:
   case ZCL_DATATYPE_IEEE_ADDR:
     len = 8;
     break;

    case ZCL_DATATYPE_NO_DATA:
    case ZCL_DATATYPE_UNKNOWN:
      // Fall through
      
    default:
      len = 0;
      break;
  }
  
  return ( len );
}

/*********************************************************************
 * @fn      zclGetAttrDataLength
 *
 * @brief   Return the length of the attribute.
 *
 * @param   dataType - data type
 * @param   pData - pointer to data
 *
 * @return  returns atrribute lentgh
 */
static uint8 zclGetAttrDataLength( uint8  dataType, uint8 *pData)
{
  uint8 dataLen = 0;
  
  if ( dataType  == ZCL_DATATYPE_CHAR_STR || dataType == ZCL_DATATYPE_OCTET_STR )
  {
    dataLen = *pData + 1; // string length + 1 for length field
  }
  else
  {
    dataLen = zclGetDataTypeLength( dataType );
  }

  return ( dataLen );
}

/*********************************************************************
 * @fn      zclReadAttrData
 *
 * @brief   Read the attribute's current value into pAttrData.
 *
 * @param   pAttrData - where to put attribute data
 * @param   pAttr - pointer to attribute
 *
 * @return Success
 */
uint8 zclReadAttrData( uint8 *pAttrData, zclAttrRec_t *pAttr )
{
  uint8 dataLen;
    
  dataLen = zclGetAttrDataLength( pAttr->attr.dataType, (uint8*)(pAttr->attr.dataPtr) );   
  osal_memcpy( pAttrData, pAttr->attr.dataPtr, dataLen );
    
  return ( ZCL_STATUS_SUCCESS );
}

#ifdef ZCL_WRITE
/*********************************************************************
 * @fn      zclWriteAttrData
 *
 * @brief   Write the received data.
 *
 * @param   pWriteRec - data to be written
 * @param   pAttr - where to write data to
 *
 * @return  Successful if data was written
 */
static uint8 zclWriteAttrData( zclAttrRec_t *pAttr, zclWriteRec_t *pWriteRec )
{
  uint8 len;

  if ( zcl_AccessCtrlWrite( pAttr->attr.accessControl ) )
  {
    if ( zcl_ValidateAttrDataCB && !zcl_ValidateAttrDataCB( pWriteRec ) )
      return ( ZCL_STATUS_INVALID_VALUE );
    
    len = zclGetAttrDataLength( pAttr->attr.dataType, pWriteRec->attrData );
    osal_memcpy( pAttr->attr.dataPtr, pWriteRec->attrData, len );
    return ( ZCL_STATUS_SUCCESS );
  }
    
  return ( ZCL_STATUS_READ_ONLY );
}
#endif // ZCL_WRITE

#ifdef ZCL_READ
/*********************************************************************
 * @fn      zclParseInReadCmd
 *
 * @brief   Parse the "Profile" Read Commands
 *
 *      NOTE: THIS FUNCTION ALLOCATES THE RETURN BUFFER, SO THE CALLING
 *            FUNCTION IS RESPONSIBLE TO FREE THE MEMORY.
 *
 * @param   endpoint - application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   dataLen - number of bytes in incoming data buffer
 * @param   pData - pointer to incoming data to parse
 *
 * @return  pointer to the parsed command structure
 */
void *zclParseInReadCmd( zclParseCmd_t *pCmd )
{
  zclReadCmd_t *readCmd;
  uint8 *pBuf = pCmd->pData;
  
  readCmd = (zclReadCmd_t *)osal_mem_alloc( sizeof ( zclReadCmd_t ) + pCmd->dataLen );
  if ( readCmd )
  {
    uint8 i;
    
    readCmd->numAttr = pCmd->dataLen / 2; // Atrribute ID
    for ( i = 0; i < readCmd->numAttr; i++ )
    {
      readCmd->attrID[i] = BUILD_UINT16( pBuf[0], pBuf[1] );
      pBuf += 2;
    }
  }

  return ( (void *)readCmd );
}

/*********************************************************************
 * @fn      zclParseInReadRspCmd
 *
 * @brief   Parse the "Profile" Read Response Commands
 *
 *      NOTE: THIS FUNCTION ALLOCATES THE RETURN BUFFER, SO THE CALLING
 *            FUNCTION IS RESPONSIBLE TO FREE THE MEMORY.
 *
 * @param   endpoint - application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   dataLen - number of bytes in incoming data buffer
 * @param   pData - pointer to incoming data to parse
 *
 * @return  pointer to the parsed command structure
 */
static void *zclParseInReadRspCmd( zclParseCmd_t *pCmd )
{
  zclReadRspCmd_t *readRspCmd;
  zclReadRspStatus_t *statusRec;
  uint8 *pBuf = pCmd->pData;
  uint8 attrDataLen = 0;
  
  readRspCmd = (zclReadRspCmd_t *)osal_mem_alloc( sizeof ( zclReadRspCmd_t ) + pCmd->dataLen );
  if ( readRspCmd )
  {
    readRspCmd->numAttr = 0;
    statusRec = readRspCmd->attrList;
    while ( pBuf < ( pCmd->pData + pCmd->dataLen ) )
    {
      readRspCmd->numAttr++;

      statusRec->attrID = BUILD_UINT16( pBuf[0], pBuf[1] );
      pBuf += 2;
      statusRec->status = *pBuf++;
      
      if ( statusRec->status == ZCL_STATUS_SUCCESS )
      {
        statusRec->dataType = *pBuf++;

        attrDataLen = zclGetAttrDataLength( statusRec->dataType, pBuf );
        osal_memcpy( statusRec->data, pBuf, attrDataLen);
        pBuf += attrDataLen; // move pass attribute data
      }
      else
        attrDataLen = 0;
      statusRec = NEXT_READ_RSP(statusRec, attrDataLen);
    }
  }

  return ( (void *)readRspCmd );
}
#endif // ZCL_READ

#ifdef ZCL_WRITE
/*********************************************************************
 * @fn      zclParseInWriteCmd
 *
 * @brief   Parse the "Profile" Write, Write Undivided and Write No
 *          Response Commands
 *
 *      NOTE: THIS FUNCTION ALLOCATES THE RETURN BUFFER, SO THE CALLING
 *            FUNCTION IS RESPONSIBLE TO FREE THE MEMORY.
 * 
 * @param   endpoint - application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   dataLen - number of bytes in incoming data buffer
 * @param   pData - pointer to incoming data to parse
 *
 * @return  pointer to the parsed command structure
 */
void *zclParseInWriteCmd( zclParseCmd_t *pCmd )
{
  zclWriteCmd_t *writeCmd;
  zclWriteRec_t *statusRec;
  uint8 *pBuf = pCmd->pData;
  uint8 attrDataLen;
  
  writeCmd = (zclWriteCmd_t *)osal_mem_alloc( sizeof ( zclWriteCmd_t ) + pCmd->dataLen );
  if ( writeCmd )
  {
    writeCmd->numAttr = 0;
    statusRec = writeCmd->attrList;
    while ( pBuf < ( pCmd->pData + pCmd->dataLen ) )
    {
      writeCmd->numAttr++; 
      
      statusRec->attrID = BUILD_UINT16( pBuf[0], pBuf[1] );
      pBuf += 2;
      statusRec->dataType = *pBuf++;

      attrDataLen = zclGetAttrDataLength( statusRec->dataType, pBuf );
      osal_memcpy( statusRec->attrData, pBuf, attrDataLen );
      pBuf += attrDataLen; // move pass attribute data
      statusRec = NEXT_WRITE( statusRec, attrDataLen );
    }
  }
  
  return ( (void *)writeCmd );
}

/*********************************************************************
 * @fn      zclParseInWriteRspCmd
 *
 * @brief   Parse the "Profile" Write Response Commands
 *
 *      NOTE: THIS FUNCTION ALLOCATES THE RETURN BUFFER, SO THE CALLING
 *            FUNCTION IS RESPONSIBLE TO FREE THE MEMORY.
 *
 * @param   endpoint - application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   dataLen - number of bytes in incoming data buffer
 * @param   pData - pointer to incoming data to parse
 *
 * @return  pointer to the parsed command structure
 */
static void *zclParseInWriteRspCmd( zclParseCmd_t *pCmd )
{
  zclWriteRspCmd_t *writeRspCmd;
  uint8 *pBuf = pCmd->pData;
  uint8 i = 0;

  writeRspCmd = (zclWriteRspCmd_t *)osal_mem_alloc( sizeof ( zclWriteRspCmd_t ) + pCmd->dataLen );
  if ( writeRspCmd )
  {
    if ( pCmd->dataLen == 1 )
    {
      // special case when all writes were successfull
      writeRspCmd->attrList[i++].status = *pBuf;
    }
    else
    {
      while ( pBuf < ( pCmd->pData + pCmd->dataLen ) )
      {
        writeRspCmd->attrList[i].status = *pBuf++;
        writeRspCmd->attrList[i++].attrID = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
      }
    }
    
    writeRspCmd->numAttr = i; 
  }

  return ( (void *)writeRspCmd );
}
#endif // ZCL_WRITE

#ifdef ZCL_REPORT
/*********************************************************************
 * @fn      zclParseInConfigReportCmd
 *
 * @brief   Parse the "Profile" Configure Reporting Command
 *
 *      NOTE: THIS FUNCTION ALLOCATES THE RETURN BUFFER, SO THE CALLING
 *            FUNCTION IS RESPONSIBLE TO FREE THE MEMORY.
 *
 * @param   endpoint - application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   dataLen - number of bytes in incoming data buffer
 * @param   pData - pointer to incoming data to parse
 *
 * @return  pointer to the parsed command structure
 */
void *zclParseInConfigReportCmd( zclParseCmd_t *pCmd )
{
  zclCfgReportCmd_t *cfgReportCmd;
  zclCfgReportRec_t *reportRec;
  uint8 *pBuf = pCmd->pData;
  uint8 direction;
  uint8 dataType;
  uint8 len = sizeof ( zclCfgReportCmd_t );
  uint8 reportChangeLen; // length of Reportable Change field
  
  // Calculate the length of the Request command
  while ( pBuf < ( pCmd->pData + pCmd->dataLen ) )
  {
    len += sizeof ( zclCfgReportRec_t );
    
    direction = *pBuf++;
    pBuf += 2; // move pass the attribute ID
    
    // Is there a Reportable Change field?
    if ( direction == SEND_ATTR_REPORTS )
    {
      dataType = *pBuf++;
      pBuf += 4; // move pass the Min and Max Reporting Intervals

      // For attributes of 'discrete' data types this field is omitted
      if ( zclAnalogDataType( dataType ) )
      {
        reportChangeLen = zclGetDataTypeLength( dataType );
        pBuf += reportChangeLen;
        len += reportChangeLen;
      }
    }
    else
    {
      pBuf += 2; // move pass the Timeout Period
    }
  } // while loop

  cfgReportCmd = (zclCfgReportCmd_t *)osal_mem_alloc( len );
  if ( cfgReportCmd )
  { 
    pBuf = pCmd->pData;
    cfgReportCmd->numAttr = 0;
    reportRec = cfgReportCmd->attrList;
    while ( pBuf < ( pCmd->pData + pCmd->dataLen ) )
    {
      cfgReportCmd->numAttr++; 
      reportChangeLen = 0; 
      
      osal_memset( reportRec, 0, sizeof( zclCfgReportRec_t ) );
        
      reportRec->direction = *pBuf++;
      reportRec->attrID = BUILD_UINT16( pBuf[0], pBuf[1] );
      pBuf += 2;
      if ( reportRec->direction == SEND_ATTR_REPORTS )
      {
        // Attribute to be reported
        reportRec->dataType = *pBuf++;
        reportRec->minReportInt = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
        reportRec->maxReportInt = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;

        // For attributes of 'discrete' data types this field is omitted
        if ( zclAnalogDataType( reportRec->dataType ) )
        {
          zcl_BuildAnalogData( reportRec->dataType, reportRec->reportableChange, pBuf);
          reportChangeLen = zclGetDataTypeLength( reportRec->dataType );
          pBuf += reportChangeLen;
        }
      }
      else
      {
        // Attribute reports to be received
        reportRec->timeoutPeriod = BUILD_UINT16( pBuf[0], pBuf[1] );
        pBuf += 2;
      }
      
      reportRec = NEXT_CFG_REPORT( reportRec, reportChangeLen );
    } // while loop
  }
  
  return ( (void *)cfgReportCmd );
}

/*********************************************************************
 * @fn      zclParseInConfigReportRspCmd
 *
 * @brief   Parse the "Profile" Configure Reporting Response Command
 *
 *      NOTE: THIS FUNCTION ALLOCATES THE RETURN BUFFER, SO THE CALLING
 *            FUNCTION IS RESPONSIBLE TO FREE THE MEMORY.
 *
 * @param   endpoint - application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   dataLen - number of bytes in incoming data buffer
 * @param   pData - pointer to incoming data to parse
 *
 * @return  pointer to the parsed command structure
 */
static void *zclParseInConfigReportRspCmd( zclParseCmd_t *pCmd )
{
  zclCfgReportRspCmd_t *cfgReportRspCmd;
  uint8 *pBuf = pCmd->pData;
  uint8 i; 
  
  cfgReportRspCmd = (zclCfgReportRspCmd_t *)osal_mem_alloc( sizeof ( zclCfgReportRspCmd_t ) + \
                                                            pCmd->dataLen );
  if ( cfgReportRspCmd )
  {
    cfgReportRspCmd->numAttr = pCmd->dataLen / 4; // Status + Direction + Attribute ID
    for ( i = 0; i < cfgReportRspCmd->numAttr; i++ )
    {
      cfgReportRspCmd->attrList[i].status = *pBuf++;
      cfgReportRspCmd->attrList[i].direction = *pBuf++;
      cfgReportRspCmd->attrList[i].attrID = BUILD_UINT16( pBuf[0], pBuf[1] );
      pBuf += 2;
    }
  }

  return ( (void *)cfgReportRspCmd );  
}

/*********************************************************************
 * @fn      zclParseInReadReportCfgCmd
 *
 * @brief   Parse the "Profile" Read Reporting Configuration Command
 *
 *      NOTE: THIS FUNCTION ALLOCATES THE RETURN BUFFER, SO THE CALLING
 *            FUNCTION IS RESPONSIBLE TO FREE THE MEMORY.
 *
 * @param   endpoint - application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   dataLen - number of bytes in incoming data buffer
 * @param   pData - pointer to incoming data to parse
 *
 * @return  pointer to the parsed command structure
 */
void *zclParseInReadReportCfgCmd( zclParseCmd_t *pCmd )
{
  zclReadReportCfgCmd_t *readReportCfgCmd;
  uint8 *pBuf = pCmd->pData;
  uint8 i;
  
  readReportCfgCmd = (zclReadReportCfgCmd_t *)osal_mem_alloc( sizeof ( zclReadReportCfgCmd_t ) + \
                                                              pCmd->dataLen );
  if ( readReportCfgCmd )
  {
    readReportCfgCmd->numAttr = pCmd->dataLen / 3; // Direction + Attribute ID
    for ( i = 0; i < readReportCfgCmd->numAttr; i++)
    {
      readReportCfgCmd->attrList[i].direction = *pBuf++;;
      readReportCfgCmd->attrList[i].attrID = BUILD_UINT16( pBuf[0], pBuf[1] );
      pBuf += 2;
    }
  }
  
  return ( (void *)readReportCfgCmd );
}

/*********************************************************************
 * @fn      zclParseInReadReportCfgRspCmd
 *
 * @brief   Parse the "Profile" Read Reporting Configuration Response Command
 *
 *      NOTE: THIS FUNCTION ALLOCATES THE RETURN BUFFER, SO THE CALLING
 *            FUNCTION IS RESPONSIBLE TO FREE THE MEMORY.
 *
 * @param   endpoint - application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   dataLen - number of bytes in incoming data buffer
 * @param   pData - pointer to incoming data to parse
 *
 * @return  pointer to the parsed command structure
 */
static void *zclParseInReadReportCfgRspCmd( zclParseCmd_t *pCmd )
{
  zclReadReportCfgRspCmd_t *readReportCfgRspCmd;
  zclReportCfgRspRec_t *reportRspRec;
  uint8 reportChangeLen;
  uint8 len = sizeof ( zclReadReportCfgRspCmd_t );
  uint8 *pBuf = pCmd->pData;
  uint8 status;
  uint8 direction;
  uint8 dataType;
  
  // Calculate the length of the response command
  while ( pBuf < ( pCmd->pData + pCmd->dataLen ) )
  {
    len += sizeof ( zclReportCfgRspRec_t );
    
    status = *pBuf++;
    direction = *pBuf++; 
    pBuf += 2; // move pass the attribute ID
    
    if ( status == ZCL_STATUS_SUCCESS )
    {
      if ( direction == SEND_ATTR_REPORTS )
      {
        pBuf += 1 + 2 + 2; // move pass Data Type, Min and Max Reporting Intervals
        dataType = *pBuf++;
 
        // For attributes of 'discrete' data types this field is omitted
        if ( zclAnalogDataType( dataType ) )
        {
          reportChangeLen = zclGetDataTypeLength( dataType );
          pBuf += reportChangeLen;
          len += reportChangeLen;
        }
      }
      else
      {
        pBuf += 2; // move pass the Timeout field
      }
    }
  } // while loop
  
  readReportCfgRspCmd = (zclReadReportCfgRspCmd_t *)osal_mem_alloc( len );
  if ( readReportCfgRspCmd )
  {
    pBuf = pCmd->pData;
    readReportCfgRspCmd->numAttr = 0;
    reportRspRec = readReportCfgRspCmd->attrList;
    while ( pBuf < ( pCmd->pData + pCmd->dataLen ) )
    {
      reportChangeLen = 0;
      readReportCfgRspCmd->numAttr++;
      
      reportRspRec->status = *pBuf++;
      reportRspRec->direction = *pBuf++;
      reportRspRec->attrID = BUILD_UINT16( pBuf[0], pBuf[1] );
      pBuf += 2;
 
      if ( reportRspRec->status == ZCL_STATUS_SUCCESS )
      {
        if ( reportRspRec->direction == SEND_ATTR_REPORTS )
        {
          reportRspRec->dataType = *pBuf++;
          reportRspRec->minReportInt = BUILD_UINT16( pBuf[0], pBuf[1] );
          pBuf += 2;
          reportRspRec->maxReportInt = BUILD_UINT16( pBuf[0], pBuf[1] );
          pBuf += 2;

          if ( zclAnalogDataType( reportRspRec->dataType ) )
          {
            zcl_BuildAnalogData( reportRspRec->dataType, reportRspRec->reportableChange, pBuf);
            reportChangeLen = zclGetDataTypeLength( reportRspRec->dataType ); 
            pBuf += reportChangeLen;
          }
        }
        else
        {
          reportRspRec->timeoutPeriod = BUILD_UINT16( pBuf[0], pBuf[1] );
          pBuf += 2;
        }
      }
      
      reportRspRec = NEXT_REPORT_RSP( reportRspRec, reportChangeLen );
    } 
  }
  
  return ( (void *)readReportCfgRspCmd );
}

/*********************************************************************
 * @fn      zclParseInReportCmd
 *
 * @brief   Parse the "Profile" Report Command
 *
 *      NOTE: THIS FUNCTION ALLOCATES THE RETURN BUFFER, SO THE CALLING
 *            FUNCTION IS RESPONSIBLE TO FREE THE MEMORY.
 *
 * @param   endpoint - application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   dataLen - number of bytes in incoming data buffer
 * @param   pData - pointer to incoming data to parse
 *
 * @return  pointer to the parsed command structure
 */
void *zclParseInReportCmd( zclParseCmd_t *pCmd )
{
  zclReportCmd_t *reportCmd;
  zclReport_t *reportRec;
  uint8 *pBuf = pCmd->pData;
  uint8 attrDataLen;
  
  reportCmd = (zclReportCmd_t *)osal_mem_alloc( sizeof (zclReportCmd_t) + pCmd->dataLen );
  if (reportCmd )
  {
    reportCmd->numAttr = 0;
    reportRec = reportCmd->attrList;
    while ( pBuf < ( pCmd->pData + pCmd->dataLen ) )
    {
      reportCmd->numAttr++;
      
      reportRec->attrID = BUILD_UINT16( pBuf[0], pBuf[1] );
      pBuf += 2;
      reportRec->dataType = *pBuf++;

      attrDataLen = zclGetAttrDataLength( reportRec->dataType, pBuf );  
      osal_memcpy( reportRec->attrData, pBuf, attrDataLen );
      pBuf += attrDataLen; // move pass attribute data
      
      reportRec = NEXT_REPORT( reportRec, attrDataLen );
    }
  }
  
  return ( (void *)reportCmd );
}
#endif // ZCL_REPORT

/*********************************************************************
 * @fn      zclParseInDefaultRspCmd
 *
 * @brief   Parse the "Profile" Default Response Command
 *
 *      NOTE: THIS FUNCTION ALLOCATES THE RETURN BUFFER, SO THE CALLING
 *            FUNCTION IS RESPONSIBLE TO FREE THE MEMORY.
 *
 * @param   endpoint - application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   dataLen - number of bytes in incoming data buffer
 * @param   pData - pointer to incoming data to parse
 *
 * @return  pointer to the parsed command structure
 */
static void *zclParseInDefaultRspCmd( zclParseCmd_t *pCmd )
{
  zclDefaultRspCmd_t *defaultRspCmd;
  uint8 *pBuf = pCmd->pData;

  defaultRspCmd = (zclDefaultRspCmd_t *)osal_mem_alloc( sizeof ( zclDefaultRspCmd_t ) );
  if ( defaultRspCmd )
  {
    defaultRspCmd->commandID = *pBuf++;
    defaultRspCmd->statusCode = *pBuf;
  }

  return ( (void *)defaultRspCmd );
}

#ifdef ZCL_DISCOVER
/*********************************************************************
 * @fn      zclParseInDiscCmd
 *
 * @brief   Parse the "Profile" Discovery Commands
 *
 *      NOTE: THIS FUNCTION ALLOCATES THE RETURN BUFFER, SO THE CALLING
 *            FUNCTION IS RESPONSIBLE TO FREE THE MEMORY.
 *
 * @param   endpoint - application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   dataLen - number of bytes in incoming data buffer
 * @param   pData - pointer to incoming data to parse
 *
 * @return  pointer to the parsed command structure
 */
void *zclParseInDiscCmd( zclParseCmd_t *pCmd )
{
  zclDiscoverCmd_t *discoverCmd;
  uint8 *pBuf = pCmd->pData;

  discoverCmd = (zclDiscoverCmd_t *)osal_mem_alloc( sizeof ( zclDiscoverCmd_t ) );
  if ( discoverCmd )
  {
    discoverCmd->startAttr = BUILD_UINT16( pBuf[0], pBuf[1] );
    pBuf += 2;
    discoverCmd->maxAttrIDs = *pBuf;
  }

  return ( (void *)discoverCmd );
}

/*********************************************************************
 * @fn      zclParseInDiscRspCmd
 *
 * @brief   Parse the "Profile" Discovery Response Commands
 *
 *      NOTE: THIS FUNCTION ALLOCATES THE RETURN BUFFER, SO THE CALLING
 *            FUNCTION IS RESPONSIBLE TO FREE THE MEMORY.
 *
 * @param   endpoint - application's endpoint
 * @param   realClusterID - real cluster ID
 * @param   dataLen - number of bytes in incoming data buffer
 * @param   pData - pointer to incoming data to parse
 *
 * @return  pointer to the parsed command structure
 */
#define ZCLDISCRSPCMD_DATALEN(a)  ((a)-1) // data len - Discovery Complete
static void *zclParseInDiscRspCmd( zclParseCmd_t *pCmd )
{
  zclDiscoverRspCmd_t *discoverRspCmd;
  uint8 *pBuf = pCmd->pData;
  uint8 i;

  discoverRspCmd = (zclDiscoverRspCmd_t *)osal_mem_alloc( sizeof ( zclDiscoverRspCmd_t ) \
                                                 + ZCLDISCRSPCMD_DATALEN(pCmd->dataLen) );
  if ( discoverRspCmd )
  {
    discoverRspCmd->discComplete = *pBuf++;
    discoverRspCmd->numAttr = ZCLDISCRSPCMD_DATALEN(pCmd->dataLen) / ( 2 + 1 ); // Attr ID + Data Type
    
    for ( i = 0; i < discoverRspCmd->numAttr; i++ )
    {
      discoverRspCmd->attrList[i].attrID = BUILD_UINT16( pBuf[0], pBuf[1] );
      pBuf += 2;
      discoverRspCmd->attrList[i].dataType = *pBuf++;;
    }
  }

  return ( (void *)discoverRspCmd );
}
#endif // ZCL_DISCOVER

#ifdef ZCL_READ
/*********************************************************************
 * @fn      zclProcessInReadCmd
 *
 * @brief   Process the "Profile" Read Command
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  TRUE if attribute was found in the Attribute list,
 *          FALSE if not
 */
static uint8 zclProcessInReadCmd( zclIncoming_t *pInMsg )
{
  zclReadCmd_t *readCmd;
  zclReadRspCmd_t *readRspCmd;
  zclReadRspStatus_t *statusRec;
  zclAttrRec_t attrRec;
  uint8 dataLen;
  uint8 len = sizeof( zclReadRspCmd_t );
  uint8 i;
  
  readCmd = (zclReadCmd_t *)pInMsg->attrCmd;
  
  // calculate the length of the response status record
  for (i = 0; i < readCmd->numAttr; i++)
  {
    len += sizeof (zclReadRspStatus_t);

    // Get Attribute Data length
    if ( zclFindAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, readCmd->attrID[i], &attrRec ) )
      len += zclGetAttrDataLength( attrRec.attr.dataType, (uint8*)(attrRec.attr.dataPtr) );
  } 
  
  readRspCmd = osal_mem_alloc( len );
  if ( readRspCmd == NULL )
    return FALSE; // EMBEDDED RETURN

  statusRec = readRspCmd->attrList;
  readRspCmd->numAttr = readCmd->numAttr;
  for (i = 0; i < readCmd->numAttr; i++)
  {
    statusRec->attrID = readCmd->attrID[i];
    
    if ( zclFindAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, readCmd->attrID[i], &attrRec ) )
    {
      statusRec->status = zclReadAttrData( statusRec->data, &attrRec );
      statusRec->dataType = attrRec.attr.dataType;
      dataLen = zclGetAttrDataLength( attrRec.attr.dataType, (uint8*)(attrRec.attr.dataPtr) );
    }
    else
    {
      statusRec->status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
      dataLen = 0;
    }
    statusRec = NEXT_READ_RSP(statusRec, dataLen);
  }
  
  // Build and send Read Response command
  zcl_SendReadRsp( pInMsg->msg->endPoint, &(pInMsg->msg->srcAddr), pInMsg->msg->clusterId,
                   readRspCmd, ZCL_FRAME_SERVER_CLIENT_DIR,
                   true, pInMsg->hdr.transSeqNum );
  osal_mem_free( readRspCmd );
    
  return TRUE;
}

/*********************************************************************
 * @fn      zclProcessInReadRspCmd
 *
 * @brief   Process the "Profile" Read Response Command
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  none
 */
static uint8 zclProcessInReadRspCmd( zclIncoming_t *pInMsg )
{
  zclReadRspCmd_t *readRspCmd;
  uint8 i;

  readRspCmd = (zclReadRspCmd_t *)pInMsg->attrCmd;
  for (i = 0; i < readRspCmd->numAttr; i++)
  {
    // Notify the originator of the results of the original read attributes 
    // attempt and, for each successfull request, the value of the requested 
    // attribute
  }

  return TRUE; 
}
#endif // ZCL_READ

#ifdef ZCL_WRITE
/*********************************************************************
 * @fn      processInWriteCmd
 *
 * @brief   Process the "Profile" Write and Write No Response Commands
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  TRUE if attribute was found in the Attribute list,
 *          FALSE if not
 */
static uint8 zclProcessInWriteCmd( zclIncoming_t *pInMsg )
{
  zclWriteCmd_t *writeCmd;
  zclWriteRec_t *statusRec;
  zclWriteRspCmd_t *writeRspCmd;
  zclAttrRec_t attrRec;
  uint8 sendRsp = FALSE;
  uint8 dataLen;
  uint8 status;
  uint8 i, j = 0;

  writeCmd = (zclWriteCmd_t *)pInMsg->attrCmd;
  if ( pInMsg->hdr.commandID == ZCL_CMD_WRITE )
  {
    // We need to send a response back - allocate space for it
    writeRspCmd = (zclWriteRspCmd_t *)osal_mem_alloc( sizeof( zclWriteRspCmd_t ) + \
                            sizeof( zclWriteRspStatus_t )* writeCmd->numAttr );
    if ( writeRspCmd == NULL )
      return FALSE; // EMBEDDED RETURN
    sendRsp = TRUE;
  }
  
  statusRec = writeCmd->attrList;
  for (i = 0; i < writeCmd->numAttr; i++)
  {
    if ( zclFindAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, statusRec->attrID, &attrRec ) )
    {
      if ( statusRec->dataType == attrRec.attr.dataType )
      {
        status = zclWriteAttrData( &attrRec, statusRec );

        // If successful, a write attribute status record shall NOT be generated
        if ( sendRsp && status != ZCL_STATUS_SUCCESS )
        {
          // Attribute is read only - move on to the next write attribute record
          writeRspCmd->attrList[j].status = status;
          writeRspCmd->attrList[j++].attrID = statusRec->attrID;
        }
      }
      else
      {
        // Attribute data type is incorrect - move on to the next write attribute record
        if ( sendRsp )
        {
          writeRspCmd->attrList[j].status = ZCL_STATUS_INVALID_DATA_TYPE;
          writeRspCmd->attrList[j++].attrID = statusRec->attrID;
        }
      }
    }
    else
    {
      // Attribute is not supported - move on to the next write attribute record
      if ( sendRsp )
      {
        writeRspCmd->attrList[j].status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
        writeRspCmd->attrList[j++].attrID = statusRec->attrID;
      }
    }
    
    dataLen = zclGetAttrDataLength( statusRec->dataType, statusRec->attrData );
    statusRec = NEXT_WRITE( statusRec, dataLen );
  } // for loop

  if ( sendRsp )
  {
    writeRspCmd->numAttr = j;
    if ( writeRspCmd->numAttr == 0 )
    {
      // Since all records were written successful, include a single status record
      // in the resonse command with the status field set to SUCCESS and the 
      // attribute ID field omitted.
      writeRspCmd->attrList[0].status = ZCL_STATUS_SUCCESS;
      writeRspCmd->numAttr = 1;
    }
    
    zcl_SendWriteRsp( pInMsg->msg->endPoint, &(pInMsg->msg->srcAddr),
                      pInMsg->msg->clusterId, writeRspCmd, ZCL_FRAME_SERVER_CLIENT_DIR, 
                      true, pInMsg->hdr.transSeqNum );
    osal_mem_free( writeRspCmd );
  }
  
  return TRUE; 
}

/*********************************************************************
 * @fn      zclRevertWriteUndividedCmd
 *
 * @brief   Revert the "Profile" Write Undevided Command
 *
 * @param   pInMsg - incoming message to process
 * @param   curWriteRec - old data
 * @param   numAttr - number of attributes to be reverted
 *
 * @return  none
 */
static void zclRevertWriteUndividedCmd( zclIncoming_t *pInMsg, 
                                    zclWriteRec_t *curWriteRec, uint16 numAttr )
{
  zclWriteRec_t *statusRec;
  zclAttrRec_t attrRec;
  uint8 dataLen;
  uint8 i;

  statusRec = curWriteRec;
  for (i = 0; i < numAttr; i++)
  {
    if ( !zclFindAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, statusRec->attrID, &attrRec ) )
      break; // should never happen
 
    // Just copy the old data back - no need to validate the data
    dataLen = zclGetAttrDataLength( attrRec.attr.dataType, statusRec->attrData );
    osal_memcpy( attrRec.attr.dataPtr, statusRec->attrData, dataLen );
    
    // Move on to the next item
    statusRec = NEXT_WRITE( statusRec, dataLen );
  } // for loop
}

/*********************************************************************
 * @fn      zclProcessInWriteUndividedCmd
 *
 * @brief   Process the "Profile" Write Undevided Command
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  TRUE if attribute was found in the Attribute list,
 *          FALSE if not
 */
static uint8 zclProcessInWriteUndividedCmd( zclIncoming_t *pInMsg )
{
  zclWriteCmd_t *writeCmd;
  zclWriteRec_t *statusRec;
  zclWriteRec_t *curWriteRec;
  zclWriteRec_t *curStatusRec;
  zclWriteRspCmd_t *writeRspCmd;
  zclAttrRec_t attrRec;
  uint8 dataLen;
  uint8 curLen;
  uint8 status;
  uint8 i, j = 0;

  writeCmd = (zclWriteCmd_t *)pInMsg->attrCmd;
  
  // Allocate space for Write Response Command
  writeRspCmd = (zclWriteRspCmd_t *)osal_mem_alloc( sizeof( zclWriteRspCmd_t ) + \
                                sizeof( zclWriteRspStatus_t )* writeCmd->numAttr );
  if ( writeRspCmd == NULL )
    return FALSE; // EMBEDDED RETURN
  
  // If any attribute cannot be written, no attribute values are changed. Hence,
  // make sure all the attributes are supported and writable
  statusRec = writeCmd->attrList;
  for (i = 0; i < writeCmd->numAttr; i++)
  {
    if ( !zclFindAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, statusRec->attrID, &attrRec ) )
    {
      // Attribute is not supported - stop here
      writeRspCmd->attrList[j].status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
      writeRspCmd->attrList[j++].attrID = statusRec->attrID;
      break;
    }
    
    if ( statusRec->dataType != attrRec.attr.dataType )
    {
      // Attribute data type is incorrect - stope here
      writeRspCmd->attrList[j].status = ZCL_STATUS_INVALID_DATA_TYPE;
      writeRspCmd->attrList[j++].attrID = statusRec->attrID;
      break;
    }
    
    if ( !zcl_AccessCtrlWrite( attrRec.attr.accessControl ) )
    {
      // Attribute is not writable - stop here
      writeRspCmd->attrList[j].status = ZCL_STATUS_READ_ONLY;
      writeRspCmd->attrList[j++].attrID = statusRec->attrID;
      break;
    }
    
    dataLen = zclGetAttrDataLength( statusRec->dataType, statusRec->attrData );
    curLen += sizeof ( zclWriteRec_t ) + dataLen;
    statusRec = NEXT_WRITE( statusRec, dataLen );
  } // for loop
  
  writeRspCmd->numAttr = j;
  if ( writeRspCmd->numAttr  == 0 ) // All attributes can be written
  {
    // Allocate space to keep a copy of the current data
    curWriteRec = ( zclWriteRec_t *) osal_mem_alloc( curLen ); 
    if ( curWriteRec == NULL )
    {
      osal_mem_free(writeRspCmd );
      return FALSE; // EMBEDDED RETURN
    }

    // Write the new data over
    statusRec = writeCmd->attrList;
    curStatusRec = curWriteRec;
    for (i = 0; i < writeCmd->numAttr; i++)
    {
      if ( !zclFindAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, statusRec->attrID, &attrRec ) )
        break; // should never happen

      // Keep a copy of the current data before before writing the new data over
      curStatusRec->attrID = statusRec->attrID;
      zclReadAttrData( curStatusRec->attrData, &attrRec );

      status = zclWriteAttrData( &attrRec, statusRec );
         
      // If successful, a write attribute status record shall NOT be generated
      if ( status != ZCL_STATUS_SUCCESS )
      {
        writeRspCmd->attrList[j].status = status;
        writeRspCmd->attrList[j++].attrID = statusRec->attrID;
          
        // Since this write failed, we need to revert all the pervious writes
        zclRevertWriteUndividedCmd( pInMsg, curWriteRec, i);
        break;
      }
      
      dataLen = zclGetAttrDataLength( statusRec->dataType, statusRec->attrData );
      statusRec = NEXT_WRITE( statusRec, dataLen );
      curStatusRec = NEXT_WRITE( curStatusRec, dataLen );
    } // for loop
  
    writeRspCmd->numAttr = j;
    if ( writeRspCmd->numAttr  == 0 )
    {
      // Since all records were written successful, include a single status record
      // in the resonse command with the status field set to SUCCESS and the 
      // attribute ID field omitted.
      writeRspCmd->attrList[0].status = ZCL_STATUS_SUCCESS;
      writeRspCmd->numAttr = 1;
    }

    osal_mem_free( curWriteRec );
  }
  
  zcl_SendWriteRsp( pInMsg->msg->endPoint, &(pInMsg->msg->srcAddr),
                    pInMsg->msg->clusterId, writeRspCmd, ZCL_FRAME_SERVER_CLIENT_DIR, 
                    true, pInMsg->hdr.transSeqNum );  
  osal_mem_free( writeRspCmd );
 
  return TRUE; 
}
 
/*********************************************************************
 * @fn      zclProcessInWriteRspCmd
 *
 * @brief   Process the "Profile" Write Response Command
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  none
 */
static uint8 zclProcessInWriteRspCmd( zclIncoming_t *pInMsg )
{
  zclWriteRspCmd_t *writeRspCmd;
  uint8 i;

  writeRspCmd = (zclWriteRspCmd_t *)pInMsg->attrCmd;
  for (i = 0; i < writeRspCmd->numAttr; i++)
  {
    // Notify the device of the results of the its original write attributes
    // command.
  }

  return TRUE; 
}
#endif // ZCL_WRITE

#ifdef ZCL_REPORT
/*********************************************************************
 * @fn      zclProcessInConfigReportCmd
 *
 * @brief   Process the "Profile" Configure Reporting Command
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  TRUE if attribute was found in the Attribute list,
 *          FALSE if not
 */
static uint8 zclProcessInConfigReportCmd( zclIncoming_t *pInMsg )
{
  zclCfgReportCmd_t *cfgReportCmd;
  zclCfgReportRec_t *reportRec;
  zclCfgReportRspCmd_t *cfgReportRspCmd;
  zclAttrRec_t attrRec;
  uint8 reportChangeLen; // length of Reportable Change field
  uint8 status;
  uint8 i, j = 0;
  
  cfgReportCmd = (zclCfgReportCmd_t *)pInMsg->attrCmd;
  
  // Allocate space for the response command
  cfgReportRspCmd = (zclCfgReportRspCmd_t *)osal_mem_alloc( sizeof ( zclCfgReportRspCmd_t ) + \
                                        sizeof ( zclCfgReportStatus_t) * cfgReportCmd->numAttr );
  if ( cfgReportRspCmd == NULL )
    return FALSE; // EMBEDDED RETURN
  
  // Process each Attribute Reporting Configuration record
  reportRec = cfgReportCmd->attrList;
  for ( i = 0; i < cfgReportCmd->numAttr; i++ )
  {     
    reportChangeLen = 0;
    status = ZCL_STATUS_SUCCESS;
    
    if ( zclFindAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, reportRec->attrID, &attrRec ) )
    {
      if ( reportRec->direction == SEND_ATTR_REPORTS )
      {
        if ( reportRec->dataType == attrRec.attr.dataType )
        {
          // This the attribute that is to be reported
          if ( zcl_MandatoryReportableAttribute( &attrRec ) == TRUE )
          {
            if ( reportRec->minReportInt < ZCL_MIN_REPORTING_INTERVAL ||
                 ( reportRec->maxReportInt != 0 && 
                   reportRec->maxReportInt < reportRec->minReportInt ) )
            {
              // Invalid fields
              status = ZCL_STATUS_INVALID_VALUE;
            }
            else
            {
              // Set the Min and Max Reporting Intervals and Reportable Change
              //status = zclSetAttrReportInterval( pAttr, cfgReportCmd );
              status = ZCL_STATUS_UNREPORTABLE_ATTRIBUTE; // for now
            }
          }
          else
          {
            // Attribute cannot be reported
            status = ZCL_STATUS_UNREPORTABLE_ATTRIBUTE;
          }
        }
        else
        {
          // Attribute data type is incorrect
          status = ZCL_STATUS_INVALID_DATA_TYPE;
        }

        if ( zclAnalogDataType( reportRec->dataType ) )
          reportChangeLen = zclGetDataTypeLength( reportRec->dataType );
      }
      else
      {
        // We shall expect reports of values of this attribute
        if ( zcl_MandatoryReportableAttribute( &attrRec ) == TRUE )
        {    
          // Set the Timeout Period
          //status = zclSetAttrTimeoutPeriod( pAttr, cfgReportCmd );
          status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE; // for now
        }
        else
        {
          // Reports of attribute cannot be received
          status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
        }
      }   
    }
    else
    {
      // Attribute is not supported
      status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
    }
    
    // If not successful then record the status
    if ( status != ZCL_STATUS_SUCCESS )
    {
      cfgReportRspCmd->attrList[j].status = status;
      cfgReportRspCmd->attrList[j++].attrID = reportRec->attrID;
    }
    
    reportRec = NEXT_CFG_REPORT( reportRec, reportChangeLen );
  } // for loop
  
  cfgReportRspCmd->numAttr = j;
  if ( cfgReportRspCmd->numAttr == 0 )
  {
    // Since all attributes were configured successfully, include a single 
    // attribute status record in the response command with the status field
    // set to SUCCESS and the attribute ID field omitted.
    cfgReportRspCmd->attrList[0].status = ZCL_STATUS_SUCCESS;
    cfgReportRspCmd->numAttr = 1;
  }

  // Send the response back
  zcl_SendConfigReportRspCmd( pInMsg->msg->endPoint, &(pInMsg->msg->srcAddr), 
                              pInMsg->msg->clusterId, cfgReportRspCmd, ZCL_FRAME_SERVER_CLIENT_DIR, 
                              true, pInMsg->hdr.transSeqNum );
  osal_mem_free( cfgReportRspCmd );
  
  return TRUE ;
}

/*********************************************************************
 * @fn      zclProcessInConfigReportRspCmd
 *
 * @brief   Process the "Profile" Configure Reporting Response Command
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  none
 */
static uint8 zclProcessInConfigReportRspCmd( zclIncoming_t *pInMsg )
{
  zclCfgReportRspCmd_t *cfgReportRspCmd;
  zclAttrRec_t attrRec;
  uint8 i;

  cfgReportRspCmd = (zclCfgReportRspCmd_t *)pInMsg->attrCmd;
  for (i = 0; i < cfgReportRspCmd->numAttr; i++)
  {
    if ( zclFindAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, 
                         cfgReportRspCmd->attrList[i].attrID, &attrRec ) )
    {
      // Notify the device of success (or otherwise) of the its original configure
      // reporting command, for each attribute.
    }
  }

  return TRUE; 
}

/*********************************************************************
 * @fn      zclProcessInReadReportCfgCmd
 *
 * @brief   Process the "Profile" Read Reporting Configuration Command
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  none
 */
static uint8 zclProcessInReadReportCfgCmd( zclIncoming_t *pInMsg )
{
  zclReadReportCfgCmd_t *readReportCfgCmd;
  zclReadReportCfgRspCmd_t *readReportCfgRspCmd;
  zclReportCfgRspRec_t *reportRspRec;
  zclAttrRec_t attrRec;
  uint8 reportChangeLen;
  uint8 rspLen = sizeof ( zclReadReportCfgRspCmd_t );
  uint8 status;
  uint8 i;
  
  readReportCfgCmd = (zclReadReportCfgCmd_t *)pInMsg->attrCmd;

  // Find out the response length (Reportable Change field is of variable length)
  for ( i = 0; i < readReportCfgCmd->numAttr; i++ )
  {
    reportChangeLen = 0;
    rspLen += sizeof ( zclReportCfgRspRec_t );

    // For supported attributes with 'analog' data type, find out the length of 
    // the Reportable Change field
    if ( zclFindAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, 
                         readReportCfgCmd->attrList[i].attrID, &attrRec ) )
    {
      if ( zclAnalogDataType( attrRec.attr.dataType ) )
        rspLen += zclGetDataTypeLength( attrRec.attr.dataType );
    }
  }
  
  // Allocate space for the response command
  readReportCfgRspCmd = (zclReadReportCfgRspCmd_t *)osal_mem_alloc( rspLen );
  if ( readReportCfgRspCmd == NULL )
    return FALSE; // EMBEDDED RETURN
  
  readReportCfgRspCmd->numAttr = 0;
  reportRspRec = readReportCfgRspCmd->attrList; 
  for (i = 0; i < readReportCfgCmd->numAttr; i++)
  {
    readReportCfgRspCmd->numAttr++;
    reportChangeLen = 0;

    if ( zclFindAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, 
                         readReportCfgCmd->attrList[i].attrID, &attrRec ) )
    {
      if ( zcl_MandatoryReportableAttribute( &attrRec ) == TRUE )
      {
        // Get the Reporting Configuration
        // status = zclReadReportCfg( readReportCfgCmd->attrID[i], reportRspRec );
        status = ZCL_STATUS_UNREPORTABLE_ATTRIBUTE; // for now
        if ( status == ZCL_STATUS_SUCCESS && zclAnalogDataType( attrRec.attr.dataType ) )
          reportChangeLen = zclGetDataTypeLength( attrRec.attr.dataType );
      }
      else
      {
        // Attribute not in the Mandatory Reportable Attribute list
        status = ZCL_STATUS_UNREPORTABLE_ATTRIBUTE;
      }
    }
    else
    {
      // Attribute not found
      status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
    }
                  
    reportRspRec->status = status;
    reportRspRec->attrID = readReportCfgCmd->attrList[i].attrID;
    reportRspRec = NEXT_REPORT_RSP( reportRspRec, reportChangeLen );
  }

  // Send the response back
  zcl_SendReadReportCfgRspCmd( pInMsg->msg->endPoint, &pInMsg->msg->srcAddr, 
                               pInMsg->msg->clusterId, readReportCfgRspCmd, ZCL_FRAME_SERVER_CLIENT_DIR, 
                               true, pInMsg->hdr.transSeqNum );
  osal_mem_free( readReportCfgRspCmd );
  
  return TRUE;
}

/*********************************************************************
 * @fn      zclProcessInReadReportCfgRspCmd
 *
 * @brief   Process the "Profile" Read Reporting Configuration Response Command
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  none
 */
static uint8 zclProcessInReadReportCfgRspCmd( zclIncoming_t *pInMsg )
{
  zclReadReportCfgRspCmd_t *readReportCfgRspCmd;
  zclReportCfgRspRec_t *reportRspRec;
  uint8 reportChangeLen;
  uint8 i;

  readReportCfgRspCmd = (zclReadReportCfgRspCmd_t *)pInMsg->attrCmd;
  reportRspRec = readReportCfgRspCmd->attrList;
  for ( i = 0; i < readReportCfgRspCmd->numAttr; i++ )
  {
    reportChangeLen = 0;
    
    // Notify the device of the results of the its original read reporting
    // configuration command.
    
    if ( reportRspRec->status == ZCL_STATUS_SUCCESS )
    {
      if ( reportRspRec->direction == SEND_ATTR_REPORTS )
      {
        if ( zclAnalogDataType( reportRspRec->dataType ) )
          reportChangeLen = zclGetDataTypeLength( reportRspRec->dataType );
      }
    }
    
    reportRspRec = NEXT_REPORT_RSP( reportRspRec, reportChangeLen );
  }

  return TRUE;   
}

/*********************************************************************
 * @fn      zclProcessInReportCmd
 *
 * @brief   Process the "Profile" Report Command
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  none
 */
static uint8 zclProcessInReportCmd( zclIncoming_t *pInMsg )
{
  zclReportCmd_t *reportCmd;
  zclReport_t *reportRec;
  uint8 dataLen;
  uint8 i;

  reportCmd = (zclReportCmd_t *)pInMsg->attrCmd;
  reportRec = reportCmd->attrList;
  for (i = 0; i < reportCmd->numAttr; i++)
  {      
    // Device is notified of the latest values of the attribute of another device.
      
    dataLen = zclGetAttrDataLength( reportRec->dataType, reportRec->attrData );
    
    reportRec = NEXT_REPORT( reportRec, dataLen );
  }

  return TRUE;   
}
#endif // ZCL_REPORT

/*********************************************************************
 * @fn      zclProcessInDefaultRspCmd
 *
 * @brief   Process the "Profile" Default Response Command
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  none
 */
static uint8 zclProcessInDefaultRspCmd( zclIncoming_t *pInMsg )
{
  // zclDefaultRspCmd_t *defaultRspCmd = (zclDefaultRspCmd_t *)pInMsg->attrCmd;
   
  // Device is notified of the Default Response command.

  return TRUE; 
}

#ifdef ZCL_DISCOVER
/*********************************************************************
 * @fn      zclProcessInDiscCmd
 *
 * @brief   Process the "Profile" Discover Command
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  none
 */
static uint8 zclProcessInDiscCmd( zclIncoming_t *pInMsg )
{
  zclDiscoverCmd_t *discoverCmd;
  zclDiscoverRspCmd_t *discoverRspCmd;
  uint8 discComplete = TRUE;
  //zclAttrRec_t *pAttr;
  zclAttrRec_t attrRec;
  uint16 attrID;
  uint8 i;
  
  discoverCmd = (zclDiscoverCmd_t *)pInMsg->attrCmd;
  
  // Find out the number of attributes supported within the specified range
  for ( i = 0, attrID = discoverCmd->startAttr; i < discoverCmd->maxAttrIDs; i++, attrID++ )
  {
    if ( !zclFindNextAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, &attrID, &attrRec ) )
      break;
  }
  
  // Allocate space for the response command
  discoverRspCmd = (zclDiscoverRspCmd_t *)osal_mem_alloc( sizeof (zclDiscoverRspCmd_t) + \
                                                          sizeof ( zclDiscoverInfo_t ) * i );
  if ( discoverRspCmd == NULL )
    return FALSE; // EMEDDED RETURN
  
  discoverRspCmd->numAttr = i;
  if ( discoverRspCmd->numAttr != 0 )
  {
    for ( i = 0, attrID = discoverCmd->startAttr; i < discoverRspCmd->numAttr; i++, attrID++ )
    {
      if ( !zclFindNextAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, &attrID, &attrRec ) )
        break; // Attribute not supported
      
      discoverRspCmd->attrList[i].attrID = attrRec.attr.attrId;
      discoverRspCmd->attrList[i].dataType = attrRec.attr.dataType;
    }
    
    // Are there more attributes to be discovered?
    if ( zclFindNextAttrRec( pInMsg->msg->endPoint, pInMsg->msg->clusterId, &attrID, &attrRec ) )
      discComplete = FALSE;
  }
  
  discoverRspCmd->discComplete = discComplete;
  zcl_SendDiscoverRspCmd( pInMsg->msg->endPoint, &pInMsg->msg->srcAddr, 
                          pInMsg->msg->clusterId, discoverRspCmd, ZCL_FRAME_SERVER_CLIENT_DIR, 
                          true, pInMsg->hdr.transSeqNum );
  osal_mem_free( discoverRspCmd );
  
  return TRUE;
}

/*********************************************************************
 * @fn      zclProcessInDiscRspCmd
 *
 * @brief   Process the "Profile" Discover Response Command
 *
 * @param   pInMsg - incoming message to process
 * @param   logicalClusterID - logical cluster ID
 *
 * @return  none
 */
static uint8 zclProcessInDiscRspCmd( zclIncoming_t *pInMsg )
{
  zclDiscoverRspCmd_t *discoverRspCmd;
  uint8 i;
  
  discoverRspCmd = (zclDiscoverRspCmd_t *)pInMsg->attrCmd;
  for ( i = 0; i < discoverRspCmd->numAttr; i++ )
  {
    // Device is notified of the result of its attribute discovery command.
  }
  
  return TRUE;
}
#endif // ZCL_DISCOVER

/*********************************************************************
*********************************************************************/
