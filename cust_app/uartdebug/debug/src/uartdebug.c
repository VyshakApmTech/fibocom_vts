/* Copyright (C) 2022 FIBOCOM Technologies Limited and/or its affiliates("FIBOCOM").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * FIBOCOM assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. FIBOCOM reserves the right to make changes in the
 * software without notification.  FIBOCOM also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */



#include "fibo_opencpu.h"
#include "oc_uart.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "uartdebug.h"


typedef enum
{
        UART1=0,
        UART2,
        UART3
}WEB_UART_PORT;
int WEBPORT = UART1;//0:uart1 MAIN UART   1:uart2
/*************************UART1 ********************/
int web_log_print(const char *file, int line, const char *fmt, ...) {  
    uint16_t count = 0;
    uint8_t UART_TxBuf[1024] = {0};
    va_list ap;

   

    // Add log level and file:line info
    count += snprintf((char *)&UART_TxBuf[count], sizeof(UART_TxBuf) - count, 
                      "\" %s:%d \" :::: ", file, line);

    // Add the user-provided message
    va_start(ap, fmt);
    count += vsnprintf((char *)&UART_TxBuf[count], sizeof(UART_TxBuf) - count, fmt, ap);
    va_end(ap);

    // Ensure buffer safety
    if (count > sizeof(UART_TxBuf)) {
        count = sizeof(UART_TxBuf);
    }

    // Send the log message via UART
    fibo_textTrace("application log [%s]",UART_TxBuf);
    return fibo_hal_uart_put(WEBPORT, UART_TxBuf, count);
}
//UART1 recv callback
static void web_uart_recv_callback(hal_uart_port_t uart_port, UINT8 *data, UINT16 len, void *arg)
{

     LOG_PRINTF("uartapi recv uart_port=%d len=%d, data=%s", uart_port, len, (char *)data);
    fibo_hal_uart_put(WEBPORT, (UINT8 *)data,(UINT32)len);
}
int web_uart_send_data(char *buff, int len)
{
    return fibo_hal_uart_put(WEBPORT, (UINT8 *)buff,(UINT32)len);
}
int web_uart_init(int baud)
{
    //int port = UART3;//0:uart1 MAIN UART   1:uart2
    hal_uart_config_t drvcfg = {0};
    drvcfg.baud = baud;
    drvcfg.parity = HAL_UART_NO_PARITY;
    drvcfg.data_bits = HAL_UART_DATA_BITS_8;
    drvcfg.stop_bits = HAL_UART_STOP_BITS_1;
    drvcfg.rx_buf_size = UART_RX_BUF_SIZE;
    drvcfg.tx_buf_size = UART_TX_BUF_SIZE;

   
    fibo_gpio_mode_set(50, 1);
    fibo_gpio_mode_set(51, 1);

   
   return fibo_hal_uart_init(WEBPORT, &drvcfg,web_uart_recv_callback, NULL);
    
}


