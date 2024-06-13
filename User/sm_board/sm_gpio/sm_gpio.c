/*
 * sm_gpio.c
 *
 *  Created on: Jun 12, 2024
 *      Author: lekhacvuong
 */

#include "sm_gpio.h"
#include "stddef.h"
#include "stdlib.h"

typedef struct sm_gpio_impl{
	void* m_port;
	uint32_t m_pin;
	uint8_t m_mode;
}sm_gpio_impl_t;

#define impl(x) ((sm_gpio_impl_t*)(x))

sm_gpio_t* sm_gpio_create(void* _port, uint32_t _pin, uint8_t _mode){

	sm_gpio_impl_t* this = malloc(sizeof(sm_gpio_impl_t));
	if(!this)
		return NULL;

	this->m_port = _port;
	this->m_pin = _pin;
	this->m_mode = _mode;
	GPIO_SetMode(_port, _pin, _mode);

	return this;
}

int32_t sm_gpio_write(sm_gpio_t* _this, int8_t _value){
	sm_gpio_impl_t* this = impl(_this);
	if(!this)
		return -1;

	uint16_t port_index = 0;

	if(this->m_port == PA) port_index = 0;
	if(this->m_port == PB) port_index = 1;
	if(this->m_port == PC) port_index = 2;
	if(this->m_port == PD) port_index = 3;
	if(this->m_port == PE) port_index = 4;
	if(this->m_port == PF) port_index = 5;

	GPIO_PIN_DATA(port_index, this->m_pin) = _value;
	return 0;
}

int32_t sm_gpio_read(sm_gpio_t* _this){
	sm_gpio_impl_t* this = impl(_this);
	if(!this)
		return -1;

	uint16_t port_index = 0;

	if(this->m_port == PA) port_index = 0;
	if(this->m_port == PB) port_index = 1;
	if(this->m_port == PC) port_index = 2;
	if(this->m_port == PD) port_index = 3;
	if(this->m_port == PE) port_index = 4;
	if(this->m_port == PF) port_index = 5;

	return GPIO_PIN_DATA(port_index, this->m_pin);
}

int32_t sm_gpio_toggle(sm_gpio_t* _this){
	sm_gpio_impl_t* this = impl(_this);
	if(!this)
		return -1;

	sm_gpio_write(this, !sm_gpio_read(_this));
	return 0;
}

int32_t sm_gpio_destroy(sm_gpio_t* _this){
	sm_gpio_impl_t* this = impl(_this);
	if(!this)
		return -1;

	free(this);
	return 0;
}

