/**************************************************************************************************
  Filename:       MT_SAPI.c
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

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "MTEL.h"
#include "MT_SAPI.h"
#include "OnBoard.h"
#include "SPIMgr.h"
#include "OSAL_Nv.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

#if defined ( MT_SAPI_CB_FUNC )
uint16 _sapiCallbackSub;
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
/*********************************************************************
 * @fn      MT_sapiCommandProcessing
 *
 * @brief
 *
 *   Process all the SAPI commands that are issued by test tool
 *
 * @param   cmd_id - Command ID
 * @param   len    - Length of received SPI data message
 * @param   data   - pointer to received SPI data message
 *
 * @return  none
 */
uint8 MT_sapiCommandProcessing( uint16 cmd_id , byte len , byte *pData )
{
  uint8 *pBuf;
  uint8 i;
  uint8 ret;

  ret = 0x00;   // Default return value ( ZSuccess )
  switch (cmd_id)
  {
    case SPI_CMD_SAPI_SYS_RESET:
      zb_SystemReset();
      ret = 0xff;       // disable spi return message
      break;

    case SPI_CMD_SAPI_START_REQ:
      zb_StartRequest();
      break;

    case SPI_CMD_SAPI_BIND_DEVICE:
      MT_ReverseBytes( &pData[3], Z_EXTADDR_LEN );
      for ( i = 0; i < Z_EXTADDR_LEN ; i++ )
      {
        if ( pData[3+i] != 0x00 )
        {
          // Not a null destination address
          break;
        }
      }
      if ( i == Z_EXTADDR_LEN )
      {
        zb_BindDevice(pData[0], BUILD_UINT16(pData[2], pData[1]), (uint8 *)NULL);
      }
      else
      {
        zb_BindDevice(pData[0], BUILD_UINT16(pData[2], pData[1]), &pData[3]);
      }
      break;

    case SPI_CMD_SAPI_ALLOW_BIND:
      zb_AllowBind( pData[0]);
      break;

    case SPI_CMD_SAPI_SEND_DATA:
      zb_SendDataRequest(BUILD_UINT16(pData[1], pData[0]), BUILD_UINT16(pData[3], pData[2]),
                        pData[7], &pData[8], pData[4], pData[5], pData[6]);
      break;

    case SPI_CMD_SAPI_READ_CFG:
      i = (uint8)osal_nv_item_len( pData[0] );
      pBuf = osal_mem_alloc(i+3);
      if (pBuf)
      {
        pBuf[1] = pData[0]; // configId
        pBuf[2] = i;        // len
        if ( i )
          ret = zb_ReadConfiguration(pData[0], i, pBuf+3);
        else
          ret = ZInvalidParameter;
        pBuf[0] = ret;      // status
        MT_BuildAndSendZToolCB( SPI_CB_SAPI_READ_CFG_RSP, i+3, pBuf );
        osal_mem_free(pBuf);
      }
      ret = 0xff;   // Disable return spi message
      break;

    case SPI_CMD_SAPI_WRITE_CFG:
      ret = zb_WriteConfiguration(pData[0], pData[1], &pData[2]);
      break;

    case SPI_CMD_SAPI_GET_DEV_INFO:
      pBuf = osal_mem_alloc(Z_EXTADDR_LEN+1);
      if (pBuf)
      {
        zb_GetDeviceInfo(pData[0], pBuf+1);
        pBuf[0] = pData[0];
        MT_BuildAndSendZToolCB( SPI_CB_SAPI_DEV_INFO_RSP, Z_EXTADDR_LEN+1, pBuf );
        osal_mem_free(pBuf);
      }
      ret = 0xff;   // Disable return spi message
      break;

    case SPI_CMD_SAPI_FIND_DEV:
      // Currently only supports IEEE search
      MT_ReverseBytes( pData, Z_EXTADDR_LEN );
      zb_FindDeviceRequest(ZB_IEEE_SEARCH, pData);
      break;

    case SPI_CMD_SAPI_PMT_JOIN:
      ret = zb_PermitJoiningRequest( BUILD_UINT16( pData[1], pData[0] ), pData[2] );
      break;

    default:
      break;
  }

  return ret;
}
#endif  // #if defined ( MT_SAPI_FUNC )

#if defined ( MT_SAPI_CB_FUNC )

/******************************************************************************
 * @fn          zb_MTCallbackStartConfirm
 *
 * @brief       Process the callback subscription for zb_StartConfirm
 *
 * @param
 *
 * @return      none
 */
void zb_MTCallbackStartConfirm( uint8 status )
{
  uint8 memPtr[SPI_CB_SAPI_START_CNF_LEN];

  memPtr[0] = status;

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
  MT_BuildAndSendZToolCB( SPI_CB_SAPI_START_CNF, SPI_CB_SAPI_START_CNF_LEN, memPtr );
#endif
}

/******************************************************************************
 * @fn          zb_MTCallbackSendDataConfirm
 *
 * @brief       Process the callback subscription for zb_SendDataConfirm
 *
 * @param
 *
 * @return      none
 */
void zb_MTCallbackSendDataConfirm( uint8 handle, uint8 status )
{
  uint8 memPtr[SPI_CB_SAPI_SEND_DATA_CNF_LEN];

  memPtr[0] = status;
  memPtr[1] = handle;

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
  MT_BuildAndSendZToolCB( SPI_CB_SAPI_SEND_DATA_CNF, SPI_CB_SAPI_SEND_DATA_CNF_LEN, memPtr );
#endif
}

/******************************************************************************
 * @fn          zb_MTCallbackBindConfirm
 *
 * @brief       Process the callback subscription for zb_BindConfirm
 *
 * @param
 *
 * @return      none
 */
void zb_MTCallbackBindConfirm( uint16 commandId, uint8 status )
{
  uint8 memPtr[SPI_CB_SAPI_BIND_CNF_LEN];

  memPtr[0] = HI_UINT16(commandId);
  memPtr[1] = LO_UINT16(commandId);
  memPtr[2] = status;

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
  MT_BuildAndSendZToolCB( SPI_CB_SAPI_BIND_CNF, SPI_CB_SAPI_BIND_CNF_LEN, memPtr );
#endif
}
/******************************************************************************
 * @fn          zb_MTCallbackAllowBindConfirm
 *
 * @brief       Indicates when another device attempted to bind to this device
 *
 * @param
 *
 * @return      none
 */
void zb_MTCallbackAllowBindConfirm( uint16 source )
{
  uint8 memPtr[SPI_CB_SAPI_ALLOW_BIND_CNF_LEN];

  memPtr[0] = HI_UINT16(source);
  memPtr[1] = LO_UINT16(source);

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
  MT_BuildAndSendZToolCB( SPI_CB_SAPI_ALLOW_BIND_CNF, SPI_CB_SAPI_ALLOW_BIND_CNF_LEN, memPtr );
#endif

}
/******************************************************************************
 * @fn          zb_MTCallbackFindDeviceConfirm
 *
 * @brief       Process the callback subscription for zb_FindDeviceConfirm
 *
 * @param
 *
 * @return      none
 */
void zb_MTCallbackFindDeviceConfirm( uint8 searchType, uint8 *searchKey, uint8 *result )
{
  uint8 memPtr[SPI_CB_SAPI_FIND_DEV_CNF_LEN];
  uint16 addr = *((uint16*)searchKey);

  // Currently only supports IEEE Addr Search
  memPtr[0] = ZB_IEEE_SEARCH;
  memPtr[1] = HI_UINT16(addr);
  memPtr[2] = LO_UINT16(addr);
  osal_memcpy(&memPtr[3], result, Z_EXTADDR_LEN);
  MT_ReverseBytes( &memPtr[3], Z_EXTADDR_LEN );

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
  MT_BuildAndSendZToolCB( SPI_CB_SAPI_FIND_DEV_CNF, SPI_CB_SAPI_FIND_DEV_CNF_LEN, memPtr );
#endif
}

/******************************************************************************
 * @fn          zb_MTCallbackReceiveDataIndication
 *
 * @brief       Process the callback subscription for zb_ReceiveDataIndication
 *
 * @param
 *
 * @return      none
 */
void zb_MTCallbackReceiveDataIndication( uint16 source, uint16 command, uint16 len, uint8 *pData  )
{
  uint8 *memPtr;
  int8 i;
  uint8 msgLen = SPI_CB_SAPI_RCV_DATA_IND_LEN + len;

  memPtr = osal_mem_alloc(msgLen);

  if (memPtr)
  {
    memPtr[0] = HI_UINT16(source);
    memPtr[1] = LO_UINT16(source);
    memPtr[2] = HI_UINT16(command);
    memPtr[3] = LO_UINT16(command);
    memPtr[4] = HI_UINT16(len);
    memPtr[5] = LO_UINT16(len);

    for (i=0; i<len; i++)
    {
      memPtr[6+i] = pData[i];
    }

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
    MT_BuildAndSendZToolCB( SPI_CB_SAPI_RCV_DATA_IND, msgLen, memPtr );
#endif
    osal_mem_free( memPtr );
  }
}

#endif  // #if defined ( MT_SAPI_CB_FUNC )

/*********************************************************************
*********************************************************************/
