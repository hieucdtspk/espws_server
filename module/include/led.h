/*
 * led.h
 *
 *  Created on: Apr 8, 2015
 *      Author: HieuNT | hieucdtspk@gmail.com
 */

#ifndef MODULES_INCLUDE_LED_H_
#define MODULES_INCLUDE_LED_H_

#define LED_GPIO 2
#define LED_GPIO_MUX PERIPHS_IO_MUX_GPIO2_U
#define LED_GPIO_FUNC FUNC_GPIO2

#define LED_DELAY 100 /* milliseconds */

void ICACHE_FLASH_ATTR LED_Init(void);

#endif /* MODULES_INCLUDE_LED_H_ */
