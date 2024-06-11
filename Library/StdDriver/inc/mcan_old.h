/****************************************************************************
 * @file     mcan.h
 * @version  V1.00
 * @brief    M253 series MCAN driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __MCAN_H__
#define __MCAN_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup MCAN_Driver MCAN Driver
  @{
*/

/** @addtogroup MCAN_EXPORTED_CONSTANTS MCAN Exported Constants
  @{
*/


/* MCAN message RAM Address */
#define MACN_RAM_ADDRESS                          MCAN_BASE + 0x200UL
/* MCAN message RAM buffer configuration settings  */
#define MCAN_STANDARD_FILTER_ELM_ADDRESS          0x0000UL        /* MCAN Standard message ID elements sram start address   */
#define MCAN_STANDARD_FILTER_ELM_NUM              12              /* The size setting as 12 elements    */ 
#define MCAN_EXTENDED_FILTER_ELM_ADDRESS          0x0030UL        /* MCAN ETD elements sram start address       */
#define MCAN_EXTENDED_FILTER_ELM_NUM              10              /* The size setting as 12 elements    */ 
#define MCAN_RX_FIFO0_ELM_ADDRESS                 0x0090UL        /* RX FIFO0 elements  start address */
#define MCAN_RX_FIFO0_ELM_NUM                     3               /* The size setting as 3 elements     */
#define MCAN_RX_FIFO1_ELM_ADDRESS                 0x0138U         /* RX FIFO1 elements start address    */
#define MCAN_RX_FIFO1_ELM_NUM                     3               /* The size setting as 3 elements     */
#define MCAN_RX_BUFFER_ELM_ADDRESS                0x210U          /* RX Buffer elements start address   */
#define MCAN_RX_BUFFER_ELM_NUM                    3               /* The size setting as 3 elements    */
#define MCAN_TX_BUFFER_ELM_ADDRESS                0x2E8U          /* TX Buffer elements start address    */
#define MCAN_TX_BUFFER_ELM_NUM                    3                /* The size setting as 3 elements   */
#define MCAN_TX_EVENT_FIFO_ELM_ADDRESS            0x3C0U          /* TX Event FIFO element start address */
#define MCAN_TX_EVENT_FIFO_ELM_NUM                5               /* The size setting as 5 elements */

/* Minimum number of time quanta in a bit. */
#define MIN_TIME_QUANTA 9
/* Maximum number of time quanta in a bit. */
#define MAX_TIME_QUANTA 20
/* Number of receive FIFOs (1 - 2) */
#define MCAN_NUM_RX_FIFOS 2


#ifdef USE_CANFD
/*! @brief Maximum size of a CAN frame. Must be a valid CAN FD value */
#define MCAN_MAX_MESSAGE_BYTES 64
#else
/*! @brief Maximum size of a CAN frame. Must be a valid CAN value */
#define MCAN_MAX_MESSAGE_BYTES 8
#endif  

#define MCAN_MAX_MESSAGE_WORDS MCAN_MAX_MESSAGE_BYTES >> 2 

/*! Maximum nominal baudrate in bps. */
#define MAX_NOMINAL_BAUDRATE     (1000000UL)
/*! MCAN CAN mode maximum data baudrate in bps */
#define MAX_MCAN_CAN_BAUDRATE    (1000000UL)
/*! MCAN CAN-FD mode maximum data baudrate in bps */
#define MAX_MCAN_CANFD_BAUDRATE  (6000000UL)

/*! Number of receive FIFOs (1 - 2) */
#define MCAN_NUM_RX_FIFOS 2
/*! Number of transmit message buffers (1 - 32) */
#define MCAN_NUM_TX_MESSAGE_BUFFERS 32
/*! Number of receive message buffers. Cannot be changed */
#define MCAN_NUM_RX_MESSAGE_BUFFERS 64


#define MCAN_TSCC_TSS_DISABLED   (0x00 << 0)
#define MCAN_TSCC_TSS_INTCOUNTER (0x01 << 0)
#define MCAN_TSCC_TSS_EXTCOUNTER (0x02 << 0)

/** Access to tx and rx message buffer fields - ID */
#define MCAN_BUF_ID_XTD_Pos    30
#define MCAN_BUF_ID_RTR_Pos    29
#define MCAN_BUF_ID_ESI        (1UL << 31)
#define MCAN_BUF_ID_XTD        (1UL << MCAN_BUF_ID_XTD_Pos)
#define MCAN_BUF_ID_RTR        (1UL << MCAN_BUF_ID_RTR_Pos)
#define MCAN_BUF_ID_STDID_Msk  (0x7FFUL << 18)
#define MCAN_BUF_ID_EXTID_Msk  (0x1FFFFFFFUL << 0)

/** Acces to tx and rx message buffer fields - config */
/** Acces to tx and rx message buffer fields - config */
#define MCAN_BUF_CONFIG_ANMF       (1UL << 31)
#define MCAN_BUF_CONFIG_FIDX_Msk  (0x7FUL << 24)
#ifdef USE_CANFD
#define MCAN_BUF_CONFIG_FDF       (1UL << 21)
#define MCAN_BUF_CONFIG_BRS       (1UL << 20)
#endif
#define MCAN_BUF_CONFIG_DLC_Msk    (0xFUL << 16)
#define MCAN_BUF_CONFIG_RXTS_Msk  (0xFFFFUL << 0)


#define  MCAN_RXFS_RFL             MCAN_RXF0S_RF0L_Msk

/*  Receive message buffer helper macro */
#define MCAN_RX_MB_STD(id, mbIdx)               (7UL << 27) | ((id & 0x7FF) << 16) | (mbIdx & 0x3F)

/* Receive message buffer extended helper macro - low */
#define MCAN_RX_MB_EXT_LOW(id, mbIdx)           (7UL << 29) | (id & 0x1FFFFFFFUL)

/*  Receive message buffer extended helper macro - high */
#define MCAN_RX_MB_EXT_HIGH(id, mbIdx)          (mbIdx & 0x3FUL)

/*  CAN Rx FIFO 0 Mask helper macro. */
#define MCAN_RX_FIFO0_STD_MASK(match, mask)     (2UL << 30) | (1UL << 27) | ((match & 0x7FF) << 16) | (mask & 0x7FF)

/*! MCAN Rx FIFO 0 extended Mask helper macro - low. */
#define MCAN_RX_FIFO0_EXT_MASK_LOW(match)       (1UL << 29) | ((match & 0x1FFFFFFF))
/* CAN Rx FIFO 0 extended Mask helper macro - high. */

#define MCAN_RX_FIFO0_EXT_MASK_HIGH(mask)       (2UL << 30) | ((mask & 0x1FFFFFFF))

/*  CAN Rx FIFO 1 Mask helper macro. */
#define MCAN_RX_FIFO1_STD_MASK(match, mask)      (2UL << 30) | (2UL << 27) | ((match & 0x7FF) << 16) | (mask & 0x7FF)

/* CAN Rx FIFO 1 extended Mask helper macro - low. */
#define MCAN_RX_FIFO1_EXT_MASK_LOW(match)        (2UL << 29) | ((match & 0x1FFFFFFF))
/* CAN Rx FIFO 1 extended Mask helper macro - high. */

#define MCAN_RX_FIFO1_EXT_MASK_HIGH(mask)        (2UL << 30) | ((mask & 0x1FFFFFFF))


/*---------------------------------------------------------------------------------------------------------*/
/* MCAN_TXBTIE constants definitions                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_TXBTIE_TIE0    0x00000001UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 0 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE1    0x00000002UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 1 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE2    0x00000004UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 2 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE3    0x00000008UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 3 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE4    0x00000010UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 4 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE5    0x00000020UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 5 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE6    0x00000040UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 6 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE7    0x00000080UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 7 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE8    0x00000100UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 8 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE9    0x00000200UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 9 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE10   0x00000400UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 10 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE11   0x00000800UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 11 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE12   0x00001000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 12 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE13   0x00002000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 13 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE14   0x00004000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 14 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE15   0x00008000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 15 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE16   0x00010000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 16 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE17   0x00020000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 17 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE18   0x00040000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 18 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE19   0x00080000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 19 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE20   0x00100000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 20 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE21   0x00200000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 21 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE22   0x00400000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 22 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE23   0x00800000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 23 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE24   0x01000000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 24 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE25   0x02000000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 25 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE26   0x04000000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 26 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE27   0x08000000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 27 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE28   0x10000000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 28 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE29   0x20000000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 29 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE30   0x40000000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 30 Enable   \hideinitializer */
#define MCAN_TXBTIE_TIE31   0x80000000UL           /*!< MCAN_TXBTIE setting to set Transmission Interrupt 31 Enable   \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  RWD constant definitions.                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_RWD_WDC(x) ((x) << MCAN_RWD_WDC_Pos)          /*!< RWD Setting for Watchdog Configuration. It could be 0~255 */

/*---------------------------------------------------------------------------------------------------------*/
/*  GFC constant definitions.                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_GFC_RRFE(x) ((x) << MCAN_GFC_RRFE_Pos)        /*!< GFC Setting for Reject Remote Frames Extended. It could be 0~1 */
#define MCAN_GFC_RRFS(x) ((x) << MCAN_GFC_RRFS_Pos)        /*!< GFC Setting for Reject Remote Frames Standard. It could be 0~1 */
#define MCAN_GFC_ANFE(x) ((x) << MCAN_GFC_ANFE_Pos)        /*!< GFC Setting for Accept Non-matching Frames Extended. It could be 0~3 */
#define MCAN_GFC_ANFS(x) ((x) << MCAN_GFC_ANFS_Pos)        /*!< GFC Setting for Accept Non-matching Frames Standard. It could be 0~3 */

/*---------------------------------------------------------------------------------------------------------*/
/*  SIDFC constant definitions.                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_SIDFC_FLSSA(x) ((x) << MCAN_SIDFC_FLSSA_Pos)             /*!< SIDFC Setting for Filter List Standard Start Address. It could be 0 ~ 0xFFFF */
#define MCAN_SIDFC_LSS(x)   ((x) << MCAN_SIDFC_LSS_Pos)              /*!< SIDFC Setting for List Size Standard. It could be 0 ~ 128 */


/*---------------------------------------------------------------------------------------------------------*/
/*  XIDFC constant definitions.                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_XIDFC_FLESA(x) ((x) << MCAN_XIDFC_FLESA_Pos)             /*!< XIDFC Setting for Filter List Extended Start Address. It could be 0 ~ 0xFFFF */
#define MCAN_XIDFC_LSE(x)   ((x) << MCAN_XIDFC_LSE_Pos)               /*!< XIDFC Setting for List Size Extended. It could be 0 ~ 64 */


/*---------------------------------------------------------------------------------------------------------*/
/*  XIDAM constant definitions.                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_XIDAM_EIDM(x) ((x) << MCAN_XIDAM_EIDM_Pos)             /*!< XIDAM Setting for Extended ID AND Mask. It could be 0 ~ 0x1FFFFFFF */

/*---------------------------------------------------------------------------------------------------------*/
/*  RXF0C constant definitions.                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_RXF0C_F0SA(x) ((x) << MCAN_RXF0C_F0SA_Pos)             /*!< RXF0C Setting for Rx FIFO 0 Start Address. It could be 0 ~ FFFF */
#define MCAN_RXF0C_F0S(x)  ((x) << MCAN_RXF0C_F0S_Pos)              /*!< RXF0C Setting for Rx FIFO 0 Size. It could be 0 ~ 64 */
#define MCAN_RXF0C_F0WM(x) ((x) << MCAN_RXF0C_F0WM_Pos)             /*!< RXF0C Setting for Rx FIFO 0 Watermark. It could be 0 ~ 64 */
#define MCAN_RXF0C_F0OM(x) ((x) << MCAN_RXF0C_F0OM_Pos)             /*!< RXF0C Setting for FIFO 0 Operation Mode. It could be 0 ~ 1 */
/*---------------------------------------------------------------------------------------------------------*/
/*  RXF1C constant definitions.                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_RXF1C_F1SA(x) ((x) << MCAN_RXF1C_F1SA_Pos)             /*!< RXF1C Setting for Rx FIFO 1 Start Address. It could be 0 ~ FFFF */
#define MCAN_RXF1C_F1S(x)  ((x) << MCAN_RXF1C_F1S_Pos)              /*!< RXF1C Setting for Rx FIFO 1 Size. It could be 0 ~ 64 */
#define MCAN_RXF1C_F1WM(x) ((x) << MCAN_RXF1C_F1WM_Pos)             /*!< RXF1C Setting for Rx FIFO 1 Watermark . It could be 0 ~ 64 */
#define MCAN_RXF1C_F1OM(x) ((x) << MCAN_RXF1C_F1OM_Pos)             /*!< RXF1C Setting for FIFO 1 Operation Mode. It could be 0 ~ 1 */
/*---------------------------------------------------------------------------------------------------------*/
/*  RXBC constant definitions.                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_RXBC_RBSA(x) ((x) << MCAN_RXBC_RBSA_Pos)             /*!< RXBC Setting for Rx Buffer Start Address. It could be 0 ~ FFFF */


/*---------------------------------------------------------------------------------------------------------*/
/*  RXESC constant definitions.                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_RXESC_F0DS(x) ((x) << MCAN_RXESC_F0DS_Pos)            /*!< RXESC Setting for Rx FIFO 0 Data Field Size. It could be 0 ~ 7 */
#define MCAN_RXESC_F1DS(x) ((x) << MCAN_RXESC_F1DS_Pos)            /*!< RXESC Setting for Rx FIF1 0 Data Field Size. It could be 0 ~ 7 */
#define MCAN_RXESC_RBDS(x) ((x) << MCAN_RXESC_RBDS_Pos)            /*!< RXESC Setting for Rx Buffer Data Field Size. It could be 0 ~ 7 */

/*---------------------------------------------------------------------------------------------------------*/
/*  TXBC constant definitions.                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_TXBC_TBSA(x) ((x) << MCAN_TXBC_TBSA_Pos)             /*!< TXBC Setting for Tx Buffers Start Address. It could be 0 ~ 0xFFFF */
#define MCAN_TXBC_NDTB(x) ((x) << MCAN_TXBC_NDTB_Pos)             /*!< TXBC Setting for Number of Dedicated Transmit Buffers. It could be 0 ~ 32 */
#define MCAN_TXBC_TFQS(x) ((x) << MCAN_TXBC_TFQS_Pos)             /*!< TXBC Setting for Transmit FIFO/Queue Size. It could be 0 ~ 32 */
#define MCAN_TXBC_TFQM(x) ((x) << MCAN_TXBC_TFQM_Pos)             /*!< TXBC Setting for Tx FIFO/Queue Mode. It could be 0 ~ 1 */

/*---------------------------------------------------------------------------------------------------------*/
/*  TXESC constant definitions.                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_TXESC_TBDS(x) ((x) << MCAN_TXBC_TBSA_Pos)             /*!< TXESC Setting for Tx Buffer Data Field Size. It could be 0 ~ 7 */


/*---------------------------------------------------------------------------------------------------------*/
/*  TXEFC constant definitions.                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MCAN_TXEFC_EFSA(x) ((x) << MCAN_TXEFC_EFSA_Pos)             /*!< TXEFC Setting for Event FIFO Start Address. It could be 0 ~ 0xFFFF */
#define MCAN_TXEFC_EFS(x)  ((x) << MCAN_TXEFC_EFS_Pos)              /*!< TXEFC Setting for Event FIFO Size. It could be 0 ~ 32 */
#define MCAN_TXEFC_EFWM(x) ((x) << MCAN_TXEFC_EFWM_Pos)             /*!< TXEFC Setting for Event FIFO Watermark. It could be 0 ~ 32 */

/* MCAN  Status. */
typedef enum _E_MCAN_STATUS
{
    eStatus_Fail,
    eStatus_Success,
    eStatus_MCAN_RxOverflow,
}E_MCAN_STATUS;


/* MCAN frame format. */
typedef enum _MCAN_FRAME_FORMAT
{
    eMCAN_FrameFormatStandard  = 0ul,   /*!< Standard frame format attribute. */
    eMCAN_FrameFormatExtend    = 1ul,   /*!< Extend frame format attribute. */
} E_MCAN_FRAME_FORMAT_T;

/* MCAN frame type. */
typedef enum _MCAN_FRAME_TYPE
{
    eMCAN_FrameTypeData   = 0,   /*!< Data frame type attribute. */
    eMCAN_FrameTypeRemote = 1,   /*!< Remote frame type attribute. */
} E_MCAN_FRAME_TYPE_T;

/* MCAN protocol type. */
typedef enum _MCAM_PROTO_TYPE
{
    eMCAN_ProtoTypeClassic = 0, /*!< Classic frame format attribute. */
#ifdef USE_CANFD
    eMCAN_ProtoTypeFD      = 1, /*!< FD frame format attribute. */
#endif
} E_MCAM_PROTO_TYPE_T;

/* MCAN bitrate mode type. */
typedef enum _MCAN_BITRATE_MODE_TYPE
{
    eMCAN_BitrateModeTypeNoSwitch = 0, /*!< No bitrate switch attribute. */
    eMCAN_BitrateModeTypeSwitch   = 1, /*!< Bitrate switch attribute. */
} E_MCAN_BITRATE_MODE_TYPE_T;

/* MCAN non-masking frames treatment. */
typedef enum _MCAN_NONMASKING_FRAME_CONFIG
{
    eMCAN_AcceptinFifo0 = 0x0U, /*!< Accept non-masking frames in Rx FIFO 0. */
    eMCAN_AcceptinFifo1 = 0x1U, /*!< Accept non-masking frames in Rx FIFO 1. */
    eMCAN_Reject0       = 0x2U, /*!< Reject non-masking frames. */
    eMCAN_Reject1       = 0x3U, /*!< Reject non-masking frames. */
} E_MCAN_NONMASKING_FRAME_CONFIG_T;

/* MCAN FIFO Operation Mode. */
typedef enum _MCAN_FIFO_OPMODE_CONFIG
{
    eMCAN_FifoBlocking  = 0x0U, /*!< FIFO blocking mode. */
    eMCAN_FifoOverwrite = 0x1U, /*!< FIFO overwrite mode. */
} E_MCAN_FIFO_OPMODE_CONFIG_T;

/* MCAN Tx FIFO/Queue Mode. */
typedef enum _MCAN_TX_MODE_CONFIG
{
    eMCAN_TxFifo  = 0x0U, /*!< Tx FIFO operation. */
    eMCAN_TxQueue = 0x1U, /*!< Tx Queue operation. */
} E_MCAN_TX_MODE_CONFIG_T;

/* MCAN frame type. */
typedef enum _MCAN_EVENT_FIFO_CONTROL
{
    eMCAN_NoStore       = 0x0U, /*!< Not store Tx events. */
    eMCAN_StoreTxEvent  = 0x1U, /*!< Store Tx events */
} MCAN_EVENT_FIFO_CONTROL_TYPE_T;


/* MCAN frame datafield size. */
typedef enum _MCAN_BYTES_IN_DATA_FIELD
{
    eMCAN_8ByteDatafield  = 0x0U, /*!< 8 byte data field. */
    eMCAN_12ByteDatafield = 0x1U, /*!< 12 byte data field. */
    eMCAN_16ByteDatafield = 0x2U, /*!< 16 byte data field. */
    eMCAN_20ByteDatafield = 0x3U, /*!< 20 byte data field. */
    eMCAN_24ByteDatafield = 0x4U, /*!< 24 byte data field. */
    eMCAN_32ByteDatafield = 0x5U, /*!< 32 byte data field. */
    eMCAN_48ByteDatafield = 0x6U, /*!< 48 byte data field. */
    eMCAN_64ByteDatafield = 0x7U, /*!< 64 byte data field. */
} E_MCAN_BYTES_IN_DATA_FIELD_T;


/*!
 * MCAN global filter flags
 * Defines what happens to messages that do not match any filter.
 * They can be placed into a FIFO or rejected
 */
typedef enum _MCAN_GLOBAL_FILTER_FLAGS
{
    eMCAN_GlobalFilter_Standard_FIFO0  = (0UL << 4), /*!< Put non-matching 11-bit msgs into FIFO0 */
    eMCAN_GlobalFilter_Standard_FIFO1  = (1UL << 4), /*!< Put non-matching 11-bit msgs into FIFO1 */
    eMCAN_GlobalFilter_Standard_Reject = (2UL << 4), /*!< Reject non-matching 11-bit msgs */
    eMCAN_GlobalFilter_Extended_FIFO0  = (0UL << 2), /*!< Put non-matching 29-bit msgs into FIFO0 */
    eMCAN_GlobalFilter_Extended_FIFO1  = (1UL << 2), /*!< Put non-matching 29-bit msgs into FIFO1 */
    eMCAN_GlobalFilter_Extended_Reject = (2UL << 2)  /*!< Reject non-matching 29-bit msgs */
}E_MCAN_GLOBAL_FILTER_FLAGS;


/** @addtogroup MCAN_EXPORTED_STRUCTS MCAN Exported Structs
  @{
*/


/**
  * @details    Layout of standard message filter.
  */
typedef struct {
  union
  {
      struct
      {
          uint32_t SFID2     : 11; /*!< Standard Filter ID 2. */
          uint32_t reserved1 : 5;
          uint32_t SFID1     : 11; /*!<Standard Filter ID 1. */
          uint32_t SFEC      : 3;  /*!<Standard Filter Element Configuration */
          uint32_t SFT       : 2;  /*!<Standard Filter Type */
      };
      struct
      {
          uint32_t VALUE; /*!< Access to filter as a word. */
      };
  };
} S_MCAN_STD_FILTER_T;

/**
  * @details    Layout of extended message filter.
  */
typedef struct {
  union
  {
      struct
      {
          uint32_t EFID1     : 29; /*!< Extended Filter ID 1. */
          uint32_t EFEC      : 3;  /*!< Extended Filter Element Configuration. */
          uint32_t EFID2     : 29; /*!< Extended Filter ID 2. */
          uint32_t reserved1 : 1;
          uint32_t EFT       : 2;  /*!< Extended Filter Type. */
      };
      struct
      {
          uint32_t LOWVALUE;  /*!< Access to filter low word. */
          uint32_t HIGHVALUE; /*!< Access to filter high word. */
      };
  };
} S_MCAN_EXT_FILTER_T;


/**
  * @details    Layout of transmit and receive message buffers..
  */
typedef struct {
    uint32_t u32Id;     /*!< Message identifier and associated flags. */
    uint32_t u32Config; /*!< Buffer configuration. */
    union {
        uint32_t u32Data[MCAN_MAX_MESSAGE_WORDS];  /*!< Word access to buffer data. */
        uint8_t  u8Data[MCAN_MAX_MESSAGE_BYTES];   /*!< Byte access to buffer data. */
    };
} S_MCAN_BUF_T;


/**
  * @details    Layout of message frame structure.
  */
typedef struct 
{
    struct
    {
        uint32_t length      : 7; /*!< CAN frame payload length in bytes (Range: 0~64) */
        uint32_t type        : 1; /*!< CAN frame type (DATA or REMOTE) */
        uint32_t format      : 1; /*!< CAN frame identifier (STD or EXT format) */
        uint32_t proto       : 1; /*!< CAN frame protocol (CLASSIC or FD) */
        uint32_t bitratemode : 1; /*!< CAN frame bitrate mode (NOSWITCH or SWITCH) */
        uint32_t reserve1    : 5;
        uint16_t timestamp;       /*!< Receive timestamp */
    };
    struct
    {
        uint32_t id       : 29;  /*!< CAN frame identifier, should be set using CAN_ID_EXT() or CAN_ID_STD() macros */
        uint32_t reserve2 : 3;   /*!< Reserved for place holder */
    };
    union
    {
        struct
        {
            uint32_t dataWord[MCAN_MAX_MESSAGE_WORDS]; /*!< CAN frame payload in words */
        };
        struct
        {
            uint8_t dataByte[MCAN_MAX_MESSAGE_BYTES];  /*!< CAN frame payload in bytes */
        };
    };
} S_MCAN_FRAME_T;


/**
  * @details    Layout of message buffer transfer structure.
  */
typedef struct _MCAN_MB_TRANSFER
{
    S_MCAN_FRAME_T *sFrame;       /*!< The buffer of CAN Message to be transfer. */
    uint8_t u8MbIdx;             /*!< The index of Message buffer used to transfer Message. */
} S_MCAN_MB_TRANSFER_T;

/**
  * @details    Layout of Rx FIFO transfer structure.
  */
typedef struct _MCAN_FIFO_TRANSFER
{
    S_MCAN_FRAME_T *sFrame;     /*!< The buffer of CAN Message to be received from Rx FIFO. */
    uint8_t u8FifoIdx;         /*!< Number of the FIFO. */
} S_MCAN_FIFO_TRANSFER_T;

/**
  * @details    Layout of Rx filter configuration structure.
  */
typedef struct _MCAN_FRAME_FILTER_CONFIG
{
    uint32_t u32Address;                       /*!< Filter start address. */
    uint32_t u32ListSize;                      /*!< Filter list size. */
    E_MCAN_FRAME_FORMAT_T eIdFormat;         /*!< Frame format. */
    E_MCAN_FRAME_TYPE_T    eRemFrame;         /*!< Remote frame treatment. */
    E_MCAN_NONMASKING_FRAME_CONFIG_T eNmFrame; /*!< Non-masking frame treatment. */
} S_MCAN_FRAME_FILTER_CONFIG_T;

/**
  * @details    Layout of Rx FIFO configuration structure.
  */
typedef struct _MCAN_RX_FIFO_CONFIG
{
    uint32_t u32Address;                          /*!< FIFOn start address. */
    uint32_t u32ElementSize;                      /*!< FIFOn element number. */
    uint32_t u32Watermark;                        /*!< FIFOn watermark level. */
    E_MCAN_FIFO_OPMODE_CONFIG_T eOpmode;          /*!< FIFOn blocking/overwrite mode. */
    E_MCAN_BYTES_IN_DATA_FIELD_T eDatafieldSize;   /*!< Data field size per frame, size>8 is for CANFD. */
} S_MCAN_RX_FIFO_CONFIG_T;

/**
  * @details    Layout of Rx Buffer configuration structure.
  */

typedef struct _MCAN_RX_BUFFER_CONFIG
{
    uint32_t u32Address;                        /*!< Rx Buffer start address. */
    E_MCAN_BYTES_IN_DATA_FIELD_T eDatafieldSize; /*!< Data field size per frame, size>8 is for CANFD. */
} S_MCAN_RX_BUFFER_CONFIG_T;


/**
  * @details    Layout of Tx Buffer configuration structure.
  */
typedef struct _MCAN_TX_BUFFER_CONFIG
{ 
    uint32_t u32Address;                          /*!< Tx Buffers Start Address. */
    uint32_t u32DedicatedSize;                    /*!< Number of Dedicated Transmit Buffers. */
    uint32_t u32FqSize;                           /*!< Transmit FIFO/Queue Size. */
    E_MCAN_TX_MODE_CONFIG_T eMode;                 /*!< Tx FIFO/Queue Mode.*/
    E_MCAN_BYTES_IN_DATA_FIELD_T eDatafieldSize;   /*!< Data field size per frame, size>8 is for CANFD. */
} S_MCAN_TX_BUFFER_CONFIG_T;


/**
  * @details    Layout of Tx Event FIFO configuration structure.
  */
typedef struct _MCAN_TX_FIFO_CONFIG
{
    uint32_t u32Address;      /*!< Event fifo start address. */
    uint32_t u32ElementSize;  /*!< FIFOn element number. */
    uint32_t u32Watermark;    /*!< FIFOn watermark level. */
} S_MCAN_TX_FIFO_CONFIG_T;

/**
  * @details    Layout of MCAN module configuration structure.
  */
typedef struct _MCAM_CONFIG
{
    uint32_t u32NominalBaudRate;             /*!< CAN nominal baud rate in bps. */
#ifdef USE_CANFD
    uint32_t u32DataBaudRate;                /*!< CAN nominal baud rate in bps. */
#endif
    uint32_t u32BaseAddress;                 /*!< RAM base address for buffers */
    uint32_t u32TimestampClock_Hz;           /*!< Frequency of timestamp clock in Hz, must be at least sysclock / 2048, zero to disable */
    uint8_t  u8RejectStandardRTR;           /*!< Set to TRUE to reject all 11-bit RTRs */
    uint8_t  u8RejectExtendedRTR;           /*!< Set to TRUE to reject all 29-bit RTRs */
    uint8_t  u8EnableLoopBack;              /*!< Set to TRUE to enable loopback mode. */
#ifdef USE_CANFD
    uint8_t  u8EnableNonISOMode;            /*!< Set to TRUE for CAN FD non-ISO mode. Only use for communication with 'old' FD controllers. */
    uint8_t  u8DisableFD;                   /*!< Set to TRUE to disable FD mode. */
#endif
} S_MCAM_CONFIG_T;

/*! @brief CAN protocol timing characteristic configuration structure. */
typedef struct _MCAN_TIMEING_CONFIG
{
    uint8_t  u8PreDivider;         /*!< Global Clock Division Factor. */
    uint16_t u16NominalPrescaler;  /*!< Nominal clock prescaler. */
    uint8_t  u8NominalRJumpwidth; /*!< Nominal Re-sync Jump Width. */
    uint8_t  u8NominalPhaseSeg1;  /*!< Nominal Phase Segment 1. */
    uint8_t  u8NominalPhaseSeg2;  /*!< Nominal Phase Segment 2. */
    uint8_t  u8NominalPropSeg;    /*!< Nominal Propagation Segment. */
#ifdef USE_CANFD
    uint8_t  u8DataPrescaler;     /*!< Data clock prescaler. */
    uint8_t  u8DataRJumpwidth;    /*!< Data Re-sync Jump Width. */
    uint8_t  u8DataPhaseSeg1;     /*!< Data Phase Segment 1. */
    uint8_t  u8DataPhaseSeg2;     /*!< Data Phase Segment 2. */
    uint8_t  u8DataPropSeg;       /*!< Data Propagation Segment. */
#endif
} S_MCAN_TIMEING_CONFIG_t;

/*@}*/ /* end of group MCAN_EXPORTED_STRUCTS */

/*! Type used for all status and error return values. */
typedef int32_t status_t;

uint32_t MCAN_GetRxFifo0ElementAddress(MCAN_T *mcan);
uint32_t MCAN_GetRxFifo1ElementAddress(MCAN_T *mcan);
uint32_t MCAN_GetRxBufferElementAddress(MCAN_T *mcan, uint8_t u8Idx);
uint32_t MCAN_GetTxBufferElementAddress(MCAN_T *mcan, uint8_t u8Idx);
uint32_t MCAN_GetStatusFlag(MCAN_T *mcan, uint32_t u32IntTypeFlag);
void MCAN_ClearStatusFlag(MCAN_T *mcan, uint32_t u32InterruptFlag);
void MCAN_SetFilterConfig(MCAN_T *mcan, const S_MCAN_FRAME_FILTER_CONFIG_T *config);
void MCAN_SetRxFifo0Config(MCAN_T *mcan, const S_MCAN_RX_FIFO_CONFIG_T *psConfig);
void MCAN_SetRxFifo1Config(MCAN_T *mcan, const S_MCAN_RX_FIFO_CONFIG_T *psConfig);
void MCAN_SetRxFifoConfig(MCAN_T *mcan, uint8_t fifoIdx,uint8_t u8RxFifoElementSize, uint8_t u8Enable);
void MCAN_SetRxBufferConfig(MCAN_T *mcan, const S_MCAN_RX_BUFFER_CONFIG_T *psConfig);
void MCAN_SetTxEventFifoConfig(MCAN_T *mcan, const S_MCAN_TX_FIFO_CONFIG_T *psConfig);
void MCAN_SetTxBufferConfig(MCAN_T *mcan, const S_MCAN_TX_BUFFER_CONFIG_T *psConfig);
void MCAN_EnableInt(MCAN_T *mcan,uint8_t u8Line, uint32_t u32IntSel);
void MCAN_DisableInt(MCAN_T *mcan,uint8_t u8Line, uint32_t u32IntSel);
void MCAN_EnableMbInterrupts(MCAN_T *mcan, uint32_t u32IntSel);
void MCAN_DisableMbInterrupts(MCAN_T *mcan, uint32_t u32IntSel);
void MCAN_GetBusErrCount(MCAN_T *mcan, uint8_t *pu8TxErrBuf, uint8_t *pu8RxErrBuf);
void MCAN_GetDefaultConfig(S_MCAM_CONFIG_T *psConfig);
void MCAN_SetTimingConfig(MCAN_T *mcan, const S_MCAN_TIMEING_CONFIG_t *psConfig);
void MCAN_Init(MCAN_T *mcan, const S_MCAM_CONFIG_T *psConfig, uint32_t sourceClock_Hz);
void MCAN_Enable(MCAN_T *mcan, uint8_t u8Enable);
void MCAN_AbortTxMb(MCAN_T *mcan, uint8_t u8MbIdx);
void MCAN_SetRxGlobalMask(MCAN_T *mcan, uint32_t u32Filter);
void MCAN_SetRxIndividualMask(MCAN_T *mcan, uint8_t u8MaskIdx, uint32_t u32Filter);
void MCAN_SetRxExtIndividualMask(MCAN_T *mcan, uint8_t u8MaskIdx, uint32_t u32FilterLow, uint32_t u32FilterHigh);
status_t MCAN_WriteTxMb(MCAN_T *mcan, uint8_t u8MbIdx, const S_MCAN_FRAME_T *psTxFrame);
status_t MCAN_TransferSendBlocking(MCAN_T *mcan, uint8_t u8MbIdx, S_MCAN_FRAME_T *psTxFrame);
status_t MCAN_ReadRxFifo(MCAN_T *mcan, uint8_t u8FifoIdx, S_MCAN_FRAME_T *psRxFrame);
status_t MCAN_TransferReceiveFifoBlocking(MCAN_T *mcan, uint8_t u8FifoIdx, S_MCAN_FRAME_T *psRxFrame);
status_t MCAN_ReadRxMb(MCAN_T *mcan, uint8_t u8MbIdx, S_MCAN_FRAME_T *psRxFrame);
status_t MCAN_TransferReceiveBlocking(MCAN_T *mcan, uint8_t mbIdx, S_MCAN_FRAME_T *psRxFrame);

/*@}*/ /* end of group MCAN_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group MCAN_Driver */

/*@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __MCAN_H__ */
