/*
 * debug.h
 *
 *  Created on: Apr 8, 2015
 *      Author: HieuNT | hieucdtspk@gmail.com
 */

#ifndef INCLUDE_DEBUG_H_
#define INCLUDE_DEBUG_H_

extern int ets_uart_printf(const char *fmt, ...);

#define INFO ets_uart_printf


#endif /* INCLUDE_DEBUG_H_ */
