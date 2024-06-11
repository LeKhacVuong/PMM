/****************************************************************************
 * @file     mcan.c
 * @version  V1.00
 * @brief    M253 series MCAN driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include "NuMicro.h"
#include "string.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup MCAN_Driver MCAN Driver
  @{
*/

/** @addtogroup MCAN_EXPORTED_FUNCTIONS MCAN Exported Functions
  @{
*/


/* Next free location in RAM for CAN data storage. */
static uint16_t g_u16RamOffset;

/*! @brief Keeps track of the size of each receive FIFO. */
static uint8_t g_au8RxFIFOSize[MCAN_NUM_RX_FIFOS];
/**
 * @brief       MCAN IP Reset  .
 *
 * @return      None.
 *
 * @details     This function is reset the MCAN IP.
 *
 */
void MCAN_IPReset(void)
{
     SYS->IPRST1 = SYS_IPRST1_MCAN0RST_Msk;   //Access IPRST1 to reset UART1 IP
     SYS->IPRST1 &= ~SYS_IPRST1_MCAN0RST_Msk; //Return Normal Operation
}

/**
 * @brief       Converts number of bytes.
 *
 * @param[in]   u8NumberOfBytes  Number of bytes in a message.
 *
 * @return      FIFO/buffer size encoding.
 *
 * @details     Converts number of bytes in a message into a FIFO/buffer size encoding.
 */
static uint8_t MCAN_NumberofBytestoFieldSize(uint8_t u8NumberOfBytes)
{
    if (u8NumberOfBytes <= 8) return 0;
    else if (u8NumberOfBytes <= 12) return 1;
    else if (u8NumberOfBytes <= 16) return 2;
    else if (u8NumberOfBytes <= 20) return 3;
    else if (u8NumberOfBytes <= 24) return 4;
    else if (u8NumberOfBytes <= 32) return 5;
    else if (u8NumberOfBytes <= 48) return 6;
    else return 7;
}


/**
 * @brief       Encode the Data Length Code.
 *
 * @param[in]   u8NumberOfBytes  Number of bytes in a message.
 *
 * @return      Data Length Code.
 *
 * @details     Converts number of bytes in a message into a Data Length Code.
 */

static uint8_t MCAN_EncodeDLC(uint8_t u8NumberOfBytes)
{
    if (u8NumberOfBytes <= 8) return u8NumberOfBytes;
    else if (u8NumberOfBytes <= 12) return 9;
    else if (u8NumberOfBytes <= 16) return 10;
    else if (u8NumberOfBytes <= 20) return 11;
    else if (u8NumberOfBytes <= 24) return 12;
    else if (u8NumberOfBytes <= 32) return 13;
    else if (u8NumberOfBytes <= 48) return 14;
    else return 15;
}

/**
 * @brief       Decode the Data Length Code.
 *
 * @param[in]   u8Dlc   dlc Data Length Code.
 *
 * @return      Number of bytes in a message.
 *
 * @details     Converts a Data Length Code into a number of message bytes.
 */
static uint8_t MCAN_DecodeDLC(uint8_t u8Dlc)
{
    if (u8Dlc <= 8) return u8Dlc;
    else if (u8Dlc == 9) return 12;
    else if (u8Dlc == 10) return 16;
    else if (u8Dlc == 11) return 20;
    else if (u8Dlc == 12) return 24;
    else if (u8Dlc == 13) return 32;
    else if (u8Dlc == 14) return 48;
    else return 64;
}

/**
 * @brief       Get the element's address when read receive fifo 0.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 *
 * @return      Address of the element in receive fifo 0.
 *
 * @details     The function is used to get the element's address when read receive fifo 0.
 */

uint32_t MCAN_GetRxFifo0ElementAddress(MCAN_T *mcan)
{
    uint32_t u32Size;
    uint32_t u32Address;
    u32Size = (mcan->RXESC & MCAN_RXESC_F0DS_Msk) >> MCAN_RXESC_F0DS_Pos;
    if (u32Size < 5U)
    {
        u32Size += 4U;
    }
    else
    {
        u32Size = u32Size * 4U - 10U;
    }
    u32Address = mcan->RXF0C & MCAN_RXF0C_F0SA_Msk;
    u32Address += ((mcan->RXF0S & MCAN_RXF0S_F0GI_Msk) >> MCAN_RXF0S_F0GI_Pos) * u32Size * 4U;
    return u32Address;
}

/**
 * @brief       Get the element's address when read receive fifo 1.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 *
 * @return      Address of the element in receive fifo 1.
 *
 * @details     The function is used to get the element's address when read receive fifo 1.
 */

uint32_t MCAN_GetRxFifo1ElementAddress(MCAN_T *mcan)
{
    uint32_t u32Size;
    uint32_t u32Address;
    u32Size = (mcan->RXESC & MCAN_RXESC_F1DS_Msk) >> MCAN_RXESC_F1DS_Pos;
    if (u32Size < 5U)
    {
        u32Size += 4U;
    }
    else
    {
        u32Size = u32Size * 4U - 10U;
    }
    u32Address = mcan->RXF1C & MCAN_RXF1C_F1SA_Msk;
    u32Address += ((mcan->RXF1S & MCAN_RXF1S_F1GI_Msk) >> MCAN_RXF1S_F1GI_Pos) * u32Size * 4U;
    return u32Address;
}

/**
 * @brief       Get the element's address when read receive buffer.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8Idx       The number of the rceive buffer element
 *
 * @return      Address of the element in receive buffer.
 *
 * @details     The function is used to get the element's address when read receive buffer.
 */

uint32_t MCAN_GetRxBufferElementAddress(MCAN_T *mcan, uint8_t u8Idx)
{
    uint32_t u32Size;
    u32Size = (mcan->RXESC & MCAN_RXESC_RBDS_Msk) >> MCAN_RXESC_RBDS_Pos;
    if (u32Size < 5U)
    {
        u32Size += 4U;
    }
    else
    {
        u32Size = u32Size * 4U - 10U;
    }
    return (mcan->RXBC & MCAN_RXBC_RBSA_Msk) + u8Idx * u32Size * 4U;
}


/**
 * @brief       Get the element's address when read transmit buffer.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8Idx       The number of the transmit buffer element
 *
 * @return      Address of the element in transmit buffer.
 *
 * @details     The function is used to get the element's address when read transmit buffer.
 */

uint32_t MCAN_GetTxBufferElementAddress(MCAN_T *mcan, uint8_t u8Idx)
{
    uint32_t u32Size;
    u32Size = (mcan->TXESC & MCAN_TXESC_TBDS_Msk) >> MCAN_TXESC_TBDS_Pos;
    if (u32Size < 5U)
    {
        u32Size += 4U;
    }
    else
    {
        u32Size = u32Size * 4U - 10U;
    }
    return (mcan->TXBC & MCAN_TXBC_TBSA_Msk) + u8Idx * u32Size * 4U;
}

/**
 * @brief       Set filter configuration.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   config      The MCAN filter configuratio.
 *
 * @return      None.
 *
 * @details     This function sets remote and non masking frames in global filter configuration,
 *              also the start address, list size in standard/extended ID filter configuration.
 */

void MCAN_SetFilterConfig(MCAN_T *mcan, const S_MCAN_FRAME_FILTER_CONFIG_T *config)
{
    /* Set global configuration of remote/nonmasking frames, set filter address and list size. */
    if (config->eIdFormat == eMCAN_FrameFormatStandard)
    {
        mcan->GFC = MCAN_GFC_RRFS(config->eIdFormat) | MCAN_GFC_ANFS(config->eNmFrame);
        mcan->SIDFC |= MCAN_SIDFC_FLSSA(config->u32Address >> MCAN_SIDFC_FLSSA_Pos) | MCAN_SIDFC_LSS(config->u32ListSize);
    }
    else
    {
        mcan->GFC |= MCAN_GFC_RRFE(config->eRemFrame) | MCAN_GFC_ANFE(config->eNmFrame);
        mcan->XIDFC |= MCAN_XIDFC_FLESA(config->u32Address >> MCAN_XIDFC_FLESA_Pos) | MCAN_XIDFC_LSE(config->u32ListSize);
    }
}

/**
 * @brief       Configures the MCAN Rx FIFO.
 *
 * @param[in]   mcan                   The pointer of the specified MCAN module.
 * @param[in]   u8RxFifoElementSize    The MCAN Rx FIFO Element configuration size.
 * @param[in]   u8Enable               Enable/Disable Rx FIFO
 *                              - \ref TRUE   : Enable Rx FIFO
 *                              - \ref FALSE  : Disable Rx FIFO
 * @return      None.
 *
 * @details     This function sets remote and non masking frames in global filter configuration,
 *              also the start address, list size in standard/extended ID filter configuration.
 */


void MCAN_SetRxFifoConfig(MCAN_T *mcan, uint8_t fifoIdx,uint8_t u8RxFifoElementSize, uint8_t u8Enable)
{
    if (fifoIdx >= MCAN_NUM_RX_FIFOS) return;

    /* remember FIFO size */
    g_au8RxFIFOSize[fifoIdx] = u8RxFifoElementSize & 0x7F;
    if (g_au8RxFIFOSize[fifoIdx] > MCAN_RX_BUFFER_ELM_NUM) g_au8RxFIFOSize[fifoIdx] = MCAN_RX_BUFFER_ELM_NUM;

    switch (fifoIdx)
    {
        case 0:
            if (u8Enable)
            {
                /* set size of Rx FIFO, set offset, blocking mode */
                mcan->RXF0C = (g_au8RxFIFOSize[fifoIdx] << 16) | (g_u16RamOffset & 0xFFFF);
                mcan->RXESC &= ~MCAN_RXESC_F0DS_Msk;
                mcan->RXESC |= MCAN_NumberofBytestoFieldSize(MCAN_MAX_MESSAGE_BYTES);
                g_u16RamOffset += g_au8RxFIFOSize[fifoIdx] * sizeof(S_MCAN_BUF_T);
            }
            else
            {
                mcan->RXF0C = 0;
            }
            break;

        case 1:
            if (u8Enable)
            {
                /* set size of Rx FIFO 1, set offset, blocking mode */
                mcan->RXF1C = (g_au8RxFIFOSize[fifoIdx] << 16) | (g_u16RamOffset & 0xFFFF);
                mcan->RXESC &= ~MCAN_RXESC_F1DS_Msk;
                mcan->RXESC |= (MCAN_NumberofBytestoFieldSize(MCAN_MAX_MESSAGE_BYTES) << 4);
                g_u16RamOffset += g_au8RxFIFOSize[fifoIdx] * sizeof(S_MCAN_BUF_T);
            }
            else
            {
                mcan->RXF1C = 0;
            }
            break;
    }
}

/**
 * @brief       Configures an MCAN receive fifo 0 buffer.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   config      The receive fifo 0 configuration structure.
 *
 * @return      None.
 *
 * @details     This function sets start address, element size, watermark, operation mode
 *              and datafield size of the recieve fifo 0.
 */

void MCAN_SetRxFifo0Config(MCAN_T *mcan, const S_MCAN_RX_FIFO_CONFIG_T *psConfig)
{
    /* Set Rx FIFO 0 start address, element size, watermark, operation mode. */
    mcan->RXF0C |= MCAN_RXF0C_F0SA(psConfig->u32Address >> MCAN_RXF0C_F0SA_Pos) | MCAN_RXF0C_F0S(psConfig->u32ElementSize) |
                   MCAN_RXF0C_F0WM(psConfig->u32Watermark) | MCAN_RXF0C_F0OM(psConfig->eOpmode);
    /* Set Rx FIFO 0 data field size */
    mcan->RXESC |= MCAN_RXESC_F0DS(psConfig->eDatafieldSize);
 }

/**
 * @brief       Configures an MCAN receive fifo 1 buffer.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   config      The receive fifo 1 configuration structure.
 *
 * @return      None.
 *
 * @details     This function sets start address, element size, watermark, operation mode
 *              and datafield size of the recieve fifo 1.
 */
void MCAN_SetRxFifo1Config(MCAN_T *mcan, const S_MCAN_RX_FIFO_CONFIG_T *psConfig)
{
    /* Set Rx FIFO 1 start address, element size, watermark, operation mode. */
    mcan->RXF1C |= MCAN_RXF1C_F1SA(psConfig->u32Address >> MCAN_RXF1C_F1SA_Pos) | MCAN_RXF1C_F1S(psConfig->u32ElementSize) |
                   MCAN_RXF1C_F1WM(psConfig->u32Watermark) | MCAN_RXF1C_F1OM(psConfig->eOpmode);
    /* Set Rx FIFO 1 data field size */
    mcan->RXESC |= MCAN_RXESC_F1DS(psConfig->eDatafieldSize);
}

/**
 * @brief       Configures an MCAN receive buffer.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   config      The receive buffer configuration structure.
 *
 * @return      None.
 *
 * @details     This function sets start address and datafield size of the recieve buffer.
 */
void MCAN_SetRxBufferConfig(MCAN_T *mcan, const S_MCAN_RX_BUFFER_CONFIG_T *psConfig)
{
    /* Set Rx Buffer start address. */
    mcan->RXBC |= MCAN_RXBC_RBSA(psConfig->u32Address >> MCAN_RXBC_RBSA_Pos);
    /* Set Rx Buffer data field size */
    mcan->RXESC |= MCAN_RXESC_RBDS(psConfig->eDatafieldSize);
}

/**
 * @brief       Configures an MCAN transmit event fifo.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   config      The transmit event fifo configuration structure.
 *
 * @return      None.
 *
 * @details     This function sets start address, element size, watermark of the transmit event fifo.
 */
void MCAN_SetTxEventFifoConfig(MCAN_T *mcan, const S_MCAN_TX_FIFO_CONFIG_T *psConfig)
{
    /* Set TX Event FIFO start address, element size, watermark. */
    mcan->TXEFC |= MCAN_TXEFC_EFSA(psConfig->u32Address >> MCAN_TXEFC_EFSA_Pos) | MCAN_TXEFC_EFS(psConfig->u32ElementSize) |
                   MCAN_TXEFC_EFWM(psConfig->u32Watermark);
}


/**
 * @brief       Configures an MCAN transmit buffer.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   config      The transmit buffer configuration structure.
 *
 * @return      None.
 *
 * @details     This function sets start address, element size, fifo/queue mode and datafield
 *              size of the transmit buffer.
 */
void MCAN_SetTxBufferConfig(MCAN_T *mcan, const S_MCAN_TX_BUFFER_CONFIG_T *psConfig)
{

    /* Set Tx Buffer start address, size, fifo/queue mode. */
    mcan->TXBC |= MCAN_TXBC_TBSA(psConfig->u32Address >> MCAN_TXBC_TBSA_Pos) | MCAN_TXBC_NDTB(psConfig->u32DedicatedSize) |
                  MCAN_TXBC_TFQS(psConfig->u32FqSize) | MCAN_TXBC_TFQM(psConfig->eMode);
    /* Set Tx Buffer data field size */
    mcan->TXESC |= MCAN_TXESC_TBDS(psConfig->eDatafieldSize);
}

/**
 * @brief       Enables MCAN interrupts according to provided mask .
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8Line      Number of the Interrupt Line, 0 or 1.
 * @param[in]   u32IntSel   Interrupt type select
 *                              - \ref MCAN_IE_ARAE_Msk     : Access to Reserved Address Interrupt
 *                              - \ref MCAN_IE_PEDE_Msk     : Protocol Error in Data Phase Interrupt
 *                              - \ref MCAN_IE_PEAE_Msk     : Protocol Error in Arbitration Phase Interrupt
 *                              - \ref MCAN_IE_WDIE_Msk     : Watchdog Interrupt
 *                              - \ref MCAN_IE_BOE_Msk      : Bus_Off Status Interrupt
 *                              - \ref MCAN_IE_EWE_Msk      : Warning Status Interrupt
 *                              - \ref MCAN_IE_EPE_Msk      : Error Passive Interrupt
 *                              - \ref MCAN_IE_ELOE_Msk     : Error Logging Overflow Interrupt
 *                              - \ref MCAN_IE_BEUE_Msk     : Bit Error Uncorrected Interrupt
 *                              - \ref MCAN_IE_BECE_Msk     : Bit Error Corrected Interrupt
 *                              - \ref MCAN_IE_DRXE_Msk     : Message stored to Dedicated Rx Buffer Interrupt
 *                              - \ref MCAN_IE_TOOE_Msk     : Timeout Occurred Interrupt
 *                              - \ref MCAN_IE_MRAFE_Msk    : Message RAM Access Failure Interrupt
 *                              - \ref MCAN_IE_TSWE_Msk     : Timestamp Wraparound Interrupt
 *                              - \ref MCAN_IE_TEFLE_Msk    : Tx Event FIFO Event Lost Interrupt
 *                              - \ref MCAN_IE_TEFFE_Msk    : Tx Event FIFO Full Interrupt
 *                              - \ref MCAN_IE_TEFWE_Msk    : Tx Event FIFO Watermark Reached Interrupt
 *                              - \ref MCAN_IE_TEFNE_Msk    : Tx Event FIFO New Entry Interrupt
 *                              - \ref MCAN_IE_TFEE_Msk     : Tx FIFO Empty Interrupt
 *                              - \ref MCAN_IE_TCFE_Msk     : Transmission Cancellation Finished Interrupt
 *                              - \ref MCAN_IE_TCE_Msk      : Transmission Completed Interrupt
 *                              - \ref MCAN_IE_HPME_Msk     : High Priority Message Interrupt
 *                              - \ref MCAN_IE_RF1LE_Msk    : Rx FIFO 1 Message Lost Interrupt
 *                              - \ref MCAN_IE_RF1FE_Msk    : Rx FIFO 1 Full Interrupt 
 *                              - \ref MCAN_IE_RF1WE_Msk    : Rx FIFO 1 Watermark Reached Interrupt
 *                              - \ref MCAN_IE_RF1NE_Msk    : Rx FIFO 1 New Message Interrupt
 *                              - \ref MCAN_IE_RF0LE_Msk    : Rx FIFO 0 Message Lost Interrupt
 *                              - \ref MCAN_IE_RF0FE_Msk    : Rx FIFO 0 Full Interrupt 
 *                              - \ref MCAN_IE_RF0WE_Msk    : Rx FIFO 0 Watermark Reached Interrupt
 *                              - \ref MCAN_IE_RF0NE_Msk    : Rx FIFO 0 New Message Interrupt
 *
 * @return      None.
 *
 * @details     This macro enable specified MCAN interrupt.
 */

void MCAN_EnableInt(MCAN_T *mcan,uint8_t u8Line, uint32_t u32IntSel)
{
    /* Enable MCAN specified interrupt */
    mcan->ILE |= ((uint32_t)1U << u8Line);
    if (0U == u8Line)
    {
        mcan->ILS &= ~u32IntSel;
    }
    else
    {
        mcan->ILS |= u32IntSel;
    }
    mcan->IE |= u32IntSel;
}

/**
 * @brief       Disables MCAN interrupts according to provided mask .
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8Line      Number of the Interrupt Line, 0 or 1.
 * @param[in]   u32IntSel   Interrupt type select
 *                              - \ref MCAN_IE_ARAE_Msk     : Access to Reserved Address Interrupt
 *                              - \ref MCAN_IE_PEDE_Msk     : Protocol Error in Data Phase Interrupt
 *                              - \ref MCAN_IE_PEAE_Msk     : Protocol Error in Arbitration Phase Interrupt
 *                              - \ref MCAN_IE_WDIE_Msk     : Watchdog Interrupt
 *                              - \ref MCAN_IE_BOE_Msk      : Bus_Off Status Interrupt
 *                              - \ref MCAN_IE_EWE_Msk      : Warning Status Interrupt
 *                              - \ref MCAN_IE_EPE_Msk      : Error Passive Interrupt
 *                              - \ref MCAN_IE_ELOE_Msk     : Error Logging Overflow Interrupt
 *                              - \ref MCAN_IE_BEUE_Msk     : Bit Error Uncorrected Interrupt
 *                              - \ref MCAN_IE_BECE_Msk     : Bit Error Corrected Interrupt
 *                              - \ref MCAN_IE_DRXE_Msk     : Message stored to Dedicated Rx Buffer Interrupt
 *                              - \ref MCAN_IE_TOOE_Msk     : Timeout Occurred Interrupt
 *                              - \ref MCAN_IE_MRAFE_Msk    : Message RAM Access Failure Interrupt
 *                              - \ref MCAN_IE_TSWE_Msk     : Timestamp Wraparound Interrupt
 *                              - \ref MCAN_IE_TEFLE_Msk    : Tx Event FIFO Event Lost Interrupt
 *                              - \ref MCAN_IE_TEFFE_Msk    : Tx Event FIFO Full Interrupt
 *                              - \ref MCAN_IE_TEFWE_Msk    : Tx Event FIFO Watermark Reached Interrupt
 *                              - \ref MCAN_IE_TEFNE_Msk    : Tx Event FIFO New Entry Interrupt
 *                              - \ref MCAN_IE_TFEE_Msk     : Tx FIFO Empty Interrupt
 *                              - \ref MCAN_IE_TCFE_Msk     : Transmission Cancellation Finished Interrupt
 *                              - \ref MCAN_IE_TCE_Msk      : Transmission Completed Interrupt
 *                              - \ref MCAN_IE_HPME_Msk     : High Priority Message Interrupt
 *                              - \ref MCAN_IE_RF1LE_Msk    : Rx FIFO 1 Message Lost Interrupt
 *                              - \ref MCAN_IE_RF1FE_Msk    : Rx FIFO 1 Full Interrupt
 *                              - \ref MCAN_IE_RF1WE_Msk    : Rx FIFO 1 Watermark Reached Interrupt
 *                              - \ref MCAN_IE_RF1NE_Msk    : Rx FIFO 1 New Message Interrupt
 *                              - \ref MCAN_IE_RF0LE_Msk    : Rx FIFO 0 Message Lost Interrupt
 *                              - \ref MCAN_IE_RF0FE_Msk    : Rx FIFO 0 Full Interrupt 
 *                              - \ref MCAN_IE_RF0WE_Msk    : Rx FIFO 0 Watermark Reached Interrupt
 *                              - \ref MCAN_IE_RF0NE_Msk    : Rx FIFO 0 New Message Interrupt
 *
 * @return      None.
 *
 * @details     This macro disable specified MCAN interrupt.
 */
void MCAN_DisableInt(MCAN_T *mcan,uint8_t u8Line, uint32_t u32IntSel)
{
    /* Disable MCAN specified interrupt */
    if (0U == u8Line)
    {
        mcan->IE &= ~(u32IntSel);
    }else
    {
        mcan->ILS &= ~(u32IntSel);
    }

}

/**
 * @brief       Enables MCAN Message Tx Buffer Transmission interrupts .
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u32IntSel   Interrupt type select
 *                              - \ref MCAN_TXBTIE_TIE0      : MCAN_TXBTIE setting to set Transmission Interrupt 0 Enable
 *                              - \ref MCAN_TXBTIE_TIE1      : MCAN_TXBTIE setting to set Transmission Interrupt 1 Enable
 *                              - \ref MCAN_TXBTIE_TIE2      : MCAN_TXBTIE setting to set Transmission Interrupt 2 Enable
 *                              - \ref MCAN_TXBTIE_TIE3      : MCAN_TXBTIE setting to set Transmission Interrupt 3 Enable
 *                              - \ref MCAN_TXBTIE_TIE4      : MCAN_TXBTIE setting to set Transmission Interrupt 4 Enable
 *                              - \ref MCAN_TXBTIE_TIE5      : MCAN_TXBTIE setting to set Transmission Interrupt 5 Enable
 *                              - \ref MCAN_TXBTIE_TIE6      : MCAN_TXBTIE setting to set Transmission Interrupt 6 Enable
 *                              - \ref MCAN_TXBTIE_TIE7      : MCAN_TXBTIE setting to set Transmission Interrupt 7 Enable
 *                              - \ref MCAN_TXBTIE_TIE8      : MCAN_TXBTIE setting to set Transmission Interrupt 8 Enable
 *                              - \ref MCAN_TXBTIE_TIE9      : MCAN_TXBTIE setting to set Transmission Interrupt 9 Enable
 *                              - \ref MCAN_TXBTIE_TIE10     : MCAN_TXBTIE setting to set Transmission Interrupt 10 Enable
 *                              - \ref MCAN_TXBTIE_TIE11     : MCAN_TXBTIE setting to set Transmission Interrupt 11 Enable
 *                              - \ref MCAN_TXBTIE_TIE12     : MCAN_TXBTIE setting to set Transmission Interrupt 12 Enable
 *                              - \ref MCAN_TXBTIE_TIE13     : MCAN_TXBTIE setting to set Transmission Interrupt 13 Enable
 *                              - \ref MCAN_TXBTIE_TIE14     : MCAN_TXBTIE setting to set Transmission Interrupt 14 Enable
 *                              - \ref MCAN_TXBTIE_TIE15     : MCAN_TXBTIE setting to set Transmission Interrupt 15 Enable
 *                              - \ref MCAN_TXBTIE_TIE16     : MCAN_TXBTIE setting to set Transmission Interrupt 16 Enable
 *                              - \ref MCAN_TXBTIE_TIE17     : MCAN_TXBTIE setting to set Transmission Interrupt 17 Enable
 *                              - \ref MCAN_TXBTIE_TIE18     : MCAN_TXBTIE setting to set Transmission Interrupt 18 Enable
 *                              - \ref MCAN_TXBTIE_TIE19     : MCAN_TXBTIE setting to set Transmission Interrupt 19 Enable
 *                              - \ref MCAN_TXBTIE_TIE20     : MCAN_TXBTIE setting to set Transmission Interrupt 20 Enable
 *                              - \ref MCAN_TXBTIE_TIE21     : MCAN_TXBTIE setting to set Transmission Interrupt 21 Enable
 *                              - \ref MCAN_TXBTIE_TIE22     : MCAN_TXBTIE setting to set Transmission Interrupt 22 Enable
 *                              - \ref MCAN_TXBTIE_TIE23     : MCAN_TXBTIE setting to set Transmission Interrupt 23 Enable
 *                              - \ref MCAN_TXBTIE_TIE24     : MCAN_TXBTIE setting to set Transmission Interrupt 24 Enable
 *                              - \ref MCAN_TXBTIE_TIE25     : MCAN_TXBTIE setting to set Transmission Interrupt 25 Enable
 *                              - \ref MCAN_TXBTIE_TIE26     : MCAN_TXBTIE setting to set Transmission Interrupt 26 Enable
 *                              - \ref MCAN_TXBTIE_TIE27     : MCAN_TXBTIE setting to set Transmission Interrupt 27 Enable
 *                              - \ref MCAN_TXBTIE_TIE28     : MCAN_TXBTIE setting to set Transmission Interrupt 28 Enable
 *                              - \ref MCAN_TXBTIE_TIE29     : MCAN_TXBTIE setting to set Transmission Interrupt 29 Enable
 *                              - \ref MCAN_TXBTIE_TIE30     : MCAN_TXBTIE setting to set Transmission Interrupt 30 Enable
 *                              - \ref MCAN_TXBTIE_TIE31     : MCAN_TXBTIE setting to set Transmission Interrupt 31 Enable
 *
 * @return      None.
 *
 * @details     This function enables the interrupts of given Message Buffers.
 */
void MCAN_EnableMbInterrupts(MCAN_T *mcan, uint32_t u32IntSel)
{
    mcan->TXBTIE |= u32IntSel;
}

/**
 * @brief       Disables MCAN Message Tx Buffer Transmission interrupts .
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u32IntSel   Interrupt type select
 *                              - \ref MCAN_TXBTIE_TIE0      : MCAN_TXBTIE setting to set Transmission Interrupt 0 Enable
 *                              - \ref MCAN_TXBTIE_TIE1      : MCAN_TXBTIE setting to set Transmission Interrupt 1 Enable
 *                              - \ref MCAN_TXBTIE_TIE2      : MCAN_TXBTIE setting to set Transmission Interrupt 2 Enable
 *                              - \ref MCAN_TXBTIE_TIE3      : MCAN_TXBTIE setting to set Transmission Interrupt 3 Enable
 *                              - \ref MCAN_TXBTIE_TIE4      : MCAN_TXBTIE setting to set Transmission Interrupt 4 Enable
 *                              - \ref MCAN_TXBTIE_TIE5      : MCAN_TXBTIE setting to set Transmission Interrupt 5 Enable
 *                              - \ref MCAN_TXBTIE_TIE6      : MCAN_TXBTIE setting to set Transmission Interrupt 6 Enable
 *                              - \ref MCAN_TXBTIE_TIE7      : MCAN_TXBTIE setting to set Transmission Interrupt 7 Enable
 *                              - \ref MCAN_TXBTIE_TIE8      : MCAN_TXBTIE setting to set Transmission Interrupt 8 Enable
 *                              - \ref MCAN_TXBTIE_TIE9      : MCAN_TXBTIE setting to set Transmission Interrupt 9 Enable
 *                              - \ref MCAN_TXBTIE_TIE10     : MCAN_TXBTIE setting to set Transmission Interrupt 10 Enable
 *                              - \ref MCAN_TXBTIE_TIE11     : MCAN_TXBTIE setting to set Transmission Interrupt 11 Enable
 *                              - \ref MCAN_TXBTIE_TIE12     : MCAN_TXBTIE setting to set Transmission Interrupt 12 Enable
 *                              - \ref MCAN_TXBTIE_TIE13     : MCAN_TXBTIE setting to set Transmission Interrupt 13 Enable
 *                              - \ref MCAN_TXBTIE_TIE14     : MCAN_TXBTIE setting to set Transmission Interrupt 14 Enable
 *                              - \ref MCAN_TXBTIE_TIE15     : MCAN_TXBTIE setting to set Transmission Interrupt 15 Enable
 *                              - \ref MCAN_TXBTIE_TIE16     : MCAN_TXBTIE setting to set Transmission Interrupt 16 Enable
 *                              - \ref MCAN_TXBTIE_TIE17     : MCAN_TXBTIE setting to set Transmission Interrupt 17 Enable
 *                              - \ref MCAN_TXBTIE_TIE18     : MCAN_TXBTIE setting to set Transmission Interrupt 18 Enable
 *                              - \ref MCAN_TXBTIE_TIE19     : MCAN_TXBTIE setting to set Transmission Interrupt 19 Enable
 *                              - \ref MCAN_TXBTIE_TIE20     : MCAN_TXBTIE setting to set Transmission Interrupt 20 Enable
 *                              - \ref MCAN_TXBTIE_TIE21     : MCAN_TXBTIE setting to set Transmission Interrupt 21 Enable
 *                              - \ref MCAN_TXBTIE_TIE22     : MCAN_TXBTIE setting to set Transmission Interrupt 22 Enable
 *                              - \ref MCAN_TXBTIE_TIE23     : MCAN_TXBTIE setting to set Transmission Interrupt 23 Enable
 *                              - \ref MCAN_TXBTIE_TIE24     : MCAN_TXBTIE setting to set Transmission Interrupt 24 Enable
 *                              - \ref MCAN_TXBTIE_TIE25     : MCAN_TXBTIE setting to set Transmission Interrupt 25 Enable
 *                              - \ref MCAN_TXBTIE_TIE26     : MCAN_TXBTIE setting to set Transmission Interrupt 26 Enable
 *                              - \ref MCAN_TXBTIE_TIE27     : MCAN_TXBTIE setting to set Transmission Interrupt 27 Enable
 *                              - \ref MCAN_TXBTIE_TIE28     : MCAN_TXBTIE setting to set Transmission Interrupt 28 Enable
 *                              - \ref MCAN_TXBTIE_TIE29     : MCAN_TXBTIE setting to set Transmission Interrupt 29 Enable
 *                              - \ref MCAN_TXBTIE_TIE30     : MCAN_TXBTIE setting to set Transmission Interrupt 30 Enable
 *                              - \ref MCAN_TXBTIE_TIE31     : MCAN_TXBTIE setting to set Transmission Interrupt 31 Enable
 *
 * @return      None.
 *
 * @details     This function disables the interrupts of given Message Buffers.
 */
void MCAN_DisableMbInterrupts(MCAN_T *mcan, uint32_t u32IntSel)
{
    mcan->TXBTIE &= ~u32IntSel;
}



/**
 * @brief       Get MCAN interrupts status.
 *
 * @param[in]   mcan            The pointer of the specified MCAN module.
 * @param[in]   u32IntTypeFlag  Interrupt Type Flag, should be
 *                              - \ref MCAN_IR_ARA_Msk     : Access to Reserved Address interrupt Indicator
 *                              - \ref MCAN_IR_PED_Msk     : Protocol Error in Data Phase interrupt Indicator
 *                              - \ref MCAN_IR_PEA_Msk     : Protocol Error in Arbitration Phase interrupt Indicator
 *                              - \ref MCAN_IR_WDI_Msk     : Watchdog interrupt Indicator
 *                              - \ref MCAN_IR_BO_Msk      : Bus_Off Status interrupt Indicator
 *                              - \ref MCAN_IR_EW_Msk      : Warning Status interrupt Indicator
 *                              - \ref MCAN_IR_EP_Msk      : Error Passive interrupt Indicator
 *                              - \ref MCAN_IR_ELO_Msk     : Error Logging Overflow interrupt Indicator
 *                              - \ref MCAN_IR_BEU_Msk     : Bit Error Uncorrected interrupt Indicator
 *                              - \ref MCAN_IR_BEC_Msk     : Bit Error Corrected interrupt Indicator
 *                              - \ref MCAN_IR_DRX_Msk     : Message stored to Dedicated Rx Buffer interrupt Indicator 
 *                              - \ref MCAN_IR_TOO_Msk     : Timeout Occurred interrupt Indicator
 *                              - \ref MCAN_IR_MRAF_Msk    : Message RAM Access Failure interrupt Indicator
 *                              - \ref MCAN_IR_TSW_Msk     : Timestamp Wraparound interrupt Indicator
 *                              - \ref MCAN_IR_TEFL_Msk    : Tx Event FIFO Event Lost interrupt Indicator
 *                              - \ref MCAN_IR_TEFF_Msk    : Tx Event FIFO Full Indicator
 *                              - \ref MCAN_IR_TEFW_Msk    : Tx Event FIFO Watermark Reached Interrupt Indicator
 *                              - \ref MCAN_IR_TEFN_Msk    : Tx Event FIFO New Entry Interrupt Indicator
 *                              - \ref MCAN_IR_TFE_Msk     : Tx FIFO Empty Interrupt Indicator
 *                              - \ref MCAN_IR_TCF_Msk     : Transmission Cancellation Finished Interrupt Indicator
 *                              - \ref MCAN_IR_TC_Msk      : Transmission Completed interrupt Indicator
 *                              - \ref MCAN_IR_HPM_Msk     : High Priority Message Interrupt Indicator
 *                              - \ref MCAN_IR_RF1L_Msk    : Rx FIFO 1 Message Lost Interrupt Indicator
 *                              - \ref MCAN_IR_RF1F_Msk    : Rx FIFO 1 Full Interrupt Indicator
 *                              - \ref MCAN_IR_RF1W_Msk    : Rx FIFO 1 Watermark Reached Interrupt Indicator
 *                              - \ref MCAN_IR_RF1N_Msk    : Rx FIFO 1 New Message Interrupt Indicator
 *                              - \ref MCAN_IR_RF0L_Msk    : Rx FIFO 0 Message Lost Interrupt Indicator
 *                              - \ref MCAN_IR_RF0F_Msk    : Rx FIFO 0 Full Interrupt Indicator
 *                              - \ref MCAN_IR_RF0W_Msk    : Rx FIFO 0 Watermark Reached Interrupt Indicator
 *                              - \ref MCAN_IR_RF0N_Msk    : Rx FIFO 0 New Message Interrupt Indicator
 *
 * @return      None.
 *
 * @details     This function gets all MCAN interrupt status flags.
 */

uint32_t MCAN_GetStatusFlag(MCAN_T *mcan, uint32_t u32IntTypeFlag)
{
    return (mcan->IR & u32IntTypeFlag);
}

/**
 * @brief       Clears the MCAN module interrupt flags
 *
 * @param[in]   mcan              The pointer of the specified MCAN module.
 * @param[in]   u32InterruptFlag  The specified interrupt of UART module
 *                               - \ref MCAN_IR_ARA_Msk     : Access to Reserved Address interrupt Indicator
 *                               - \ref MCAN_IR_PED_Msk     : Protocol Error in Data Phase interrupt Indicator
 *                               - \ref MCAN_IR_PEA_Msk     : Protocol Error in Arbitration Phase interrupt Indicator
 *                               - \ref MCAN_IR_WDI_Msk     : Watchdog interrupt Indicator
 *                               - \ref MCAN_IR_BO_Msk      : Bus_Off Status interrupt Indicator
 *                               - \ref MCAN_IR_EW_Msk      : Warning Status interrupt Indicator
 *                               - \ref MCAN_IR_EP_Msk      : Error Passive interrupt Indicator
 *                               - \ref MCAN_IR_ELO_Msk     : Error Logging Overflow interrupt Indicator
 *                               - \ref MCAN_IR_BEU_Msk     : Bit Error Uncorrected interrupt Indicator
 *                               - \ref MCAN_IR_BEC_Msk     : Bit Error Corrected interrupt Indicator
 *                               - \ref MCAN_IR_DRX_Msk     : Message stored to Dedicated Rx Buffer interrupt Indicator 
 *                               - \ref MCAN_IR_TOO_Msk     : Timeout Occurred interrupt Indicator
 *                               - \ref MCAN_IR_MRAF_Msk    : Message RAM Access Failure interrupt Indicator
 *                               - \ref MCAN_IR_TSW_Msk     : Timestamp Wraparound interrupt Indicator
 *                               - \ref MCAN_IR_TEFL_Msk    : Tx Event FIFO Event Lost interrupt Indicator
 *                               - \ref MCAN_IR_TEFF_Msk    : Tx Event FIFO Full Indicator
 *                               - \ref MCAN_IR_TEFW_Msk    : Tx Event FIFO Watermark Reached Interrupt Indicator
 *                               - \ref MCAN_IR_TEFN_Msk    : Tx Event FIFO New Entry Interrupt Indicator
 *                               - \ref MCAN_IR_TFE_Msk     : Tx FIFO Empty Interrupt Indicator
 *                               - \ref MCAN_IR_TCF_Msk     : Transmission Cancellation Finished Interrupt Indicator
 *                               - \ref MCAN_IR_TC_Msk      : Transmission Completed interrupt Indicator
 *                               - \ref MCAN_IR_HPM_Msk     : High Priority Message Interrupt Indicator
 *                               - \ref MCAN_IR_RF1L_Msk    : Rx FIFO 1 Message Lost Interrupt Indicator
 *                               - \ref MCAN_IR_RF1F_Msk    : Rx FIFO 1 Full Interrupt Indicator
 *                               - \ref MCAN_IR_RF1W_Msk    : Rx FIFO 1 Watermark Reached Interrupt Indicator
 *                               - \ref MCAN_IR_RF1N_Msk    : Rx FIFO 1 New Message Interrupt Indicator
 *                               - \ref MCAN_IR_RF0L_Msk    : Rx FIFO 0 Message Lost Interrupt Indicator
 *                               - \ref MCAN_IR_RF0F_Msk    : Rx FIFO 0 Full Interrupt Indicator
 *                               - \ref MCAN_IR_RF0W_Msk    : Rx FIFO 0 Watermark Reached Interrupt Indicator
 *                               - \ref MCAN_IR_RF0N_Msk    : Rx FIFO 0 New Message Interrupt Indicator
 *
 * @return      None.
 *
 * @details     This function clears MCAN interrupt status flags.
 */

void MCAN_ClearStatusFlag(MCAN_T *mcan, uint32_t u32InterruptFlag)
{
    /* Write 1 to clear status flag. */
    mcan->IR |= u32InterruptFlag;
}


/**
 * @brief       Gets the MCAN Bus Error Counter value.
 *
 * @param[in]   mcan           The pointer of the specified MCAN module.
 * @param[in]   pu8TxErrBuf    TxErrBuf Buffer to store Tx Error Counter value.
 * @param[in]   pu8RxErrBuf    RxErrBuf Buffer to store Rx Error Counter value.
 *
 * @return      None.
 *
 * @details     This function gets the MCAN Bus Error Counter value for both Tx and Rx direction.
 *              These values may be needed in the upper layer error handling.
 */

void MCAN_GetBusErrCount(MCAN_T *mcan, uint8_t *pu8TxErrBuf, uint8_t *pu8RxErrBuf)
{
    if (pu8TxErrBuf)
    {
        *pu8TxErrBuf = (uint8_t)((mcan->ECR >> MCAN_ECR_TEC_Pos) & MCAN_ECR_TEC_Msk);
    }

    if (pu8RxErrBuf)
    {
        *pu8RxErrBuf = (uint8_t)((mcan->ECR >> MCAN_ECR_REC_Pos) & MCAN_ECR_REC_Msk);
    }
}



/**
 * @brief       Get the default configuration structure.
 *
 * @param[in]   mcan           The pointer of the specified MCAN module.
 * @param[in]   psConfig       Pointer to MCAN configuration structure.
 *
 * @return      None.
 *
 * @details     This function initializes the MCAN configure structure to default value.
 *              The default value are:
 *              u32NominalBaudRate   = 125000;
 *              u32DataBaudRate      = 0;
 *              u32BaseAddress       = 0x20010000;
 *              u32TimestampClock_Hz = 0;
 *              u8RejectStandardRTR = TRUE;
 *              u8RejectExtendedRTR = TRUE;
 *              u8EnableLoopBack    = FALSE;
 *              u8EnableNonISOMode  = FALSE;
 *              u8DisableFD         = FALSE;
 */
void MCAN_GetDefaultConfig(S_MCAM_CONFIG_T *psConfig)
{
    memset(psConfig, 0, sizeof(S_MCAM_CONFIG_T));
    psConfig->u32NominalBaudRate   = 125000;
#ifdef USE_CANFD
    psConfig->u32DataBaudRate      = 0;
#endif
    psConfig->u32BaseAddress       = MACN_RAM_ADDRESS;
    psConfig->u32TimestampClock_Hz = 0;
    psConfig->u8RejectStandardRTR = TRUE;
    psConfig->u8RejectExtendedRTR = TRUE;
    psConfig->u8EnableLoopBack    = FALSE;
#ifdef USE_CANFD
    psConfig->u8EnableNonISOMode  = FALSE;
    psConfig->u8DisableFD         = FALSE;
#endif
}

 
/**
 * @brief       Get the segment values.
 *
 * @param[in]   u32NominalBaudRate  The nominal speed in bps.
 * @param[in]   u32DataBaudRate     The data speed in bps.
 * @param[in]   u32Ntq              Number of nominal time quanta per bit.
 * @param[in]   u32Dtq              Number of data time quanta per bit.
 * @param[in]   psConfig            Passed is a configuration structure, on return the configuration is stored in the structure
 *
 * @return      None.
 *
 * @details     Calculates the segment values for a single bit time for nominal and data baudrates.
 */

static void MCAN_GetSegments(uint32_t u32NominalBaudRate, uint32_t u32DataBaudRate, uint32_t u32Ntq,
        uint32_t u32Dtq, S_MCAN_TIMEING_CONFIG_t *psConfig)
{
    float ideal_sp;
    int int32P1;

    /* get ideal sample point */
    if (u32NominalBaudRate >= 1000000)     ideal_sp = 0.750;
    else if (u32NominalBaudRate >= 800000) ideal_sp = 0.800;
    else                                   ideal_sp = 0.875;

    /* distribute time quanta */
    int32P1 = (int)(u32Ntq * ideal_sp);
    /* can controller doesn't separate prop seg and phase seg 1 */
    psConfig->u8NominalPropSeg = 0;
    /* subtract one TQ for sync seg */
    psConfig->u8NominalPhaseSeg1 = int32P1 - 1;
    psConfig->u8NominalPhaseSeg2 = u32Ntq - int32P1;
    /* sjw is 20% of total TQ, rounded to nearest int */
    psConfig->u8NominalRJumpwidth = (u32Ntq + (5 - 1)) / 5;

#ifdef USE_CANFD
    /* if using baud rate switching then distribute time quanta for data rate */
    if (u32Dtq > 0)
    {
        /* get ideal sample point */
        if (u32DataBaudRate >= 1000000)     ideal_sp = 0.750;
        else if (u32DataBaudRate >= 800000) ideal_sp = 0.800;
        else                             ideal_sp = 0.875;

        /* distribute time quanta */
        int32P1 = (int)(u32Dtq * ideal_sp);
        /* can controller doesn't separate prop seg and phase seg 1 */
        psConfig->u8DataPropSeg = 0;
        /* subtract one TQ for sync seg */
        psConfig->u8DataPhaseSeg1 = int32P1 - 1;
        psConfig->u8DataPhaseSeg2 = u32Dtq - int32P1;
        /* sjw is 20% of total TQ, rounded to nearest int */
        psConfig->u8DataRJumpwidth = (u32Dtq + (5 - 1)) / 5;
    }
    else
    {
        psConfig->u8DataPropSeg = 0;
        psConfig->u8DataPhaseSeg1 = 0;
        psConfig->u8DataPhaseSeg2 = 0;
        psConfig->u8DataRJumpwidth = 0;
    }
#endif
}

/**
 * @brief       Sets the MCAN protocol timing characteristic.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   psConfig    Pointer to the timing configuration structure.
 *
 * @return      None.
 *
 * @details     This function gives user settings to CAN bus timing characteristic.
 *              The function is for an experienced user. For less experienced users, call
 *              the MCAN_Init() and fill the baud rate field with a desired value.
 *              This provides the default timing characteristics to the module.
 */

void MCAN_SetTimingConfig(MCAN_T *mcan, const S_MCAN_TIMEING_CONFIG_t *psConfig)
{
#ifdef USE_CANFD
    uint32_t *pu32DBTP;
#endif
  
    /* configuration change enable */
    mcan->CCCR |= MCAN_CCCR_CCE_Msk;

   if(mcan == (MCAN_T *)MCAN0)
   {    
     /* Get UART clock divider number */
     CLK->CLKDIV4 = (CLK->CLKDIV4 & CLK_CLKDIV4_MCAN0DIV_Msk) | CLK_CLKDIV4_MCAN0(psConfig->u8PreDivider) ;
   }
    else{}

    /* nominal bit rate */
    mcan->NBTP = (((psConfig->u8NominalRJumpwidth & 0x7F) - 1) << 25) +
                 (((psConfig->u16NominalPrescaler & 0x1FF) - 1) << 16) +
                 ((((psConfig->u8NominalPhaseSeg1 + psConfig->u8NominalPropSeg) & 0xFF) - 1) << 8) +
                 (((psConfig->u8NominalPhaseSeg2 & 0x7F) - 1) << 0);

#ifdef USE_CANFD
    /* data bit rate */
    /* mcan->DBTP */
     pu32DBTP = (((uint32_t *)mcan) + 0x03);
    *pu32DBTP = (((psConfig->u8DataPrescaler & 0x1F) - 1) << 16) +
                ((((psConfig->u8DataPhaseSeg1 + psConfig->u8DataPropSeg) & 0x1F) - 1) << 8) +
                (((psConfig->u8DataPhaseSeg2 & 0xF) - 1) << 4) +
                (((psConfig->u8DataRJumpwidth & 0xF) - 1) << 0);
#endif
}


/**
 * @brief       Calculates the CAN controller timing values for specific baudrates.
 *
 * @param[in]   u32NominalBaudRate  The nominal speed in bps.
 * @param[in]   u32DataBaudRate     The data speed in bps. Zero to disable baudrate switching.
 * @param[in]   u32SourceClock_Hz   MCAN Protocol Engine clock source frequency in Hz.
 * @param[in]   psConfig            Passed is a configuration structure, on return the configuration is stored in the structure
 *
 * @return      true if timing configuration found, false if failed to find configuration.
 *
 * @details     Calculates the CAN controller timing values for specific baudrates.
 */

static status_t MCAN_CalculateTimingValues(uint32_t u32NominalBaudRate,
#ifdef USE_CANFD
        uint32_t u32DataBaudRate,
#endif
        uint32_t u32SourceClock_Hz, S_MCAN_TIMEING_CONFIG_t *psConfig)
{
    int i32Nclk;
    int i32Nclk2;
    int i32Ntq;
#ifdef USE_CANFD
    int i32Dclk;
    int i32Dclk2;
    int i32Dtq;
#endif

    /* observe baud rate maximums */
    if (u32NominalBaudRate > MAX_NOMINAL_BAUDRATE) u32NominalBaudRate = MAX_NOMINAL_BAUDRATE;

    for (i32Ntq = MAX_TIME_QUANTA; i32Ntq >= MIN_TIME_QUANTA; i32Ntq--)
    {
        i32Nclk = u32NominalBaudRate * i32Ntq;

        for (psConfig->u16NominalPrescaler = 0x001; psConfig->u16NominalPrescaler <= 0x400; (psConfig->u16NominalPrescaler)++)
        {
            i32Nclk2 = i32Nclk * psConfig->u16NominalPrescaler;

            if (((u32SourceClock_Hz / i32Nclk2) <= 5) && ((float)(u32SourceClock_Hz) / i32Nclk2) == (u32SourceClock_Hz / i32Nclk2))
            {
                psConfig->u8PreDivider = u32SourceClock_Hz / i32Nclk2;

#ifdef USE_CANFD
                /* if not using baudrate switch then we are done */
                if (!u32DataBaudRate)
                {
                    i32Dtq = 0;
                    psConfig->u8DataPrescaler = 0;
                    MCAN_GetSegments(u32NominalBaudRate, u32DataBaudRate, i32Ntq, i32Dtq, psConfig);
                    return TRUE;
                }

                /* if baudrates are the same and the solution for nominal will work for
                data, then use the nominal settings for both */
                if ((u32DataBaudRate == u32NominalBaudRate) && psConfig->u16NominalPrescaler <= 0x20)
                {
                    i32Dtq = i32Ntq;
                    psConfig->u8DataPrescaler = (uint8_t)psConfig->u16NominalPrescaler;
                    MCAN_GetSegments(u32NominalBaudRate, u32DataBaudRate, i32Ntq, i32Dtq, psConfig);
                    return TRUE;
                }

                /* calculate data settings */
                for (i32Dtq = MAX_TIME_QUANTA; i32Dtq >= MIN_TIME_QUANTA; i32Dtq--)
                {
                    i32Dclk = u32DataBaudRate * i32Dtq;

                    for (psConfig->u8DataPrescaler = 0x01; psConfig->u8DataPrescaler <= 0x20; (psConfig->u8DataPrescaler)++)
                    {
                        i32Dclk2 = i32Dclk * psConfig->u8DataPrescaler;

                        if ((float)(u32SourceClock_Hz) / i32Dclk2 == psConfig->u8PreDivider)
                        {
                            MCAN_GetSegments(u32NominalBaudRate, u32DataBaudRate, i32Ntq, i32Dtq, psConfig);
                            return TRUE;
                        }
                    }
                }
#else
                MCAN_GetSegments(u32NominalBaudRate, 0, i32Ntq, 0, psConfig);
                return 1;                
#endif
            }
        }
    }

    /* failed to find solution */
    return 0;
}

/**
 * @brief       Initializes a MCAN instance.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u32SourceClock_Hz   MCAN Protocol Engine clock source frequency in Hz.
 * @param[in]   psConfig            Pointer to user-defined configuration structure
 *
 * @return      None.
 *
 * @details     This function initializes the CAN module with user-defined settings.
 */

void MCAN_Init(MCAN_T *mcan, const S_MCAM_CONFIG_T *psConfig, uint32_t sourceClock_Hz)
{
    S_MCAN_TIMEING_CONFIG_t timingConfig;
    int f;
    int timestampdivisor;

    /* reset offset */
    g_u16RamOffset = 0;

    if(mcan == (MCAN_T *)MCAN0)
    {
      CLK_EnableModuleClock(MCAN0_MODULE);
      MCAN_IPReset();
      NVIC_EnableIRQ(MCAN0_IRQ0_IRQn);
      NVIC_EnableIRQ(MCAN0_IRQ1_IRQn);
    }
    else{}

    /* configuration change enable */
    mcan->CCCR |= MCAN_CCCR_CCE_Msk;
#ifdef USE_CANFD
    /* enable FD and baud-rate switching */
    mcan->CCCR |= MCAN_CCCR_BRSE_Msk;

    if (!psConfig->u8DisableFD)
    {
        mcan->CCCR |= MCAN_CCCR_FDOE_Msk;
    }
#endif
    
    /* no rx FIFOs */
    for (f = 0; f < MCAN_NUM_RX_FIFOS; f++) g_au8RxFIFOSize[f] = 0;
    mcan->RXF0C = 0;
    mcan->RXF1C = 0;

    /* calculate and apply timing */
    if (MCAN_CalculateTimingValues(psConfig->u32NominalBaudRate,
#ifdef USE_CANFD
      psConfig->u32DataBaudRate,
#endif
      sourceClock_Hz, &timingConfig))
    {
        MCAN_SetTimingConfig(mcan, &timingConfig);
    }


    /* define 11-bit filters */
    if (MCAN_STANDARD_FILTER_ELM_NUM > 0)
    {
        mcan->SIDFC = ((MCAN_STANDARD_FILTER_ELM_NUM & 0xFF) << 16) | (g_u16RamOffset & 0xFFFF);
        memset((uint32_t *)(MACN_RAM_ADDRESS+ g_u16RamOffset), 0x00, MCAN_STANDARD_FILTER_ELM_NUM * sizeof(S_MCAN_STD_FILTER_T));
        g_u16RamOffset += MCAN_STANDARD_FILTER_ELM_NUM * sizeof(S_MCAN_STD_FILTER_T);
    }

    /* define 29-bit filters */
    if (MCAN_EXTENDED_FILTER_ELM_NUM > 0)
    {
        mcan->XIDFC = ((MCAN_EXTENDED_FILTER_ELM_NUM & 0x7F) << 16) | (g_u16RamOffset & 0xFFFF);
        memset((uint32_t *)(MACN_RAM_ADDRESS + g_u16RamOffset), 0x00, MCAN_EXTENDED_FILTER_ELM_NUM * sizeof(S_MCAN_EXT_FILTER_T));
        g_u16RamOffset += MCAN_EXTENDED_FILTER_ELM_NUM * sizeof(S_MCAN_EXT_FILTER_T);
    }

    /* set number of tx message buffers */
    mcan->TXBC = ((MCAN_TX_BUFFER_ELM_NUM & 0x3F) << 16) + (g_u16RamOffset & 0xFFFF);
    /* set tx buffer elements size */
    mcan->TXESC = MCAN_NumberofBytestoFieldSize(MCAN_MAX_MESSAGE_BYTES);
    memset((uint32_t *)(MACN_RAM_ADDRESS + g_u16RamOffset), 0x00, MCAN_TX_BUFFER_ELM_NUM * sizeof(S_MCAN_BUF_T));
    g_u16RamOffset += MCAN_TX_BUFFER_ELM_NUM * sizeof(S_MCAN_BUF_T);

    /* set location of rx message buffers */
    mcan->RXBC = g_u16RamOffset & 0xFFFF;
    /* set rx message buffer elements size */
    mcan->RXESC = MCAN_NumberofBytestoFieldSize(MCAN_MAX_MESSAGE_BYTES) << 8;
    memset((uint32_t *)(MACN_RAM_ADDRESS+ g_u16RamOffset), 0x00, MCAN_RX_BUFFER_ELM_NUM * sizeof(S_MCAN_BUF_T));
    g_u16RamOffset += MCAN_RX_BUFFER_ELM_NUM * sizeof(S_MCAN_BUF_T);

    /* set location of rx fifo0 buffers */
    MCAN_SetRxFifoConfig(mcan,0,MCAN_RX_FIFO0_ELM_NUM,TRUE);
    /* set location of rx fifo1 buffers */
    MCAN_SetRxFifoConfig(mcan,1,MCAN_RX_FIFO1_ELM_NUM,TRUE);
    
    /* default configuration is to reject all non-matching frames */
    mcan->GFC = MCAN_GFC_ANFS_Msk | MCAN_GFC_ANFE_Msk;

    /* RTR handling */
    if (psConfig->u8RejectStandardRTR) mcan->GFC |= MCAN_GFC_RRFS_Msk;
    if (psConfig->u8RejectExtendedRTR) mcan->GFC |= MCAN_GFC_RRFE_Msk;

    /* enable interrupt 0, all interrupts use interrupt 0 */
//    mcan->ILS = 0;
//    mcan->ILE = MCAN_ILE_EINT0_Msk;

    if (psConfig->u8EnableLoopBack)
    {
        mcan->CCCR |= MCAN_CCCR_TEST_Msk;
        mcan->TEST |= MCAN_TEST_LBCK_Msk;
    }

#ifdef USE_CANFD
    if (psConfig->u8EnableNonISOMode)
    {
        mcan->CCCR |= MCAN_CCCR_NISO_Msk;
    }
#endif
    
    if (psConfig->u32TimestampClock_Hz)
    {
        /* use external timestamp counter */
        mcan->TSCC = MCAN_TSCC_TSS_EXTCOUNTER;
    }
}

/**
 * @brief       Reads a message frame from a receive buffer.
 *
 * @param[in]   psRxBuffer     Buffer to read from.
 * @param[in]   psRxFrame      Location to store read message.
 *
 * @return      None.
 *
 * @details     This function is readed a message frame from a receive buffer.
 */

static void MCAN_ReadRxMessage(S_MCAN_BUF_T *psRxBuffer, S_MCAN_FRAME_T *psRxFrame)
{
    uint32_t u32Idx ;

    /* if 29-bit ID */
    if (psRxBuffer->u32Id & MCAN_BUF_ID_XTD)
    {
        psRxFrame->id = psRxBuffer->u32Id & 0x1FFFFFFF;
        psRxFrame->format = eMCAN_FrameFormatExtend;
    }
    /* if 11-bit ID */
    else
    {
        psRxFrame->id = (psRxBuffer->u32Id >> 18) & 0x7FF;
        psRxFrame->format = eMCAN_FrameFormatStandard;
    }

#ifdef USE_CANFD
    if (psRxBuffer->u32Config & MCAN_BUF_CONFIG_FDF)
        psRxFrame->proto = eMCAN_ProtoTypeFD;
    else
#endif
        psRxFrame->proto = eMCAN_ProtoTypeClassic;

#ifdef USE_CANFD
    if (psRxBuffer->u32Config  & MCAN_BUF_CONFIG_BRS)
        psRxFrame->bitratemode = eMCAN_BitrateModeTypeSwitch;
    else
        psRxFrame->bitratemode = eMCAN_BitrateModeTypeNoSwitch;
#endif
    
    psRxFrame->timestamp = psRxBuffer->u32Config & MCAN_BUF_CONFIG_RXTS_Msk;

    if (psRxBuffer->u32Id & MCAN_BUF_ID_RTR)
        psRxFrame->type = eMCAN_FrameTypeRemote;
    else
        psRxFrame->type = eMCAN_FrameTypeData;

    psRxFrame->length = MCAN_DecodeDLC((psRxBuffer->u32Config >> 16) & 0xF);

    for (u32Idx = 0; u32Idx < (uint32_t)(psRxFrame->length + (4 - 1)) / 4; u32Idx++)
    {
        psRxFrame->dataWord[u32Idx] = psRxBuffer->u32Data[u32Idx];
    }
}

/**
 * @brief       Enables or disables the MCAN module operation.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8Enable    true to enable, false to disable.
 *
 * @return      None.
 *
 * @details     This function enables or disables the MCAN module.
 */

void MCAN_Enable(MCAN_T *mcan, uint8_t u8Enable)
{
    if (u8Enable)
    {
        /* start operation */
        mcan->CCCR &= ~(MCAN_CCCR_CCE_Msk | MCAN_CCCR_INIT_Msk);
        while (mcan->CCCR & MCAN_CCCR_INIT_Msk);
    }
    else
    {
        /* init mode */
        mcan->CCCR |= MCAN_CCCR_INIT_Msk;
        while (!(mcan->CCCR & MCAN_CCCR_INIT_Msk));
    }
}

/**
 * @brief       Aborts transmission on a MCAN transmit message buffer.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8MbIdx     The Message Buffer index.
 *
 * @return      None.
 *
 * @details     This function aborts the previous transmission and cleans the Message Buffer.
 */

void MCAN_AbortTxMb(MCAN_T *mcan, uint8_t u8MbIdx)
{
    if (u8MbIdx >= MCAN_TX_BUFFER_ELM_NUM) return;

    /* abort any current transmission in this message buffer */
    if (mcan->TXBRP & (1UL << u8MbIdx))
    {
        mcan->TXBCR |= (1UL << u8MbIdx);
        while (mcan->TXBRP & (1UL << u8MbIdx));
    }
}

/**
 * @brief       Writes a CAN Message to Transmit Message Buffer.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8MbIdx     The Message Buffer index.
 * @param[in]   psTxFrame   Pointer to MCAN message frame to be sent.
 *
 * @return      eStatus_Success  Write Tx Message Buffer Successfully.
 *              eStatus_Fail     Tx Message Buffer is currently in use.
 *
 * @details     This function writes a MCAN Message to the specified Transmit Message Buffer
 *              and changes the Message Buffer state to start MCAN Message transmit. After
 *              that the function returns immediately.
 */

status_t MCAN_WriteTxMb(MCAN_T *mcan, uint8_t u8MbIdx, const S_MCAN_FRAME_T *psTxFrame)
{
    S_MCAN_BUF_T *psTxBuffer;
    int b;

    if (u8MbIdx >= MCAN_TX_BUFFER_ELM_NUM) return eStatus_Fail;

    /* transmission is pending in this message buffer */
    if (mcan->TXBRP & (1UL << u8MbIdx)) return eStatus_Fail;

    psTxBuffer = (S_MCAN_BUF_T *)(MACN_RAM_ADDRESS + (mcan->TXBC & 0xFFFF) + (u8MbIdx * sizeof(S_MCAN_BUF_T)));

    if (psTxFrame->format == eMCAN_FrameFormatExtend)
    {
        psTxBuffer->u32Id = MCAN_BUF_ID_XTD | (psTxFrame->id & 0x1FFFFFFF);
    }
    else
    {
        psTxBuffer->u32Id = (psTxFrame->id & 0x7FF) << 18;
    }

    if (psTxFrame->type == eMCAN_FrameTypeRemote) psTxBuffer->u32Id |= MCAN_BUF_ID_RTR;

    psTxBuffer->u32Config = (MCAN_EncodeDLC(psTxFrame->length) << 16);
#ifdef USE_CANFD
    if (psTxFrame->proto == eMCAN_ProtoTypeFD) psTxBuffer->u32Config |= MCAN_BUF_CONFIG_FDF;
    if (psTxFrame->bitratemode == eMCAN_BitrateModeTypeSwitch) psTxBuffer->u32Config |= MCAN_BUF_CONFIG_BRS;
#endif
    
    for (b = 0; b < (int)(psTxFrame->length + (4 - 1)) / 4; b++)
    {
        psTxBuffer->u32Data[b] = psTxFrame->dataWord[b];
    }

    mcan->TXBAR = (1 << u8MbIdx);

    return eStatus_Success;
}


/**
 * @brief       Performs a polling send transaction on the MCAN bus.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8MbIdx     The Message Buffer index.
 * @param[in]   psTxFrame   Pointer to MCAN message frame to be sent.
 *
 * @return      eStatus_Success  Write Tx Message Buffer Successfully.
 *              eStatus_Fail     Tx Message Buffer is currently in use.
 *
 * @details     Note that a transfer handle does not need to be created  before calling this API.
 */

status_t MCAN_TransferSendBlocking(MCAN_T *mcan, uint8_t u8MbIdx, S_MCAN_FRAME_T *psTxFrame)
{
    /* write the message to the message buffer */
    int status = MCAN_WriteTxMb(mcan, u8MbIdx, psTxFrame);
    if (status == eStatus_Success)
    {
        /* wait for completion */
        while (!(mcan->TXBRP & (1UL << u8MbIdx)));
    }

    return status;
}

/**
 * @brief       Sets the MCAN receive global mask.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u32Filter   Rx Global Mask value.
 *
 * @return      eStatus_Success  Write Tx Message Buffer Successfully.
 *              eStatus_Fail     Tx Message Buffer is currently in use.
 *
 * @details     This function sets the global mask for MCAN in a matching process.
 */

void MCAN_SetRxGlobalMask(MCAN_T *mcan, uint32_t u32Filter)
{
    mcan->GFC &= (MCAN_GFC_RRFS_Msk | MCAN_GFC_RRFE_Msk);
    mcan->GFC |= (u32Filter & (MCAN_GFC_ANFS_Msk | MCAN_GFC_ANFE_Msk));
}


/**
 * @brief       Sets the MCAN receive individual mask.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8MaskIdx   The Index of individual Mask.
 * @param[in]   u32Filter   Rx Individual filter value.
 *
 * @return      None.
 *
 * @details     This function sets the individual mask for CAN matching process.
 */


void MCAN_SetRxIndividualMask(MCAN_T *mcan, uint8_t u8MaskIdx, uint32_t u32Filter)
{
    S_MCAN_STD_FILTER_T *psFilter;

    /* ignore if index is too high */
    if (u8MaskIdx >= MCAN_STANDARD_FILTER_ELM_NUM ) return;

    psFilter = (S_MCAN_STD_FILTER_T *)(MACN_RAM_ADDRESS + (mcan->SIDFC & 0xFFFF) + (u8MaskIdx * sizeof(S_MCAN_STD_FILTER_T)));
    psFilter->VALUE = u32Filter;
}

/**
 * @brief       Sets the MCAN receive individual mask.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8MaskIdx   The Index of individual Mask.
 * @param[in]   u32Filter   Rx Individual filter low value.
 * @param[in]   u32Filter   Rx Individual filter high value.
 *
 * @return      None.
 *
 * @details     This function sets the extended individual mask for MCAN matching process.
 */

void MCAN_SetRxExtIndividualMask(MCAN_T *mcan, uint8_t u8MaskIdx, uint32_t u32FilterLow, uint32_t u32FilterHigh)
{
    S_MCAN_EXT_FILTER_T *psFilter;

    /* ignore if index is too high */
    if (u8MaskIdx >= MCAN_EXTENDED_FILTER_ELM_NUM) return;

    psFilter = (S_MCAN_EXT_FILTER_T *)(MACN_RAM_ADDRESS + (mcan->XIDFC & 0xFFFF) + (u8MaskIdx * sizeof(S_MCAN_EXT_FILTER_T)));
    psFilter->LOWVALUE = u32FilterLow;
    psFilter->HIGHVALUE = u32FilterHigh;
}

/**
 * @brief       Reads a MCAN Message from Rx FIFO.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8FifoIdx   Number of the FIFO, 0 or 1.
 * @param[in]   psRxFrame   Pointer to CAN message frame structure for reception.
 *
  * @return     eStatus_Success          Read Message from Rx FIFO successfully.
                eStatus_MCAN_RxOverflow  Rx FIFO is already overflowed and has been read successfully
 *              eStatus_Fail             Rx FIFO is not enabled.
 *
 * @details     This function reads a CAN message from the MCAN build-in Rx FIFO.
 */

status_t MCAN_ReadRxFifo(MCAN_T *mcan, uint8_t u8FifoIdx, S_MCAN_FRAME_T *psRxFrame)
{
    S_MCAN_BUF_T *pRxBuffer;
    uint8_t GetIndex;
    status_t status = eStatus_Fail;
    __I  uint32_t *pRXFS;
    __IO uint32_t *pRXFC, *pRXFA;
    uint8_t msgLostBit;

    /* check for valid FIFO number */
    if (u8FifoIdx < MCAN_NUM_RX_FIFOS)
    {
        if (u8FifoIdx == 0)
        {
            pRXFS = &(mcan->RXF0S);
            pRXFC = &(mcan->RXF0C);
            pRXFA = &(mcan->RXF0A);
            msgLostBit = 3;
        }
        else
        {
            pRXFS = &(mcan->RXF1S);
            pRXFC = &(mcan->RXF1C);
            pRXFA = &(mcan->RXF1A);
            msgLostBit = 7;
        }

        /* if FIFO is not empty */
        if ((*pRXFS & 0x7F) > 0)
        {
            GetIndex = (uint8_t)((*pRXFS >> 8) & 0x3F);

            pRxBuffer = (S_MCAN_BUF_T *)(MACN_RAM_ADDRESS + (*pRXFC & 0xFFFF) + (GetIndex * sizeof(S_MCAN_BUF_T)));

            MCAN_ReadRxMessage(pRxBuffer, psRxFrame);

            /* we got the message */
            *pRXFA = GetIndex;

            /* check for overflow */
            if (*pRXFS & MCAN_RXFS_RFL)
            {
                /* clear overflow flag */
                mcan->IR = (1UL << msgLostBit);

                status = eStatus_MCAN_RxOverflow;
            }
            else
            {
                status = eStatus_Success;
            }
        }
    }

    return status;
}


/**
 * @brief       Performs a polling receive transaction from Rx FIFO on the CAN bus.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8FifoIdx   Number of the FIFO, 0 or 1.
 * @param[in]   psRxFrame   Pointer to MCAN message frame structure for reception.
 *
 * @return      eStatus_Success          Read Message from Rx FIFO successfully.
 *              eStatus_MCAN_RxOverflow  Rx FIFO is already overflowed and has been read successfully
 *              eStatus_Fail             Rx FIFO is not enabled.
 *
 * @details     Note that a transfer handle does not need to be created  before calling this API.
 */

status_t MCAN_TransferReceiveFifoBlocking(MCAN_T *mcan, uint8_t u8FifoIdx, S_MCAN_FRAME_T *psRxFrame)
{
    status_t status;

    /* check if FIFO is enabled */
    if (u8FifoIdx == 0)
    {
        if (((mcan->RXF0C >> 16) & 0x7F) == 0) return eStatus_Fail;
    }
    else
    {
        if (((mcan->RXF1C >> 16) & 0x7F) == 0) return eStatus_Fail;
    }

    while ((status = MCAN_ReadRxFifo(mcan, u8FifoIdx, psRxFrame)) == eStatus_Fail);

    return status;
}


/**
 * @brief       Reads a CAN Message from Receive Message Buffer.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8MbIdx     The MCAN Message Buffer index.
 * @param[in]   psRxFrame   Pointer to MCAN message frame structure for reception.
 *
 * @return     eStatus_Success         Rx Message Buffer is full and has been read successfully.
 *             eStatus_Fail            Rx Message Buffer is empty.
 *
 * @details     This function reads a CAN message from a specified Receive Message Buffer.
 *              The function fills a receive CAN message frame structure with just received data
 *              and activates the Message Buffer again.The function returns immediately.
 */


status_t MCAN_ReadRxMb(MCAN_T *mcan, uint8_t u8MbIdx, S_MCAN_FRAME_T *psRxFrame)
{
    S_MCAN_BUF_T *psRxBuffer;
    status_t status = eStatus_Fail;
    uint32_t newData = 0;

    if (u8MbIdx < MCAN_NUM_RX_MESSAGE_BUFFERS)
    {
        if (u8MbIdx < 32)
            newData = (mcan->NDAT1 >> u8MbIdx) & 1;
        else
            newData = (mcan->NDAT2 >> (u8MbIdx - 32)) & 1;

        /* new message is waiting to be read */
        if (newData)
        {
            /* get memory location of rx buffer */
            psRxBuffer = (S_MCAN_BUF_T *)(MACN_RAM_ADDRESS + (mcan->RXBC & 0xFFFF) + (u8MbIdx * sizeof(S_MCAN_BUF_T)));

            /* read the message */
            MCAN_ReadRxMessage(psRxBuffer, psRxFrame);

            /* clear 'new data' flag */
            if (u8MbIdx < 32)
                mcan->NDAT1 |= (1UL << u8MbIdx);
            else
                mcan->NDAT2 |= (1UL << (u8MbIdx - 32));

            status = eStatus_Success;
        }
    }

    return status;
}


/**
 * @brief       Performs a polling receive transaction on the MCAN bus.
 *
 * @param[in]   mcan        The pointer of the specified MCAN module.
 * @param[in]   u8MbIdx     The MCAN Message Buffer index.
 * @param[in]   psRxFrame   Pointer to MCAN message frame structure for reception.
 *
 * @return     eStatus_Success         Rx Message Buffer is full and has been read successfully.
 *             eStatus_Fail            Rx Message Buffer is empty.
 *
 * @details     Note that a transfer handle does not need to be created  before calling this API.
 */

status_t MCAN_TransferReceiveBlocking(MCAN_T *mcan, uint8_t mbIdx, S_MCAN_FRAME_T *psRxFrame)
{
    while (MCAN_ReadRxMb(mcan, mbIdx, psRxFrame) != eStatus_Success);

    return eStatus_Success;
}


/*@}*/ /* end of group MCAN_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group MCAN_Driver */

/*@}*/ /* end of group Standard_Driver */

/*** (C) COPYRIGHT 2020 Nuvoton Technology Corp. ***/
