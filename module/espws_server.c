/*
 * espws_server.c
 * Source file for ESP8266 websocket server
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

#include "esp_common.h"
#include "debug.h"
#include "websocket.h"
#include "espws_server.h"

/* ---------------------------------------------------------------------------------
 * PRIVATE
 * --------------------------------------------------------------------------------- */
#if 0 /* Test ws header parsing */
const char header[] = "GET /ws HTTP/1.1\r\nHost: 10.86.85.83:3000\r\n\
Connection: Upgrade\r\n\
Pragma: no-cache\r\n\
Cache-Control: no-cache\r\n\
Upgrade: websocket\r\n\
Origin: null\r\n\
Sec-WebSocket-Version: 13\r\n\
User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 Safari/537.36\r\n\
Accept-Encoding: gzip, deflate, sdch\r\n\
Accept-Language: en-US,en;q=0.8\r\n\
Sec-WebSocket-Key: HiQhy2nHfmlfcQk1j+I68A==\r\n\
Sec-WebSocket-Extensions: permessage-deflate; client_max_window_bits\r\n\r\n";
void ICACHE_FLASH_ATTR parsingHeader_Test(void)
{
	struct handshake *hs = (struct handshake *)os_zalloc(sizeof(struct handshake));
	int frameType = 0;
	INFO("Parsing...\r\n");
	frameType = wsParseHandshake((uint8_t *)header, sizeof(header), hs);
	INFO("frameType: %d", frameType);
	INFO("Host: %s\n\rorigin: %s\n\rkey: %s\n\rresource: %s\n\r", hs->host?hs->host:"null", \
			hs->origin? hs->origin : "null", hs->key?hs->key:"null", hs->resource?hs->resource:"");
}
#endif

#if 0 /* Test SHA1 & base64 */
const char key1[] = "IsK2uuEpcevnS1aOMzlxUg==";
void ICACHE_FLASH_ATTR testEncode(void)
{
	INFO("Encode test:\n\r");

	char *responseKey = NULL;
	uint8_t length = strlen(key1)+strlen(secret);
	responseKey = (char *)malloc(length);
	memcpy(responseKey, key1, strlen(key1));
	memcpy(&(responseKey[strlen(key1)]), secret, strlen(secret));
	responseKey[length]=0;
	INFO("input len %d data:%s\n\r", length, responseKey);
	unsigned char shaHash[20];
	memset(shaHash, 0, sizeof(shaHash));
	sha1(shaHash, responseKey, length);

	int i;
	INFO("sha1:\n\r");
	for (i=0; i < sizeof(shaHash);i++){
		INFO("%02x", shaHash[i]);
	}
	INFO("\n\r");
	// size_t base64Length = base64(responseKey, length, shaHash, 20);
	size_t base64Length = base64_encode(responseKey, shaHash, 20);
	responseKey[base64Length] = '\0';
	INFO("output len %d data:%s\n\r", base64Length, responseKey);
}
#endif

// Connection pool
static EspWsConn espWsConn[MAX_CONN];

static EspWsCallback _connectedCb = NULL;
static EspWsCallback _disconnectedCb = NULL;
static EspWsDataCallback _dataCb = NULL;
static EspWsCallback _sentCb = NULL;

static void ICACHE_FLASH_ATTR espWsConnected_Cb(void *arg);
//void ICACHE_FLASH_ATTR espWsReconCb(void *arg, sint8 err);
void ICACHE_FLASH_ATTR espWsDisconCb(void *arg);
void ICACHE_FLASH_ATTR espWsRecvCb(void *arg, char *pdata, unsigned short len);
void ICACHE_FLASH_ATTR espWsSentCb(void *arg);

static void ICACHE_FLASH_ATTR espWsConnected_Cb(void *arg)
{
	struct espconn *conn = arg;
	int i;

	INFO("Client connected! %p\n\r", conn);

	// Notes: Here, we don't need allocate new conn for server!

	// Find empty conndata in pool
	for (i=0; i<MAX_CONN; i++) if (espWsConn[i].conn == NULL) break;
	INFO("Con req, conn=%p, pool slot %d\n", conn, i);
	if (i == MAX_CONN) {
		INFO("Aiee, conn pool overflow!\n");
		espconn_disconnect(conn);
		return;
	}

	espWsConn[i].conn=conn;
	espWsConn[i].handshake = (struct handshake *)os_zalloc(sizeof(struct handshake));
	espWsConn[i].handshake->frameType = WS_INCOMPLETE_FRAME;
	espWsConn[i].state = WS_STATE_OPENING;
	conn->reverse = &espWsConn[i];

	espconn_regist_recvcb(conn, espWsRecvCb);
//	espconn_regist_reconcb(conn, espWsReconCb);
	espconn_regist_disconcb(conn, espWsDisconCb);
	espconn_regist_sentcb(conn, espWsSentCb);
}

/*void ICACHE_FLASH_ATTR espWsReconCb(void *arg, sint8 err)
{
	// TODO: add attemp cnt
}*/

void ICACHE_FLASH_ATTR espWsDisconCb(void *arg)
{
	struct espconn *conn = (struct espconn *)arg;
	EspWsConn *wsConn = (EspWsConn *)conn->reverse;

	INFO("Client disconnected!\n\r");

	// Notes: Here, we don't need deallocate conn!
	// It may be done some where in the stack!

	wsConn->conn = NULL;
	if (wsConn->handshake) os_free(wsConn->handshake);
}

void ICACHE_FLASH_ATTR espWsRecvCb(void *arg, char *pdata, unsigned short len)
{
	struct espconn *conn = (struct espconn *)arg;
	EspWsConn *wsConn = (EspWsConn *)conn->reverse;

	enum wsFrameType frameType = WS_INCOMPLETE_FRAME;
	size_t frameSize;
	uint8_t *data = NULL;
	size_t dataSize = 0;
	if (len > MAX_BUF) goto parsing_bypass;

	INFO("State: %s\n\rData: %s\n\r", wsConn->state == WS_STATE_OPENING ? "Opening" : \
			wsConn->state == WS_STATE_NORMAL ? "Normal" : "Closing", pdata);

	if (wsConn->state == WS_STATE_OPENING) {
		frameType = wsParseHandshake(pdata, len, wsConn->handshake);
	}
	else {
		frameType = wsParseInputFrame(pdata, len, &data, &dataSize);
	}
	INFO("FrameType: %d\n\r", frameType);

	parsing_bypass:
	if ((frameType == WS_INCOMPLETE_FRAME && len > MAX_BUF) || frameType == WS_ERROR_FRAME){
	  if (frameType == WS_INCOMPLETE_FRAME)
		INFO(("Buffer overflow\n\r"));
	  else
		INFO(("Frame error\n\r"));

	  // TODO:
	 /* if (wsConn->state == WS_STATE_OPENING){
		frameSize = os_sprintf((char *)wsConn->buffer,
							("HTTP/1.1 400 Bad Request\r\n"
							"%s%s\r\n\r\n"),
							versionField,
							version);
		espconn_sent(conn, wsConn->buffer, frameSize);
		return;
	  }
	  else {
		wsMakeFrame(NULL, 0, wsConn->buffer, &frameSize, WS_CLOSING_FRAME);
		espconn_sent(conn, wsConn->buffer, frameSize);
//		wsConn->state = WS_STATE_CLOSING;
		wsConn->state = WS_STATE_OPENING;
		// TODO: check state after sending....
		return;
	  }*/
	}

	if (wsConn->state == WS_STATE_OPENING) {
	  assert(frameType == WS_OPENING_FRAME);
	  if (frameType == WS_OPENING_FRAME) {
		// if resource is right, generate answer handshake and send it
		// TODO: add more resource here! Currently, accept ANY resource!
		/*if (strcmp(wsConn->handshake->resource, "/test") != 0) {
			frameSize = os_sprintf((char *)wsConn->buffer, ("HTTP/1.1 404 Not Found\r\n\r\n"));
			espconn_sent(conn, wsConn->buffer, frameSize);
			INFO("Resource not found\n\r");
			return;
		}
		else {*/
		  	INFO("HandshakeInfo:\n\rhost:%s\n\rorigin:%s\n\rkey:%s\r\nresource:%s\n\r",\
		  			wsConn->handshake->host, wsConn->handshake->origin,\
					wsConn->handshake->key, wsConn->handshake->resource);
			wsGetHandshakeAnswer(wsConn->handshake, wsConn->buffer, &frameSize);
			//freeHandshake(wsConn->handshake);
			espconn_sent(conn, wsConn->buffer, frameSize);
			wsConn->state = WS_STATE_NORMAL;
			INFO("Handshaked\n\r");
			return;
		//}
	  }
	}
	else { // WS_STATE_NORMAL
		if (frameType == WS_CLOSING_FRAME) {
//		  if (wsConn->state == WS_STATE_CLOSING) {
//			return; // break;
//		  }
//		  else {
			wsMakeFrame(NULL, 0, wsConn->buffer, &frameSize, WS_CLOSING_FRAME);
			espconn_sent(conn, wsConn->buffer, frameSize);
			INFO("Closing frame\n\r");
			return; // break;
//		  }
		}
		else if (frameType == WS_TEXT_FRAME) {
		  os_memcpy(wsConn->buffer, data, dataSize);
		  wsConn->buffer[dataSize] = 0;
		  if (_dataCb) _dataCb(wsConn, wsConn->buffer, dataSize);
		}
		// TODO: more frame handle here!
	}

	//os_free(wsConn->buffer);
}

void ICACHE_FLASH_ATTR espWsSentCb(void *arg)
{
	struct espconn *conn = (struct espconn *)arg;
	EspWsConn *wsConn = (EspWsConn *)conn->reverse;

	INFO("Data sent to client!\n\r");
}


/* ---------------------------------------------------------------------------------
 * PUBLIC
 * --------------------------------------------------------------------------------- */
void ICACHE_FLASH_ATTR EspWsServer_Init(uint16_t port)
{
	int i;
	struct espconn *conn;

	for (i=0; i<MAX_CONN; i++){
		espWsConn[i].conn = NULL;
	}
	conn = (struct espconn*)os_zalloc(sizeof(struct espconn));
	conn->type=ESPCONN_TCP;
	conn->state=ESPCONN_NONE;
	conn->proto.tcp=(esp_tcp *)os_zalloc(sizeof(esp_tcp));
	conn->proto.tcp->local_port=port;
	espconn_regist_connectcb(conn, espWsConnected_Cb);
	espconn_accept(conn);
	INFO("Init listenConn %p\n\r", conn);
}

void ICACHE_FLASH_ATTR EspWsServer_OnConnected(EspWsCallback connectedCb)
{
	_connectedCb = connectedCb;
}

void ICACHE_FLASH_ATTR EspWsServer_OnDisconnected(EspWsCallback disconnectedCb)
{
	_disconnectedCb = disconnectedCb;
}

void ICACHE_FLASH_ATTR EspWsServer_OnData(EspWsDataCallback dataCb)
{
	_dataCb = dataCb;
}

void ICACHE_FLASH_ATTR EspWsServer_OnSent(EspWsCallback sentCb)
{
	_sentCb = sentCb;
}
