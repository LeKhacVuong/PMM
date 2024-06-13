/*
 * sm_uart_define.h
 *
 *  Created on: Jun 12, 2024
 *      Author: lekhacvuong
 */

#ifndef SM_BOARD_SM_UART_SM_UART_DEFINE_H_
#define SM_BOARD_SM_UART_SM_UART_DEFINE_H_

#include "NuMicro.h"
#include "stdint.h"

#define SM_UART_RX_BUFFER_SIZE 256

typedef struct sm_uart{
	void* m_instance;
	uint16_t m_fifo_size;
	uint16_t m_baudrate;
	uint8_t m_priority;
}uart_t;


uart_t uart_debug = {.m_instance = UART0, .m_fifo_size = 0, .m_baudrate = 9600};

uart_t uart_rs485_1 = {.m_instance = UART1, .m_fifo_size = 256, .m_baudrate = 9600};

uart_t uart_rs485_2 = {.m_instance = UART2, .m_fifo_size = 256, .m_baudrate = 9600};

#endif /* SM_BOARD_SM_UART_SM_UART_DEFINE_H_ */
