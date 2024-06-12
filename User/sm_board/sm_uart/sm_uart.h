/*
 * sm_uart.h
 *
 *  Created on: Jun 12, 2024
 *      Author: lekhacvuong
 */

#ifndef SM_BOARD_SM_UART_SM_UART_H_
#define SM_BOARD_SM_UART_SM_UART_H_

#include "sm_uart_define.h"

sm_uart_t uart_debug = {.m_instance = UART0, .m_fifo_size = 0, .m_baudrate = 9600};

sm_uart_t uart_rs485_1 = {.m_instance = UART1, .m_fifo_size = 256, .m_baudrate = 9600};

sm_uart_t uart_rs485_2 = {.m_instance = UART2, .m_fifo_size = 256, .m_baudrate = 9600};

#endif /* SM_BOARD_SM_UART_SM_UART_H_ */
