/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef _FIBO_OLD_API_H_
#define _FIBO_OLD_API_H_

#include <stdint.h>
#include <stdlib.h>


INT32  fibo_get_cfun_status();
INT32 fibo_get_operatorName(char *prsp);
UINT16 fibo_read_drxtm(CFW_SIM_ID nSimID);
void fibo_charger_set_notice_callbk(drvChargerNoticeCB_t cb);
UINT64 fibo_getSysTick_ext(void);
INT32 fibo_get_chipid(uint32_t *chipid,uint32_t *metaid);
INT32 fibo_file_getTotalSize(void);
INT32 fibo_get_heapinfo(uint32_t * size,uint32_t *avail_size,uint32_t *max_block_size);
bool  fibo_set_heartbeat_param(char *serverip, uint16_t port,  uint16_t time, char *hexdata);
INT32 fibo_get_module_info();



#endif
