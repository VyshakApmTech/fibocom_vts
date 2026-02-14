
#ifndef __BT_LOG_H__
#define __BT_LOG_H__

#include <stdio.h>

//#define DEBUG_LOG

#define BT_TRACE_LEVEL_NONE     0
#define BT_TRACE_LEVEL_ERROR    1
#define BT_TRACE_LEVEL_WARNING  2
#define BT_TRACE_LEVEL_INFO     3
#define BT_TRACE_LEVEL_DEBUG    4


#define DEBUG_LOG_LEVEL (BT_TRACE_LEVEL_DEBUG)

#define BH_TRACE(tag, points, fmt, ...)
#define logpt_primary(...)
#define logpt_addr(...)
#define logpt_msg_mini(...)

void bt_assert_mask(unsigned int value);
void bt_assert();
void bt_stack_log_print(const char *fmt, ...);

extern char debug_buff[300];
typedef char *x_va_list;

#define BTE(...)                                            \
    do{                                                     \
        if (DEBUG_LOG_LEVEL >= BT_TRACE_LEVEL_ERROR)        \
        {                                                   \
             bt_stack_log_print(__VA_ARGS__);               \
        }                                                   \
    }while(0)

#define BTW(...)                                            \
    do{                                                     \
        if (DEBUG_LOG_LEVEL >= BT_TRACE_LEVEL_WARNING)      \
        {                                                   \
             bt_stack_log_print(__VA_ARGS__);               \
        }                                                   \
    }while(0)


#define BTI(...)                                            \
    do{                                                     \
        if (DEBUG_LOG_LEVEL >= BT_TRACE_LEVEL_INFO)         \
        {                                                   \
             bt_stack_log_print(__VA_ARGS__);               \
        }                                                   \
    }while(0)

#define BTD(...)                                            \
    do{                                                     \
        if (DEBUG_LOG_LEVEL >= BT_TRACE_LEVEL_DEBUG)        \
        {                                                   \
             sprintf(debug_buff, __VA_ARGS__);                           \
             bt_stack_log_print(debug_buff);                             \
        }                                                   \
    }while(0)                                               \

/** log interface function*/
typedef struct
{
    void (*output) (const char *fmt, ...);
    void (*cleanup) (void);
} bt_stack_log_interface_t;

void bt_stack_log_init(bt_stack_log_interface_t *log_if);
void bt_stack_log_close(void);
bt_stack_log_interface_t *bt_stack_log_get_interface(void);

#endif //__BT_LOG_H__

