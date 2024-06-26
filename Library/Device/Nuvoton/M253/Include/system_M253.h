/**************************************************************************//**
 * @file     system_M253.h
 * @version  V0.10
 * @brief    System Setting Header File
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 ****************************************************************************/

#ifndef __SYSTEM_M253_H__
#define __SYSTEM_M253_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define __HXT       (12000000UL)              /*!< External Crystal Clock Frequency */
#define __LXT       (32768UL)                 /*!< External Crystal Clock Frequency 32.768KHz */
#define __LIRC      (38400UL)                 /*!< Internal 38.4KHz RC Oscillator Frequency */
#define __MIRC      (4032000UL)               /*!< Internal 4.032M RC Oscillator Frequency */
#define __HIRC      (48000000UL)              /*!< Internal 48M RC Oscillator Frequency */
#define __HSI       (48000000UL)              /*!< PLL default output is 48MHz */
#define __SYS_OSC_CLK     (    ___HSI)        /*!< Main oscillator frequency */

#define __SYSTEM_CLOCK    (1UL*__HXT)

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/


extern uint32_t SystemCoreClock;    /*!< System Clock Frequency (Core Clock)  */
extern uint32_t CyclesPerUs;        /*!< Cycles per micro second              */
extern uint32_t PllClock;           /*!< PLL Output Clock Frequency           */


/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the micro controller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit(void);

/**
 * Update SystemCoreClock variable
 *
 * @param  none
 * @return none
 *
 * @brief  Updates the SystemCoreClock with current core Clock
 *         retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate(void);

#ifndef NOT_USE_DBG_UART
/**
 * Set UART debug multi function pin
 *
 * @param  none
 * @return none
 *
 * @brief  The initialization of uart default multiple-function pin.
 */
extern void UartDebugMFP(void);

/**
 * Set UART debug clock
 *
 * @param    None
 * @return   None
 *
 * @brief  The initialization of debug uart clock source.
 */
extern void UartDebugCLK(void);


/**
 * @brief    UART Initial
 *
 * @param    None
 * @return   None
 *
 * @details  The initialization of debug uart.
 */
extern void UartDebugInit(void);

#endif /* NOT_USE_DBG_UART */

/**
 * Check if debug message finished
 *
 * @param    None
 *
 * @retval   1: Message is finished
 * @retval   0: Message is transmitting.
 *
 * @details  Check if message finished (FIFO empty of debug port)
 */
extern int IsDebugFifoEmpty(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_M253_H__ */
