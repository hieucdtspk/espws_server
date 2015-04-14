/*
 * espws_server.h
 * Header file for ESP8266 websocket server
 *
 *  Created on: Apr 10, 2015
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

#ifndef MODULE_INCLUDE_ESPWS_SERVER_H_
#define MODULE_INCLUDE_ESPWS_SERVER_H_

// Max amount of connections
#define MAX_CONN 2
#define MAX_BUF 1024

typedef struct EspWsConn EspWsConn;
typedef struct EspWsSendData EspWsSendData;
typedef void (*EspWsCallback)(EspWsConn *espWsConn);
typedef void (*EspWsDataCallback)(EspWsConn *espWsConn, uint8_t *pdata, uint16_t len);

struct EspWsConn {
	struct espconn *conn;
	struct handshake *handshake;
	uint8_t state;
	uint8_t buffer[MAX_BUF];
};

/* --------------------------------------------------------------------
 * FUNCTIONS
 * -------------------------------------------------------------------- */
void ICACHE_FLASH_ATTR EspWsServer_Init(uint16_t port);


#endif /* MODULE_INCLUDE_ESPWS_SERVER_H_ */
