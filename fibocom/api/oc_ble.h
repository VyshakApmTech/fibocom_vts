#ifndef FFW_API_OC_BLE
#define FFW_API_OC_BLE

#include <stdint.h>
#include <stdbool.h>
#include "fibo_opencpu_comm.h"

typedef enum {
    FIBO_BLE_SET_NAME = 0,
    FIBO_BLE_READ_NAME
}fibo_ble_name_opt_e;

typedef struct
{
    char *data;
    int data_len;
    char *pkg_path;
}fibo_ble_upgrade_t;

typedef INT32 (*ble_data_recv_callback) (int data_len,char *data);

//ble basic and service function
int32_t fibo_ble_power(void);
INT32 fibo_ble_onoff(uint8_t onoff);
INT32 fibo_ble_get_ver(char *ver);
INT32 fibo_ble_read_addr(uint8_t addr_type,char *addr);
INT32 fibo_ble_set_read_name(uint8_t set_read,uint8_t *name,uint8_t name_type);
INT32 fibo_ble_get_connect_state(int *state);
INT32 fibo_ble_set_dev_param(int param_count,int AdvMin,int AdvMax,int AdvType,int OwnAddrType,int DirectAddrType,int AdvChannMap,int AdvFilter,char *DirectAddr);
INT32 fibo_ble_set_dev_data(int data_len,char *data);
INT32 fibo_ble_enable_dev(int onoff);
INT32 fibo_ble_send_data(int data_len,char *data);
INT32 fibo_ble_set_read_mtu(uint8_t set_read, int *mtu);
INT32 fibo_ble_get_peer_mtu(int *mtu);
INT32 fibo_ble_set_scan_response_data(int data_len,char *data);
INT32 fibo_ble_recv_data(ble_data_recv_callback callback);
INT32 fibo_ble_data_request(int data_len,void *data, int timeout);
INT32 fibo_ble_gtsend_data(int index, int len, void* data);
INT32 fibo_ble_set_gtuuid(int index, void *uuid, UINT8 attr);
INT32 fibo_ble_response_to_read(int len, void* data);
INT32 fibo_ble_handle(INT8 *data, UINT32 len);
int32 fibo_ble_upgrade(int8 type, fibo_ble_upgrade_t *info);
INT32 fibo_ble_ota(UINT8 type, UINT8 *url,UINT8 *filename,UINT8 *username,UINT8 *password);
#endif
