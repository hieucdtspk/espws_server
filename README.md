**ESP8266 Websocket Server with multiple connection support**
==========
This is a lightweight websocket server lib. for ESP8266 - a tiny but powerful wifi SoC!
It's actually ported from many excellent references:
 * [cwebsocket](https://github.com/m8rge/cwebsocket)(mainly based on) - a lightweight websocket server library.
 * [Arduino-Websocket](https://github.com/brandenhall/Arduino-Websocket) - simple library that implements a Websocket client and server running on an Arduino.

**Features:**

 * Support multiple connection (from multiple websocket client)(can be configured).
 * Easy to setup and use

**Compile:**
Currently, I'm using a [full devkit for esp8266 in windows](http://www.esp8266.com/viewtopic.php?f=9&t=820)
Thanks so  much, it works perfect!
This devkit already including:
- Toolchain
- SDK
- Example (with eclipse project format)
- Eclipse project files, all required environment configuration and optimized makefile!

```bash
git clone https://github.com/hieucdtspk/espws_server
```

Just import existing eclipse project file after cloning source file!
And NOT forget to change your actual COM port that connected to esp8266 in makefile, or you can add 'COM' parameter in make target!

**Usage**
```c
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
```
**Limitation:**
- secure websocket (require SSL server side support from SDK)
- websocket client (TODO)
- websocket extensions
- websocket subprotocols
- status codes
- cookies and/or authentication-related header fields
- continuation frame (all payload data must be encapsulated into one websocket frame)
- big frames, which payload size bigger than 1024 (can be confifured)

**Status:** *Initial*

**Contributing:**

***Feel free to pull any request, any contribution from you is appreciated and that's my effort to improve my dream in coding!***

**Requried:**

SDK esp_iot_sdk_v0.9.4_14_12_19 or higher

**Authors:**
[HieuNT](hieucdtspk@gmail.com)


**LICENSE - "MIT License"**

The MIT License (MIT)

Copyright (c) 2015 HieuNT

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
