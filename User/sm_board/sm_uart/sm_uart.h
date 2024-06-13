/*
 * sm_uart.h
 *
 *  Created on: Jun 12, 2024
 *      Author: lekhacvuong
 */

#ifndef SM_BOARD_SM_UART_SM_UART_H_
#define SM_BOARD_SM_UART_SM_UART_H_

#include "sm_uart_define.h"

typedef void sm_uart_t;

sm_uart_t* sm_uart_create(void* _instance, uint16_t _baudrate, uint16_t _fifo_size);

int32_t sm_uart_enable_interrupt(sm_uart_t* _this, uint8_t _priority);

int32_t sm_uart_disable_interrupt(sm_uart_t* _this, uint8_t _priority);

int32_t sm_uart_destroy(sm_uart_t* _this);

#endif /* SM_BOARD_SM_UART_SM_UART_H_ */
