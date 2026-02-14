#ifndef OPENCPU_API_OC_WS
#define OPENCPU_API_OC_WS

#include <stdbool.h>
#include <stdint.h>

#include "fibo_opencpu_comm.h"

typedef struct fibo_ws_header_s
{
    char* head_name;
    char* head_value;
}fibo_ws_header_t;

//2025-03-28 Add Start, qiaoyu, MTC1041-12, modify JIRA bug.
typedef void (*fibo_ws_data_t)(void *c, int32 datatype, bool fin, void *data, uint32 len, void *arg);
typedef void (*fibo_ws_close_t)(void *c, int32 reason, void *arg);
//2025-03-28 Add End, qiaoyu, MTC1041-12.

typedef struct fibo_ws_opt_s
{
    uint32 ping_interval;
    uint32 ping_timout;
    bool should_handshake;
    const int8 *origin;
    const int8 *key;
    uint32 timeout;
    fibo_ws_header_t* user_header;
    uint32 head_cnt;
} fibo_ws_opt_t;

void *fibo_ws_open(const int8 *url, fibo_ws_opt_t *opt, fibo_ws_data_t data_cb, fibo_ws_close_t close_cb, void *arg);

int32 fibo_ws_send_binary(void *ws_client, const void *data, uint32 size);

int32 fibo_ws_send_text(void *ws_client, const int8 *data, uint32 size);

int32 fibo_ws_close(void *ws_client);

#endif /* OPENCPU_API_OC_WS */
