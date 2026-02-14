#ifndef __BT_HAL_H__
#define __BT_HAL_H__

#define VENDOR_CMD_MAX_COUNT 8

#define HCI_MARLIN_WRITE_PSKEY ((0x3f<<10)|0xa0)
#define HCI_MARLIN_MODULE_ENABLE ((0x3f<<10)|0xa1)
//#define HCI_MARLIN_WRITE_RF     ((0x3f<<10)|0xa2)
//#define HCI_MARLIN_ACL_PRIORITY ((0x3f<<10)|0x57)
//#define HCI_SPRD_SET_ACL_PROFILE ((0x3f<<10)|0x59)

/**
 * @enum bt_drv_state_t
 * @brief Enumeration with possible bt driver state.
 *
 */
typedef enum
{
    BT_DRV_STATE_OFF,
    BT_DRV_STATE_ON,
} bt_drv_state_t;

typedef struct
{
    UINT8 *data;
} hal_ctrl_package_t;

typedef struct controller_version_tag
{
    UINT8  day;
    UINT8  week;
    UINT16  year;
}controller_version_t;

typedef struct hci_vendor_report_evt_tag
{
    /// status
    UINT8  connhdl;
    /// rssi
    UINT8  rssi;
    controller_version_t  version;
    /// Time in 625 us step.
    UINT32 time;
    /// random bd_address
    bdaddr_t rand_add;
    /// public bd_address
    bdaddr_t public_add;
    UINT32   svn_version;
}hci_vendor_report_evt_t;

typedef struct _VENDOR_CMD_T
{
    UINT8 *cmd_buf;
    UINT8 cmd_len;
}vendor_cmd_t;

/** callback function*/
typedef struct
{
    void* (*malloc_cb) (int size);
    void (*free_cb) (void *p_buf);
    void (*recv_cb) (unsigned char type, unsigned char *data, unsigned int length);
    void (*state_changed_callback)(bt_drv_state_t state);
} bt_stack_hal_callback_t;

/** hal interface function*/
typedef struct
{
/**
* @brief  init
* @details init bt hal callback structure
* @param [in] p_cb bt hal callback structure
* @return none
*/
    int (*init) (bt_stack_hal_callback_t *p_cb);
/**
* @brief  send
* @details hci data send.
* @param [in] unsigned char type Type of send data
* @param [in] unsigned char *p_buf send data
* @param [in] unsigned int length Length of send data
* @return void
*/
    void (*send) (UINT8 type, void *p_buf, UINT32 length);
/**
* @brief  recv
* @details hci data receive.
* @param [in] unsigned char type Type of received data
* @param [in] unsigned char *p_buf Received data
* @param [in] unsigned int length Length of received data
* @return void
*/
    void (*recv)(UINT8 type, UINT8 *p_buf, UINT32 length);
/**
* @brief  check_vendor_event
* @details bluestack check vendor cmd and vendor event
* @param [in] hci_data_t received hci data
* @return UINT8 checkout result
*/
    UINT8 (*check_vendor_event)(hci_data_t *hci_data);

/**
* @brief  supply_vendor_cmd
* @details supply the vendor cmds table of controller init cmd
* @param [out] cmd table of controller init, the cmd sent befored reset cmd
* @return UINT8 count of cmd table
*/
    UINT8 (*supply_vendor_cmd_for_power_On)(vendor_cmd_t **cmd);

/**
* @brief  supply_vendor_cmd
* @details supply the vendor cmds table of controller init cmd
* @param [out] cmd table of controller init, the cmd sent befored reset cmd
* @return UINT8 count of cmd table
*/
    UINT8 (*supply_vendor_cmd_for_power_Off)(vendor_cmd_t **cmd);


/**
* @brief  clean_up
* @details bt hal cleanup the interface
* @param voied
* @return void
*/
    void (*clean_up) (void);
} bt_stack_hal_interface_t;

void bt_stack_hal_init(bt_stack_hal_interface_t *p_if);

bt_stack_hal_interface_t *bt_stack_hal_get_interface(void);

#endif //__BT_HAL_H__

