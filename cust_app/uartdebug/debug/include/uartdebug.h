#ifndef _UARTDEBUG_H_
#define	_UARTDEBUG_H_

#include "fibo_opencpu.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include <stdint.h>
#include "string.h"



int web_uart_send_data(char *buff, int len);
static void web_uart_recv_callback(hal_uart_port_t uart_port, UINT8 *data, UINT16 len, void *arg);
int web_uart_init(int baud);
int web_log_print(const char *file, int line, const char *fmt, ...);
#define LOG_PRINTF(fmt, ...) web_log_print( __FILE__, __LINE__, fmt, ##__VA_ARGS__)





#endif