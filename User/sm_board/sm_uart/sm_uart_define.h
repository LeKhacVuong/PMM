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
	uint16_t m_fifo_head;
	uint16_t m_fifo_type;
	uint16_t m_fifo_size;
	uint8_t* m_rx_fifo;
	uint16_t m_baudrate;
	uint8_t m_priority;
}sm_uart_t;

#endif /* SM_BOARD_SM_UART_SM_UART_DEFINE_H_ */
