/*
 * sm_gpio_define.h
 *
 *  Created on: Jun 12, 2024
 *      Author: lekhacvuong
 */

#ifndef SM_BOARD_SM_GPIO_SM_GPIO_DEFINE_H_
#define SM_BOARD_SM_GPIO_SM_GPIO_DEFINE_H_

#include "stdint.h"
#include "NuMicro.h"

typedef struct gpio{
	void* m_port;
	uint32_t m_pin;
	uint8_t m_mode;
}gpio_t;

// GPIO output

static gpio_t io_ctrl_rclk     = {.m_port = PC, .m_pin = BIT14, .m_mode = GPIO_MODE_OUTPUT};

static gpio_t io_oe_n          = {.m_port = PB, .m_pin = BIT15, .m_mode = GPIO_MODE_OUTPUT};

static gpio_t io_srclk         = {.m_port = PB, .m_pin = BIT14, .m_mode = GPIO_MODE_OUTPUT};

static gpio_t io_srclr_n       = {.m_port = PB, .m_pin = BIT13, .m_mode = GPIO_MODE_OUTPUT};

static gpio_t io_ser           = {.m_port = PB, .m_pin = BIT12, .m_mode = GPIO_MODE_OUTPUT};


static gpio_t io_rs485_en_1    = {.m_port = PA, .m_pin = BIT5, .m_mode = GPIO_MODE_OUTPUT};

static gpio_t io_rs485_en_2    = {.m_port = PC, .m_pin = BIT2, .m_mode = GPIO_MODE_OUTPUT};

static gpio_t io_can_mode      = {.m_port = PC, .m_pin = BIT3, .m_mode = GPIO_MODE_OUTPUT};


static gpio_t io_led_test      = {.m_port = PF, .m_pin = BIT15, .m_mode = GPIO_MODE_OUTPUT};


static gpio_t io_ctrl_rl_bss   = {.m_port = PA, .m_pin = BIT0, .m_mode = GPIO_MODE_OUTPUT};

static gpio_t io_ctrl_rl_fan   = {.m_port = PA, .m_pin = BIT1, .m_mode = GPIO_MODE_OUTPUT};

static gpio_t io_ctrl_rl_chr   = {.m_port = PA, .m_pin = BIT4, .m_mode = GPIO_MODE_OUTPUT};


static gpio_t io_charger_on    = {.m_port = PA, .m_pin = BIT11, .m_mode = GPIO_MODE_OUTPUT};


static gpio_t io_status_ac_dc  = {.m_port = PB, .m_pin = BIT6, .m_mode = GPIO_MODE_INPUT};

static gpio_t io_status_ac     = {.m_port = PB, .m_pin = BIT4, .m_mode = GPIO_MODE_INPUT};


static gpio_t io_sens_slv_s    = {.m_port = PB, .m_pin = BIT5, .m_mode = GPIO_MODE_INPUT};

static gpio_t io_sens_rsv_s    = {.m_port = PB, .m_pin = BIT3, .m_mode = GPIO_MODE_INPUT};

static gpio_t io_sens_cam_s    = {.m_port = PB, .m_pin = BIT2, .m_mode = GPIO_MODE_INPUT};

static gpio_t io_sens_mst_s    = {.m_port = PB, .m_pin = BIT1, .m_mode = GPIO_MODE_INPUT};

static gpio_t io_sens_led_s    = {.m_port = PB, .m_pin = BIT0, .m_mode = GPIO_MODE_INPUT};




#endif /* SM_BOARD_SM_GPIO_SM_GPIO_DEFINE_H_ */
