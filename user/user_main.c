/*
 * user_main.c --- main entry for esp8266 websocket server test!
 *
 *  Created on: Apr 8, 2015
 *      Author: HieuNT | hieucdtspk@gmail.com
 *
 *  Copyright (c) 2015 HieuNT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "esp_common.h"
#include "uart.h"
#include "led.h"
#include "wifi.h"
#include "espws_server.h"
#include "debug.h"

void ICACHE_FLASH_ATTR espWsServer_test_init(void);

void ICACHE_FLASH_ATTR userWifiConnectCb(uint8_t status)
{
	if(status == STATION_GOT_IP){
		INFO("Wifi connected!\n\r");
		espWsServer_test_init();
	} else {
		INFO("Wifi disconnected!\n\r");
	}
}

void ICACHE_FLASH_ATTR espWsServer_test_dataCb(EspWsConn *espWsConn, uint8_t *pdata, uint16_t len)
{
	INFO("wsServeronData!\n\r");
	INFO("Len: %d: %s\n\r", len, pdata);
}

void ICACHE_FLASH_ATTR espWsServer_test_init(void)
{
	EspWsServer_OnData(espWsServer_test_dataCb);
	EspWsServer_Init(8000);
}

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	os_delay_us(1000000);

	INFO("System started!\n\r");

	LED_init();
	WIFI_Connect("Your-wifi-ssid", "Your-wifi-password", userWifiConnectCb);
}
