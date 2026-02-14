#ifndef __BT_OSI_H__
#define __BT_OSI_H__

#define OSI_MUTEX_MAX_TIMEOUT 0xffffffffUL
#define COS_WAIT_FOREVER 0xFFFFFFFF
#define COS_EVENT_PRI_NORMAL 0
#define OSI_CREATE_DEFAULT      0

#define TASK_QUEUE_MAX_COUNT    40

#define  pMutexCreate(initState) (0)
#define  pMutexFree(mutex) //(0)        // clear warning: statement with no effect
#define  pMutexLock(mutex) //(0)      // clear warning
#define  pMutexUnlock(mutex) //(0)   // clear warning

#define ARRAY_REV_TO_STREAM(p, a, len) do{register int ijk; for (ijk = 0; ijk < (len);        ijk++) *(p)++ = (UINT8) ((a)[(len) - 1 - (ijk)]) ;} while(0)
#define ARRAY_TO_STREAM(p, a, len) do{memcpy((p), (a), (len)); p = (UINT8 *)(p) + (len);} while(0)
#define ARRAY_TO_STREAM_WITH_PAD(p, a, len, plen) do{memcpy((p), (a), (len)); p = (UINT8 *)(p) + (len); if((plen) > 0){memset((p), 0, (plen)); p = (UINT8 *)(p) + (plen); } }while(0)


#define UINT32_TO_STREAM(p, u32) do{*(p)++ = (UINT8)(u32); *(p)++ = (UINT8)((u32) >> 8); *(p)++ = (UINT8)((u32) >> 16); *(p)++ = (UINT8)((u32) >> 24);} while(0)
#define UINT24_TO_STREAM(p, u24) do{*(p)++ = (UINT8)(u24); *(p)++ = (UINT8)((u24) >> 8); *(p)++ = (UINT8)((u24) >> 16);} while(0)
#define UINT16_TO_STREAM(p, u16) do{*(p)++ = (UINT8)(u16); *(p)++ = (UINT8)((u16) >> 8);} while(0)
#define UINT8_TO_STREAM(p, u8)   do{*(p)++ = (UINT8)(u8);} while(0)

#define STREAM_TO_UINT8(u8, p)   do{u8 = (UINT8)(*(p)); (p) += 1;} while(0)
#define STREAM_TO_UINT16(u16, p) do{u16 = ((UINT16)(*(p)) + (((UINT16)(*((p) + 1))) << 8)); (p) += 2;} while(0)
#define STREAM_TO_UINT24(u24, p) do{u24 = (((UINT32)(*(p))) + ((((UINT32)(*((p) + 1)))) << 8) + ((((UINT32)(*((p) + 2)))) << 16) ); (p) += 3;} while(0)
#define STREAM_TO_UINT32(u32, p) do{u32 = (((UINT32)(*(p))) + ((((UINT32)(*((p) + 1)))) << 8) + ((((UINT32)(*((p) + 2)))) << 16) + ((((UINT32)(*((p) + 3)))) << 24)); (p) += 4;} while(0)

#define STREAM_TO_ARRAY(a, p, len) do{memcpy(a, p, len); p = (UINT8 *)(p) + (len);}while(0)

#define STREAM_TO_UINT(out, p, len) \
    do \
    { \
        switch(len) \
        { \
        case 1: \
            STREAM_TO_UINT8(out, p); \
            break; \
        case 2: \
            STREAM_TO_UINT16(out, p); \
            break; \
        case 3: \
            STREAM_TO_UINT24(out, p); \
            break; \
        case 4: \
            STREAM_TO_UINT32(out, p); \
            break; \
        default:\
            break;\
        } \
    }while(0)

#define CONTAIN_OF_STRUCT(stru, mem) ((unsigned int)&(((stru *)0)->mem))
typedef struct _TASK_INFO_T
{
    void (*task_entry)(void *);
    void *args;
    const char *name;
    UINT16 stack_size;
    UINT8 priority;
} task_info_t;

typedef enum
{
    MOD_TYPE_APP,
    MOD_TYPE_BT,
    MOD_TYPE_BT_MESH,
    TOTAL_MODULES
} OSI_MOD_ID;

typedef struct _TASK_CONTEXT_T
{
    task_info_t task_info;
    OSI_MOD_ID    task_mod;
    void         *queue;
    void         *task_handle;
    BOOL          exclusive;
} task_context_t;

typedef void (*timer_cb) (void *args);
typedef void * timer_handle_t;
typedef void (*osi_timer_cb)( timer_handle_t xTimer );

typedef void * osi_mutex_t;
typedef void osi_thread_t;
typedef void (*osi_thread_entry_t)(void *argument);
typedef void osi_message_queue_t;

typedef struct
{
    timer_handle_t id;
    timer_cb cb;
    void *args;
    UINT32 type;
} osi_timer_t;


typedef struct
{
    UINT32 nEventId;
    UINT32 nParam1;
} osi_event_t;


typedef enum
{
    TIMER_MODE_SINGLE = 1,
    TIMER_MODE_PERIODIC,
} os_timer_type_t;

typedef struct
{
    BOOL (*osiThreadCreate)(task_context_t *p_task_ctx);
    void (*osiThreadDelete)(void *task_handle);
    osi_message_queue_t *(*osiQueueCreate)(UINT32 msg_count, UINT32 msg_size);
    void (*osiQueueDelete)(osi_message_queue_t *mq);
    UINT8 (*osiQueuePut)(osi_message_queue_t *mq, const void *msg, UINT32 timeout);
    UINT8 (*osiQueueGet)(osi_message_queue_t *mq, void *msg, UINT32 timeout);
    void (*osiThreadSleep)(UINT32 ms);

    void *(*osiMalloc)(UINT32 size);
    void (*osiFree)(void *ptr);

    timer_handle_t (*osiTimerCreate)(void * arg, char *name, UINT32 ms, os_timer_type_t type, osi_timer_cb cb);
    INT32 (*osiTimerStart)(timer_handle_t timer_id);
    INT32 (*osiTimerChange)(timer_handle_t timer_id, UINT32 ms);
    void (*osiTimerStop)(timer_handle_t timer_id);
    void (*osiTimerDelete)(timer_handle_t timer_id);

    osi_mutex_t (*osiMutexCreate)(void);
    INT32 (*osiMutexLock)(osi_mutex_t mutex, UINT32 timeout);
    void (*osiMutexUnlock)(osi_mutex_t mutex);
    void (*osiMutexDelete)(osi_mutex_t mutex);
    INT32 (*osiRand)(void);
}bt_stack_osi_interface_t;

void *osi_malloc(unsigned int size);
void *osi_calloc(unsigned int size);
void osi_free(void *ptr);
UINT32 osi_thread_create(
    osi_thread_entry_t entry,
    OSI_MOD_ID mod,
    void *args,
    UINT16 stack_size,
    UINT8 priority,
    UINT16 flags,
    const INT8 *name
);

int osi_timer_init(void);
void osi_timer_free(void);
osi_timer_t *osi_timer_create(os_timer_type_t type, unsigned int milli_seconds, timer_cb cb, void *args);
int osi_timer_cancel(osi_timer_t *timer);
int osi_timer_change(osi_timer_t *timer, unsigned int milli_seconds);
int osi_mutex_new(osi_mutex_t *mutex);
int osi_mutex_lock(osi_mutex_t *mutex);
int osi_mutex_unlock(osi_mutex_t *mutex);
int osi_mutex_free(osi_mutex_t *mutex);
BOOL osi_wait_event(OSI_MOD_ID mod, osi_event_t *event, UINT32 timeout);
BOOL osi_send_event(OSI_MOD_ID  mod, osi_event_t *event, UINT32 timeout, UINT16 option);
void osi_sleep(unsigned int milli_secondes);
int osi_init(void);
void osi_srand(unsigned int init);
int osi_rand(void);

/*** app layer used to register realize api of osi*/
void bt_stack_osi_interface_init(bt_stack_osi_interface_t *p_if);
void bt_stack_osi_interface_cleanup(void);
bt_stack_osi_interface_t *bt_stack_osi_interface_get(void);

#endif //__BT_OSI_H__


