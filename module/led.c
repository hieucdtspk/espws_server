/*
 * led.c
 *
 *  Created on: Apr 8, 2015
 *  Author: HieuNT | hieucdtspk@gmail.com
 */

#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>

#include <gpio.h>
#include "led.h"

// extern void wdt_feed (void);

LOCAL os_timer_t led_timer;
LOCAL uint8_t led_state=0;

LOCAL void ICACHE_FLASH_ATTR led_cb(void *arg)
{
	GPIO_OUTPUT_SET(LED_GPIO, led_state);
	led_state ^=1;
	// wdt_feed();
}

void ICACHE_FLASH_ATTR LED_init(void)
{
	// Configure pin as a GPIO
	PIN_FUNC_SELECT(LED_GPIO_MUX, LED_GPIO_FUNC);
	// Set up a timer to blink the LED
	// os_timer_disarm(ETSTimer *ptimer)
	os_timer_disarm(&led_timer);
	// os_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *parg)
	os_timer_setfn(&led_timer, (os_timer_func_t *)led_cb, (void *)0);
	// void os_timer_arm(ETSTimer *ptimer,uint32_t milliseconds, bool repeat_flag)
	os_timer_arm(&led_timer, LED_DELAY, 1);
}





