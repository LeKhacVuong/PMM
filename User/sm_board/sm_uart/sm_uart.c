/*
 * sm_uart.c
 *
 *  Created on: Jun 12, 2024
 *      Author: lekhacvuong
 */

#include "sm_uart.h"

#include <stdlib.h>

typedef struct sm_uart_impl{
	void* m_instance;
	uint16_t m_fifo_size;
	uint8_t* m_fifo;
	uint16_t m_fifo_head;
	uint16_t m_fifo_tail;
	uint16_t m_baudrate;
	uint8_t m_priority;
}sm_uart_impl_t;

#define impl(x) ((sm_uart_impl_t*)(x))

sm_uart_t* sm_uart_create(void* _instance, uint16_t _baudrate, uint16_t _fifo_size){
	sm_uart_impl_t* this = malloc(sizeof(sm_uart_impl_t));

	if(!this)
		return NULL;

	this->m_fifo = malloc(_fifo_size);

	if(!this->m_fifo){
		free(this);
		return NULL;
	}

	this->m_instance = _instance;
	this->m_baudrate = _baudrate;
	this->m_fifo_size = _fifo_size;

	UART_Open(_instance, _baudrate);

	return this;
}

int32_t sm_uart_enable_interrupt(sm_uart_t* _this, uint8_t _priority){
	sm_uart_impl_t* this = impl(_this);
	if(!this)
		return -1;

	if(this->m_instance == UART0){
		NVIC_SetPriority(UART0_IRQn, _priority);
		UART_EnableInt(UART0, UART_INTEN_RDAIEN_Msk);
		NVIC_EnableIRQ(UART0_IRQn);
	}else if(this->m_instance == UART1){
		NVIC_SetPriority(UART1_IRQn, _priority);
		UART_EnableInt(UART1, UART_INTEN_RDAIEN_Msk);
		NVIC_EnableIRQ(UART1_IRQn);

	}else if(this->m_instance == UART2){
		NVIC_SetPriority(UART2_IRQn, _priority);
		UART_EnableInt(UART2, UART_INTEN_RDAIEN_Msk);
		NVIC_EnableIRQ(UART2_IRQn);

	}else if(this->m_instance == UART3){
		NVIC_SetPriority(UART3_IRQn, _priority);
		UART_EnableInt(UART3, UART_INTEN_RDAIEN_Msk);
		NVIC_EnableIRQ(UART3_IRQn);

	}else if(this->m_instance == UART4){
		NVIC_SetPriority(UART4_IRQn, _priority);
		UART_EnableInt(UART4, UART_INTEN_RDAIEN_Msk);
		NVIC_EnableIRQ(UART4_IRQn);

	}else{
		return -1;
	}

	return 0;

}

int32_t sm_uart_disable_interrupt(sm_uart_t* _this, uint8_t _priority){
	sm_uart_impl_t* this = impl(_this);
	if(!this)
		return -1;

	if(this->m_instance == UART0){
		NVIC_DisableIRQ(UART0_IRQn);
	}else if(this->m_instance == UART1){
		NVIC_DisableIRQ(UART1_IRQn);

	}else if(this->m_instance == UART2){
		NVIC_DisableIRQ(UART2_IRQn);

	}else if(this->m_instance == UART3){
		NVIC_DisableIRQ(UART3_IRQn);

	}else if(this->m_instance == UART4){
		NVIC_DisableIRQ(UART4_IRQn);
	}else{
		return -1;
	}

	return 0;

}

int32_t sm_uart_destroy(sm_uart_t* _this){

	return 0;
}
