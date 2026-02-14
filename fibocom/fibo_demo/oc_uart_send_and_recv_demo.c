/**
 * @file oc_uart_send_and_recv_demo.c
 * @author Ethan Guo (Guo Junhuang) (ethan.guo@fibocom.com)
 * @brief Demo of uart testing sending and receiving, including:
 *          1. Callback mode: receiving data in callback receiving mode
 *              a. Receive and respone test: receive data from other mechine and respone to it with 
 *                  data has got.
 *              b. Self cricle test: by connecting its own TX and RX pins together, it will send 
 *                  data and receive the same data all by itself.
 * 
 *          2. Polling mode: receiving data in polling receiving mode
 *              a. Receive and respone test: same as above.
 *              b. Self cricle test: same as above.
 * @version 0.1
 * @date 2025-04-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "osi_api.h"
#include "osi_log.h"
#include "app_image.h"
#include "fibo_opencpu.h"


/**
 * @brief Test Option
 */
/* Main Option (Choose one of the two) */
#define _UART_CALLBACK_RECV_TEST    /* receiving data in callback receiving mode */
// #define _UART_POLLING_RECV_TEST     /* receiving data in polling receiving mode */

/* Test second Option (Choose one of the two) */
#define _UART_TEST_RECV_AND_RESPONE    /* receive and respone */
// #define _UART_TEST_SELF_CRICLE_TEST    /* self cricle test */


/**
 * @brief Hardware UARTs' numbers and their port number
 */
#define _UART1_PORT_NUM                 (0)
#define _UART2_PORT_NUM                 (1)
#define _UART3_PORT_NUM                 (2)
#define _UART4_PORT_NUM                 (3)
#define _UART5_PORT_NUM                 (4)
#define _UART6_PORT_NUM                 (5)


/**
 * @brief User uart testing configuration
 */
#define _TEST_UART_PORT                 _UART1_PORT_NUM
#define _TEST_UART_BAUD                 (115200)
#define _TEST_UART_DATA_BITS            HAL_UART_DATA_BITS_8
#define _TEST_UART_STOP_BITS            HAL_UART_STOP_BITS_1
#define _TEST_UART_PARITY               HAL_UART_NO_PARITY

#define _TEST_UART_RECV_BUFF_SIZE       (1024 * 5)
#define _TEST_UART_SEND_BUFF_SIZE       (1024 * 5)


#if defined(_UART_CALLBACK_RECV_TEST)
typedef struct {
    UINT8 *data;
    UINT16 len;
} uart_recv_msg_t;


UINT32 g_uart_recv_mq = 0;  // message queue for storing uart received data


static void _uart_recv_cb(hal_uart_port_t uart_port, UINT8 *data, UINT16 len, void *arg)
{
    uart_recv_msg_t recv_msg = {0};

    // fibo_textTrace("GJH: Uart port%d cb: recv data(len=%d):%s", uart_port, len, data);
    recv_msg.data = (UINT8 *)calloc(len + 1, 1);
    if (recv_msg.data == NULL) {
        fibo_textTrace("GJH: err: Uart port%d malloc recv_msg.data", uart_port);
        return;
    }
    memcpy(recv_msg.data, data, len);
    recv_msg.len = len;
    // put received data in message
    fibo_queue_put_isr(g_uart_recv_mq, &recv_msg);
}
#endif

static void sig_res_callback(GAPP_SIGNAL_ID_T sig, va_list arg)
{
    fibo_textTrace("GJH: sig_res_callback  sig = %d", sig);
}

static void at_res_callback(UINT8 *buf, UINT16 len)
{
    fibo_textTrace("GJH: FIBO <--%s", buf);
}

static FIBO_CALLBACK_T user_callback = {
    .fibo_signal = sig_res_callback,
    .at_resp = at_res_callback
};

static void hello_world_demo()
{
    int32_t ret = -1;

    fibo_textTrace("GJH: application thread enter");
    set_app_ver(app_ver);

    // configuarte uart pins
    #if defined(CONFIG_FIBOCOM_MC669) // MC669 ADP test
    #if ((_TEST_UART_PORT) == (_UART1_PORT_NUM))
    // pin31 --> UART1_RXD
    // pin32 --> UART1_TXD
    fibo_gpio_mode_set(31, 0);
    fibo_gpio_mode_set(32, 0);
    #elif ((_TEST_UART_PORT) == (_UART2_PORT_NUM))
    // pin123 --> UART2_RXD
    // pin124 --> UART2_TXD
    fibo_gpio_mode_set(123, 0);
    fibo_gpio_mode_set(124, 0);
    #elif ((_TEST_UART_PORT) == (_UART3_PORT_NUM))
    // pin51 --> UART3_RXD
    // pin50 --> UART3_TXD
    fibo_gpio_mode_set(50, 1);
    fibo_gpio_mode_set(51, 1);
    #endif
    #else
    // TODO: achived by customer...
    #endif

/*############################################################################################*/
// callback receiving test
/*############################################################################################*/
#if defined(_UART_CALLBACK_RECV_TEST)
    hal_uart_config_t uart_config = {
        .baud = _TEST_UART_BAUD,
        .data_bits = _TEST_UART_DATA_BITS,
        .stop_bits = _TEST_UART_STOP_BITS,
        .parity = _TEST_UART_PARITY,
        .cts_enable = false,
        .rts_enable = false,
        .rx_buf_size = UART_RX_BUF_SIZE, // set buffer size for driver layer
        .tx_buf_size = UART_TX_BUF_SIZE, // set buffer size for driver layer
        .recv_timeout = 0,   // timeout of callback mode, 0 as default value
    };
    UINT8 *send_buff = NULL;
    uart_recv_msg_t recv_msg = {0};

    g_uart_recv_mq = fibo_queue_create(10, sizeof(uart_recv_msg_t));
    if (g_uart_recv_mq == 0) {
        fibo_textTrace("GJH: err: fibo_queue_create");
        goto error;
    }

    send_buff = (UINT8 *)malloc(_TEST_UART_SEND_BUFF_SIZE);
    if (send_buff == NULL) {
        fibo_textTrace("GJH: err: malloc send_buff");
        goto error;
    }

    // uart init
    ret = fibo_hal_uart_init(_TEST_UART_PORT, 
                            &uart_config, 
                            _uart_recv_cb, // Set reveived callback to receive data in callback mode
                            NULL);
    if (ret != 0) {
        fibo_textTrace("GJH: err: fibo_hal_uart_init(port=%d)=%d", _TEST_UART_PORT, ret);
        goto error;
    } else {
        fibo_textTrace("GJH: ok: fibo_hal_uart_init(port=%d)=%d", _TEST_UART_PORT, ret);
    }

    /*******************************************************/
    // Callback receiving test: receive and respone test
    /*******************************************************/
    #if defined(_UART_TEST_RECV_AND_RESPONE)
    uint64_t cnt = 0;

    fibo_textTrace("GJH: Uart port%d is ready, do callback receiving test, which will receive data "
                    "from other mechine and respone to it with receiving data.", 
                    _TEST_UART_PORT);

    sprintf((char *)send_buff, 
            "Uart port%d is ready to receive data, and now you can send anything you want.", 
            _TEST_UART_PORT);
    fibo_hal_uart_put(_TEST_UART_PORT, send_buff, strlen((const char *)send_buff));

    for (;;) {
        memset(send_buff, 0x00, _TEST_UART_SEND_BUFF_SIZE);

        // blocking until message has stored received data
        fibo_queue_get(g_uart_recv_mq, &recv_msg, 0);
        if (recv_msg.len >= 0) {
            cnt++;
            fibo_textTrace("GJH: Uart port%d recv data(len=%d):%s", 
                            _TEST_UART_PORT, recv_msg.len, recv_msg.data);
            sprintf((char *)send_buff, 
                    "uart port%d has reveived data(cnt=%lld): %s", 
                    _TEST_UART_PORT, cnt, recv_msg.data);
            free(recv_msg.data);
            recv_msg.data = NULL;
            recv_msg.len = 0;
            fibo_hal_uart_put(_TEST_UART_PORT, send_buff, strlen((const char *)send_buff));
            fibo_textTrace("GJH: Uart port%d send_buff(len=%d):%s", 
                            _TEST_UART_PORT, strlen((const char *)send_buff), send_buff);
        }
    }
    #endif

    /*******************************************************/
    // Callback receiving test: self cricle test
    /*******************************************************/
    #if defined(_UART_TEST_SELF_CRICLE_TEST)
    fibo_textTrace("GJH: Uart port%d is ready, do self cricle test by connecting its own TX and RX "
                    "pins together, which will send data and receive the same data all by itself.", 
                    _TEST_UART_PORT);

    uint64_t cnt = 0;
    uint64_t err_cnt = 0;

    for (;;) {
        cnt++;
        memset(send_buff, 0x00, _TEST_UART_SEND_BUFF_SIZE);

        sprintf((char *)send_buff, "Uart port%d self cricle test: cnt=%lld", _TEST_UART_PORT, cnt);
        fibo_hal_uart_put(_TEST_UART_PORT, send_buff, strlen((const char *)send_buff));
        fibo_textTrace("GJH: Uart port%d send_buff(len=%d):%s", 
                        _TEST_UART_PORT, strlen((const char *)send_buff), send_buff);

        // blocking until message has stored received data
        fibo_queue_get(g_uart_recv_mq, &recv_msg, 0);
        if (recv_msg.len >= 0) {
            cnt++;
            fibo_textTrace("GJH: Uart port%d recv data(len=%d):%s", 
                            _TEST_UART_PORT, recv_msg.len, recv_msg.data);
            ret = strcmp((const char *)recv_msg.data, (const char *)send_buff);
            if (ret != 0) {
                err_cnt++;
            }
            free(recv_msg.data);
            recv_msg.data = NULL;
            recv_msg.len = 0;
        } else {
            err_cnt++;
        }
        fibo_textTrace("GJH: Uart port%d self cricle test:cnt=%lld, err_cnt=%lld", 
                        _TEST_UART_PORT, cnt, err_cnt);
        fibo_taskSleep(100);
    }
    #endif
#endif

/*############################################################################################*/
// Polling receiving test
/*############################################################################################*/
#if defined(_UART_POLLING_RECV_TEST)
    hal_uart_config_t uart_config = {
        .baud = _TEST_UART_BAUD,
        .data_bits = _TEST_UART_DATA_BITS,
        .stop_bits = _TEST_UART_STOP_BITS,
        .parity = _TEST_UART_PARITY,
        .cts_enable = false,
        .rts_enable = false,
        .rx_buf_size = UART_RX_BUF_SIZE, // set buffer size for driver layer
        .tx_buf_size = UART_TX_BUF_SIZE, // set buffer size for driver layer
    };
    UINT8 *recv_buff = NULL;
    UINT8 *send_buff = NULL;

    // uart init
    ret = fibo_hal_uart_init(_TEST_UART_PORT, 
                            &uart_config, 
                            NULL, // Set reveived callback as NULL to use read API in polling mode
                            NULL);
    if (ret != 0) {
        fibo_textTrace("GJH: err: fibo_hal_uart_init(port=%d)=%d", _TEST_UART_PORT, ret);
        goto error;
    } else {
        fibo_textTrace("GJH: ok: fibo_hal_uart_init(port=%d)=%d", _TEST_UART_PORT, ret);
    }

    recv_buff = (UINT8 *)malloc(_TEST_UART_RECV_BUFF_SIZE);
    if (recv_buff == NULL) {
        fibo_textTrace("GJH: err: malloc recv_buff");
        goto error;
    }

    send_buff = (UINT8 *)malloc(_TEST_UART_SEND_BUFF_SIZE);
    if (send_buff == NULL) {
        fibo_textTrace("GJH: err: malloc send_buff");
        goto error;
    }

    
    /*******************************************************/
    // Polling receiving test: receive and respone test
    /*******************************************************/
    #if defined(_UART_TEST_RECV_AND_RESPONE)
    uint64_t cnt = 0;

    fibo_textTrace("GJH: Uart port%d is ready, do Polling receiving test, which will receive data "
                    "from other mechine and respone to it with receiving data.", 
                    _TEST_UART_PORT);

    sprintf((char *)send_buff, 
            "Uart port%d is ready to receive data, and now you can send anything you want.", 
            _TEST_UART_PORT);
    fibo_hal_uart_put(_TEST_UART_PORT, send_buff, strlen((const char *)send_buff));

    for (;;) {
        memset(recv_buff, 0x00, _TEST_UART_RECV_BUFF_SIZE);
        memset(send_buff, 0x00, _TEST_UART_SEND_BUFF_SIZE);

        // Receive uart data, it will be blocked and not return until received data.
        ret = fibo_pi_read_uartdata(_TEST_UART_PORT, 
                                    recv_buff, 
                                    _TEST_UART_RECV_BUFF_SIZE, 
                                    (uint32_t)-1 /* wait forever to receive data */);
        fibo_textTrace("GJH: Uart port%d fibo_pi_read_uartdata=%d", _TEST_UART_PORT, ret);

        if (ret >= 0) {
            cnt++;
            fibo_textTrace("GJH: Uart port%d recv data(len=%d):%s", _TEST_UART_PORT, ret, recv_buff);
            sprintf((char *)send_buff, 
                    "uart port%d has reveived data(cnt=%lld): %s", 
                    _TEST_UART_PORT, cnt, recv_buff);
            fibo_hal_uart_put(_TEST_UART_PORT, send_buff, strlen((const char *)send_buff));
            fibo_textTrace("GJH: Uart port%d send_buff(len=%d):%s", 
                            _TEST_UART_PORT, strlen((const char *)send_buff), send_buff);
        }
        fibo_taskSleep(10);
    }
    #endif

    /*******************************************************/
    // Polling receiving test: self cricle test
    /*******************************************************/
    #if defined(_UART_TEST_SELF_CRICLE_TEST)
    fibo_textTrace("GJH: Uart port%d is ready, do self cricle test by connecting its own TX and RX "
                    "pins together, which will send data and receive the same data all by itself.", 
                    _TEST_UART_PORT);

    uint64_t cnt = 0;
    uint64_t err_cnt = 0;

    for (;;) {
        cnt++;
        memset(recv_buff, 0x00, _TEST_UART_RECV_BUFF_SIZE);
        memset(send_buff, 0x00, _TEST_UART_SEND_BUFF_SIZE);

        sprintf((char *)send_buff, "Uart port%d self cricle test: cnt=%lld", _TEST_UART_PORT, cnt);
        fibo_hal_uart_put(_TEST_UART_PORT, send_buff, strlen((const char *)send_buff));
        fibo_textTrace("GJH: Uart port%d send_buff(len=%d):%s", 
                        _TEST_UART_PORT, strlen((const char *)send_buff), send_buff);

        // Receive uart data, it will be blocked and not return until received data.
        ret = fibo_pi_read_uartdata(_TEST_UART_PORT, 
                                    recv_buff, 
                                    _TEST_UART_RECV_BUFF_SIZE, 
                                    (uint32_t)-1 /* wait forever to receive data */);
        fibo_textTrace("GJH: Uart port%d fibo_pi_read_uartdata=%d", _TEST_UART_PORT, ret);

        if (ret >= 0) {
            fibo_textTrace("GJH: Uart port%d recv data(len=%d):%s", _TEST_UART_PORT, ret, recv_buff);
            ret = strcmp((const char *)recv_buff, (const char *)send_buff);
            if (ret != 0) {
                err_cnt++;
            }
        } else {
            err_cnt++;
        }
        fibo_textTrace("GJH: Uart port%d self cricle test:cnt=%lld, err_cnt=%lld", 
                        _TEST_UART_PORT, cnt, err_cnt);
        fibo_taskSleep(100);
    }
    #endif
#endif

error:
    fibo_hal_uart_deinit(_TEST_UART_PORT);
    fibo_textTrace("GJH: mythread exit");
    fibo_thread_delete();
}


void *appimg_enter(void *param)
{
    fibo_textTrace("GJH: app image enter");
    fibo_thread_create(hello_world_demo, "mythread", 10*1024, NULL, OSI_PRIORITY_NORMAL);
    return (void *)&user_callback;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "GJH: application image exit");
}
