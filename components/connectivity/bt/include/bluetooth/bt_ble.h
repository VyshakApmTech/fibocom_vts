/**
 * @file    bt_ble.h
 * @brief
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2021-1-29
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */

#ifndef __BT_BLE_H__
#define __BT_BLE_H__

//GAP AD type from Generic Access Profile of Assigned Numbers
#define GAP_ADTYPE_FLAGS 0x01                     /**< Discovery Mode: @ref GAP_ADTYPE_FLAGS_MODES  */
#define GAP_ADTYPE_16BIT_MORE 0x02                /**< Service: More 16-bit UUIDs available  */
#define GAP_ADTYPE_16BIT_COMPLETE 0x03            /**< Service: Complete list of 16-bit UUIDs  */
#define GAP_ADTYPE_32BIT_MORE 0x04                /**< Service: More 32-bit UUIDs available  */
#define GAP_ADTYPE_32BIT_COMPLETE 0x05            /**< Service: Complete list of 32-bit UUIDs  */
#define GAP_ADTYPE_128BIT_MORE 0x06               /**< Service: More 128-bit UUIDs available  */
#define GAP_ADTYPE_128BIT_COMPLETE 0x07           /**< Service: Complete list of 128-bit UUIDs  */
#define GAP_ADTYPE_LOCAL_NAME_SHORT 0x08          /**< Shortened local name  */
#define GAP_ADTYPE_LOCAL_NAME_COMPLETE 0x09       /**< Complete local name  */
#define GAP_ADTYPE_POWER_LEVEL 0x0A               /**< TX Power Level: 0xXX: -127 to +127 dBm  */
#define GAP_ADTYPE_OOB_CLASS_OF_DEVICE 0x0D       /**< Simple Pairing OOB Tag: Class of device (3 octets)  */
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC 0x0E  /**< Simple Pairing OOB Tag: Simple Pairing Hash C (16 octets)  */
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR 0x0F  /**< Simple Pairing OOB Tag: Simple Pairing Randomizer R (16 octets)  */
#define GAP_ADTYPE_SM_TK 0x10                     /**< Security Manager TK Value  */
#define GAP_ADTYPE_SM_OOB_FLAG 0x11               /**< Secutiry Manager OOB Flags   */
#define GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE 0x12  /**< Min and Max values of the connection interval (2 octets Min, 2 octets Max) (0xFFFF indicates no conn interval min or max)  */
#define GAP_ADTYPE_SIGNED_DATA 0x13               /**< Signed Data field  */
#define GAP_ADTYPE_SERVICES_LIST_16BIT 0x14       /**< Service Solicitation: list of 16-bit Service UUIDs  */
#define GAP_ADTYPE_SERVICES_LIST_128BIT 0x15      /**< Service Solicitation: list of 128-bit Service UUIDs  */
#define GAP_ADTYPE_SERVICE_DATA 0x16              /**< Service Data  */
#define GAP_ADTYPE_APPEARANCE 0x19                /**< Appearance  */
#define GAP_ADTYPE_ADVERTISING_INTERVAL  0x1A     /**<Advertising Interval?  */
#define GAP_ADTYPE_LE_BLUETOOTH_DEVICE_ADDRESS 0x1B  /**<LE Bluetooth Device Address?  */
#define GAP_ADTYPE_LE_ROLE 0x1C                      /**<LE Role?  */
#define GAP_ADTYPE_MANUFACTURER_SPECIFIC 0xFF      /**< Manufacturer Specific Data: first 2 octets contain the Company Identifier Code followed by the additional manufacturer specific data  */

/**@brief attribute length*/
#define CHAR_SHORT_UUID_LEN            (2)
#define CHAR_LONG_UUID_LEN            (16)
#define CHAR_EXTENDED_PROP_LEN        (2)
#define CHAR_CLIENT_CFG_LEN            (2)
#define CHAR_SERVER_CFG_LEN            (2)
#define CHAR_FORMAT_LEN                (7)

/**@brief le device state*/
#define LE_DEVICE_FLAG_UNUSED            (0x0)
#define LE_DEVICE_FLAG_USED              (0x100)
#define LE_DEVICE_FLAG_NEWDEVICE         (0x200)
#define LE_DEVICE_FLAG_RESP                (0x400)

/**@brief attribute opcode*/
#define ATT_ERROR_RSP                   0x01
#define ATT_EXCHANGE_MTU_REQ            0x02
#define ATT_EXCHANGE_MTU_RSP            0x03
#define ATT_FIND_INFORMATION_REQ        0x04
#define ATT_FIND_INFORMATION_RSP        0x05
#define ATT_FIND_BY_TYPE_VALUE_REQ      0x06
#define ATT_FIND_BY_TYPE_VALUE_RSP      0x07
#define ATT_READ_BY_TYPE_REQ            0x08
#define ATT_READ_BY_TYPE_RSP            0x09
#define ATT_READ_REQ                    0x0A
#define ATT_READ_RSP                    0x0B
#define ATT_READ_BLOB_REQ               0x0C
#define ATT_READ_BLOB_RSP               0x0D
#define ATT_READ_MULTIPLE_REQ           0x0E
#define ATT_READ_MULTIPLE_RSP           0x0F
#define ATT_READ_BY_GROUP_TYPE_REQ      0x10
#define ATT_READ_BY_GROUP_TYPE_RSP      0x11
#define ATT_WRITE_REQ                   0x12
#define ATT_WRITE_RSP                   0x13
#define ATT_WRITE_CMD                   0x52
#define ATT_SIGNED_WRITE_CMD            0xD2
#define ATT_PREPARE_WRITE_REQ           0x16
#define ATT_PREPARE_WRITE_RSP           0x17
#define ATT_EXECUTE_WRITE_REQ           0x18
#define ATT_EXECUTE_WRITE_RSP           0x19
#define ATT_HANDLE_VALUE_NOTIFICATION   0x1B
#define ATT_HANDLE_VALUE_INDICATION     0x1D
#define ATT_HANDLE_VALUE_CONFIRMATION   0x1E

/**@brief attribute error codes*/
#define ATT_ERR_INVALID_HANDLE          0x01
#define ATT_ERR_READ_NOT_PERMITTED      0x02
#define ATT_ERR_WRITE_NOT_PERMITTED     0x03
#define ATT_ERR_INVALID_PDU             0x04
#define ATT_ERR_INSUFFICIENT_AUTHEN     0x05
#define ATT_ERR_REQUEST_NOT_SUPPORT     0x06
#define ATT_ERR_INVALID_OFFSET          0x07
#define ATT_ERR_INSUFFICIENT_AUTHOR     0x08
#define ATT_ERR_PREPARE_QUEUE_FULL      0x09
#define ATT_ERR_ATTRIBUTE_NOT_FOUND     0x0A
#define ATT_ERR_ATTRIBUTE_NOT_LONG      0x0B
#define ATT_ERR_INSUFFICIENT_EK_SIZE    0x0C
#define ATT_ERR_INVALID_ATTRI_VALUE_LEN 0x0D
#define ATT_ERR_UNLIKELY_ERROR          0x0E
#define ATT_ERR_INSUFFICIENT_ENCRYPTION 0x0F
#define ATT_ERR_UNSUPPORTED_GROUP_TYPE  0x10
#define ATT_ERR_INSUFFICIENT_RESOURCES  0x11
#define ATT_ERR_APPLICATION_ERROR       0x80

/**@brief attribute properties*/
#define ATT_CHARA_PROP_BROADCAST    0x01
#define ATT_CHARA_PROP_READ         0x02
#define ATT_CHARA_PROP_WWP          0x04    // WWP short for "write without response"
#define ATT_CHARA_PROP_WRITE        0x08
#define ATT_CHARA_PROP_NOTIFY       0x10
#define ATT_CHARA_PROP_INDICATE     0x20
#define ATT_CHARA_PROP_ASW          0x40    // ASW short for "Authenticated signed write"
#define ATT_CHARA_PROP_EX_PROP      0x80

/**@brief attribute format*/
#define ATT_FMT_INITAL_LEN(x)   ((x<<22)&0xff800000)
#define ATT_FMT_SHORT_UUID      0x00000001
#define ATT_FMT_FIXED_LENGTH    0x00000002
#define ATT_FMT_WRITE_NOTIFY    0x00000004
#define ATT_FMT_GROUPED         0x00000008
#define ATT_FMT_CONFIRM_NOTITY  0x00000010
#define ATT_FMT_APPEND_WRITE    0x00000020
#define ATT_FMT_LONG_UUID          0x00000040

/**@brief attribute permissions*/
#define ATT_PM_READABLE                    0x0001
#define ATT_PM_WRITEABLE                   0x0002
#define ATT_PM_R_AUTHENT_REQUIRED          0x0004
#define ATT_PM_R_AUTHORIZE_REQUIRED        0x0008
#define ATT_PM_R_ENCRYPTION_REQUIRED       0x0010
#define ATT_PM_R_AUTHENT_MITM_REQUERED     0x0020
#define ATT_PM_W_AUTHENT_REQUIRED          0x0040
#define ATT_PM_W_AUTHORIZE_REQUIRED        0x0080
#define ATT_PM_W_ENCRYPTION_REQUIRED       0x0100
#define ATT_PM_W_AUTHENT_MITM_REQUERED     0x0200
#define ATT_PM_BR_ACCESS_ONLY              0x0400

/**@brief attribute uuid*/
#define ATT_UUID_PRIMARY            0x2800
#define ATT_UUID_SECONDARY          0x2801
#define ATT_UUID_INCLUDE            0x2802
#define ATT_UUID_CHAR               0x2803

//GATT primary service UUID
#define ATT_UUID_GAP                0x1800   //Generic Access
#define ATT_UUID_GATT               0x1801   //Generic Attribute
#define ATT_UUID_IAS                0x1802   //Immediate Alert Service
#define ATT_UUID_LLS                0x1803   //Link Loss Service
#define ATT_UUID_TPS                0x1804   //Tx Power Service
#define ATT_UUID_CTS                0x1805   //Current Tim??e Service
#define ATT_UUID_RTUS               0x1806   //Reference Time Update Service
#define ATT_UUID_NDCS               0x1807   //Next DST Change Service
#define ATT_UUID_GLS                0x1808   //Glucose Service
#define ATT_UUID_HTS                0x1809   //Health Thermometer Service
#define ATT_UUID_DIS                0x180A   //Device Information Service
#define ATT_UUID_HRS                0x180D   //Heart Rate Service
#define ATT_UUID_PASS               0x180E   //Phone Alert Status Service
#define ATT_UUID_BAS                0x180F   //Battery Service
#define ATT_UUID_BLS                0x1810   //Blood Pressure Service
#define ATT_UUID_ANS                0x1811   //Alert Notification Service
#define ATT_UUID_HIDS               0x1812   //HID Service
#define ATT_UUID_SCPS               0x1813   //Scan Parameters Service
#define ATT_UUID_RSCS               0x1814   //Running Speed and Cadence Service
#define ATT_UUID_AIOS               0x1815   //Automation IO Service
#define ATT_UUID_CSCS               0x1816   //Cycling Speed and Cadence Service
#define ATT_UUID_CPS                0x1818   //Cycling Power Service
#define ATT_UUID_LNS                0x1819   //Location and Navigation Service
#define ATT_UUID_ESS                0x181A   //Environmental Sensing Service
#define ATT_UUID_BCS                0x181B   //Body Composition Service
#define ATT_UUID_UDS                0x181C   //User Data Service
#define ATT_UUID_WSS                0x181D   //Weight Scale Service
#define ATT_UUID_BMS                0x181E   //Bond Management Service
#define ATT_UUID_CGMS               0x181F   //Continuous Glucose Monitoring Service
#define ATT_UUID_IPSS               0x1820   //Internet Protocol Support Service
#define ATT_UUID_IPS                0x1821   //Indoor Positioning Service
#define ATT_UUID_PLXS               0x1822   //Pulse Oximeter Service

//GATT Characteristic Types
#define ATT_UUID_DEVICENAME                     0x2A00
#define ATT_UUID_APPEARANCE                     0x2A01
#define ATT_UUID_PRIVACY_FLAG                   0x2A02
#define ATT_UUID_RECONN_ADDR                    0x2A03
#define ATT_UUID_CONN_PARAMETER                 0x2A04
#define ATT_UUID_SERVICE_CHANGE                 0x2A05
#define ATT_UUID_ALERT_LEVEL                    0x2A06
#define ATT_UUID_CENTRAL_ADDRESS_RESOLUTIION    0x2AA6
#define ATT_UUID_RESOLVABLE_PRIVATE_ADDRESS     0x2AC9

#define ATT_UUID_GLUCOSE_MEASUREMENT              0x2A18
#define ATT_UUID_BATTERY_LEVEL                    0x2A19
#define ATT_UUID_GLUCOSE_MEASUREMENT_CONTEXT      0x2A34
#define ATT_UUID_GLUCOSE_FEATURE                  0x2A51
#define ATT_UUID_RECORD_ACCESS_CONTROL_POINT      0x2A52
#define ATT_UUID_CURRENT_TIME_CHAR                0x2A2B
#define ATT_UUID_LOCAL_TIME_INFO_CHAR             0x2A0F
#define ATT_UUID_REFERENCE_TIME_INFO_CHAR         0x2A14

#define ATT_UUID_CHAR_EXT           0x2900
#define ATT_UUID_CHAR_USER          0x2901
#define ATT_UUID_CLIENT             0x2902
#define ATT_UUID_SERVER             0x2903
#define ATT_UUID_CHAR_FORMAT        0x2904
#define ATT_UUID_CHAR_AGGREGATE     0x2905
#define ATT_UUID_EXTERNAL_REF       0x2907
#define ATT_UUID_REPORT_RE          0x2908

/**@brief smp pair mode*/
//#define SMP_JUST_WORK           0x00
//#define SMP_PASSKEY             0x01
//#define SMP_NUMBER_COMPARISON   0x02
//#define SMP_OUT_OF_BAND         0x03

/**@brief smp io capabilities*/
#define SMP_DISPLAY_ONLY    0x00
#define SMP_DISPLAY_YESNO   0x01
#define SMP_KEYBOARD_ONLY   0x02
#define SMP_NOIN_NOOUT      0x03
#define SMP_KEYBOARD_DISP   0x04

#define LE_KEY_MAGIC    (0x20160623)

/**@brief acl connection role*/
#define LE_MASTER       1
#define LE_SLAVE        0

/**@brief ble address type*/
#define BLE_ADDR_TYPE_PUBLIC    0
#define BLE_ADDR_TYPE_RANDOM    1

/**@brief define primary service with short uuid*/
#define DEF_PRIMARY_SERVICE_S(_value, r_cb)         \
{                                                    \
    .length = CHAR_SHORT_UUID_LEN,                    \
    .permisssion = ATT_PM_READABLE,                    \
    .uuid = {ATT_UUID_PRIMARY},                        \
    .fmt = ATT_FMT_SHORT_UUID|ATT_FMT_GROUPED,        \
    .attValue = _value,                                \
    .read_cb = r_cb,                                \
    .cb = NULL,                                         \
}                                                    \

/**@brief define primary service with long uuid*/
#define DEF_PRIMARY_SERVICE_L(_value, r_cb)         \
{                                                    \
    .length = CHAR_LONG_UUID_LEN,                    \
    .permisssion = ATT_PM_READABLE,                    \
    .uuid = {ATT_UUID_PRIMARY},                        \
    .fmt = ATT_FMT_GROUPED,        \
    .attValue = _value,                                \
    .read_cb = r_cb,                                \
    .cb = NULL,                                         \
}                                                  \

/**@brief define characteristic with short uuid*/
#define DEF_CHAR_S(_value, r_cb)                     \
{                                                    \
    .length = sizeof(gatt_chara_def_short_t),        \
    .permisssion = ATT_PM_READABLE,                    \
    .uuid = {ATT_UUID_CHAR},                        \
    .fmt = ATT_FMT_SHORT_UUID|ATT_FMT_FIXED_LENGTH,    \
    .attValue = _value,                                \
    .read_cb = r_cb,                                \
    .cb = NULL,                                         \
}                                                    \

/**@brief define characteristic with long uuid*/
#define DEF_CHAR_L(_value, r_cb)                     \
{                                                    \
    .length = sizeof(gatt_chara_value_long_t),                                   \
    .permisssion = ATT_PM_READABLE,                    \
    .uuid = {ATT_UUID_CHAR},                        \
    .fmt = ATT_FMT_SHORT_UUID|ATT_FMT_FIXED_LENGTH,    \
    .attValue = _value,                                \
    .read_cb = r_cb,                                \
    .cb = NULL,                                         \
}                                                    \

/**@brief define characteristic value with short uuid*/
#define DEF_CHAR_VALUE_S(_len,_permission, _uuid, _value, r_cb, w_cb) \
{                                                    \
    .length = _len,                                    \
    .permisssion = _permission,                        \
    .uuid = {_uuid},                                     \
    .fmt = ATT_FMT_SHORT_UUID|ATT_FMT_FIXED_LENGTH,    \
    .attValue = _value,                                \
    .read_cb = r_cb,                                \
    .cb = w_cb,                                        \
}                                                    \

/**@brief define characteristic value with long uuid*/
#define DEF_CHAR_VALUE_L(_len,_permission, _uuid, _value, r_cb, w_cb) \
{                                                    \
    .length = _len,                                    \
    .permisssion = _permission,                        \
    .uuid = {.uuid_l = _uuid},                        \
    .fmt = ATT_FMT_FIXED_LENGTH,    \
    .attValue = _value,                                \
    .read_cb = r_cb,                                \
    .cb = w_cb,                                         \
}                                                    \

/**@brief define characteristic extended properties*/
#define DEF_CHAR_EXTENDED_PROP(_value, r_cb)          \
{                                                    \
    .length= CHAR_EXTENDED_PROP_LEN,                \
    .permisssion = ATT_PM_READABLE,                     \
    .uuid = {ATT_UUID_CHAR_EXT},                    \
    .fmt = ATT_FMT_SHORT_UUID|ATT_FMT_FIXED_LENGTH,    \
    .attValue = _value,                                \
    .read_cb = r_cb,                                \
    .cb = NULL,                                         \
}                                                    \

/**@brief define characteristic user description*/
#define DEF_CHAR_USER_DESC(_len, _permission ,_value, r_cb, w_cb)          \
{                                                    \
    .length= _len,                                    \
    .permisssion = _permission,                         \
    .uuid = {ATT_UUID_CHAR_USER},                    \
    .fmt = ATT_FMT_FIXED_LENGTH,                     \
    .attValue = _value,                                \
    .read_cb = r_cb,                                \
    .cb = w_cb,                                         \
}                                                    \

/**@brief define client characteristic configuration*/
#define DEF_CHAR_CLIENT_CFG(_value, r_cb, w_cb)      \
{                                                    \
    .length= CHAR_CLIENT_CFG_LEN,                    \
    .permisssion = ATT_PM_READABLE|ATT_PM_WRITEABLE,\
    .uuid = {ATT_UUID_CLIENT},                        \
    .fmt = ATT_FMT_SHORT_UUID|ATT_FMT_FIXED_LENGTH,    \
    .attValue = _value,                                \
    .read_cb = r_cb,                                \
    .cb = w_cb,                                         \
}                                                    \

/**@brief define server characteristic configuration*/
#define DEF_CHAR_SERVER_CFG(_value, r_cb, w_cb)      \
{                                                    \
    .length= CHAR_SERVER_CFG_LEN,                    \
    .permisssion = ATT_PM_READABLE|ATT_PM_WRITEABLE,\
    .uuid = {ATT_UUID_SERVER},                        \
    .fmt = ATT_FMT_SHORT_UUID|ATT_FMT_FIXED_LENGTH,    \
    .attValue = _value,                                \
    .read_cb = r_cb,                                \
    .cb = w_cb,                                         \
}                                                    \

/**@brief define characteristic presentation format*/
#define DEF_CHAR_PRESENTATION_FORMAT(_value, r_cb)     \
{                                                    \
    .length= CHAR_FORMAT_LEN,                        \
    .permisssion = ATT_PM_READABLE,                     \
    .uuid = {ATT_UUID_CHAR_FORMAT},                    \
    .fmt = ATT_FMT_SHORT_UUID|ATT_FMT_FIXED_LENGTH,    \
    .attValue = _value,                                \
    .read_cb = r_cb,                                \
    .cb = NULL,                                         \
}                                                    \

#define DEF_CHAR_REPORT_REFERENCE(_value, r_cb)     \
{                                                    \
    .length= CHAR_FORMAT_LEN,                        \
    .permisssion = ATT_PM_READABLE,                     \
    .uuid = {ATT_UUID_REPORT_RE},                    \
    .fmt = ATT_FMT_SHORT_UUID|ATT_FMT_FIXED_LENGTH,    \
    .attValue = _value,                                \
    .read_cb = r_cb,                                \
    .cb = NULL,                                         \
}                                                    \
/**@brief define max attribute length*/
#define ATT_MAX_LEN                    (512)

/**@brief define attribute uuid format*/
#define ATT_FORMAT_SHORT_UUID            (1)
#define ATT_FORMAT_LONG_UUID            (2)

/**
 * @enum ble_gatt_req_code_t
 * @brief Enumeration gatt request code.
*/
typedef enum
{
    BLE_GATT_EXCHANGE_MTU,                        /**< gatt exchange mtu */
    BLE_GATT_DISCOVER_ALL_PRIMARY_SERVICE,        /**< gatt discover all primary service */
    BLE_GATT_DISCOVER_PRIMARY_SERVICE_BY_UUID,    /**< gatt discover primary service by uuid */
    BLE_GATT_FIND_INCLUDE_SERVICE,                /**< gatt find include service */
    BLE_GATT_DISCOVER_ALL_CHAR,                    /**< gatt discover all characteristics */
    BLE_GATT_DISCOVER_CHAR_BY_UUID,                /**< gatt discover all characteristic by uuid */
    BLE_GATT_DISCOVER_ALL_CHAR_DESC,            /**< gatt discover all characteristic description */
    BLE_GATT_READ_CHAR,                            /**< gatt read characteristic */
    BLE_GATT_READ_CHAR_BY_UUID,                    /**< gatt read characteristic by uuid */
    BLE_GATT_READ_LONG_CHAR,                    /**< gatt read characteristic long */
    BLE_GATT_READ_CHAR_MULTIPUL,                /**< gatt read characteristic multipul */
    BLE_GATT_WRITE_CHAR_WITHOUT_RSP,            /**< gatt write characteristic without response */
    BLE_GATT_SIGNED_WRITE_CHAR_WITHOUT_RSP,        /**< gatt signed write characteristic without response */
    BLE_GATT_WRITE_CHAR,                        /**< gatt write characteristic */
    BLE_GATT_WRITE_CHAR_LONG,                    /**< gatt write long characteristic values */
    BLE_GATT_WRITE_CHAR_RELIABLE,                /**< gatt write characteristic reliable */
    BLE_GATT_WRITE_EXECUTE,                        /**< gatt write characteristic execute */
    BLE_GATT_MAX_REQ_CODE,                        /**< gatt max code */
} ble_gatt_req_code_t;

/**@brief attribute type*/







/**
 * @union ble_gatt_uuid_t
 * @brief uuid.
*/
typedef union
{
    UINT16  uuid_s;            /**< short uuid*/
    UINT8  *uuid_l;            /**< pointer of long uuid*/
}ble_gatt_uuid_t;

/**
 * @struct ble_gatt_discover_t
 * @brief discover params of gatt.
*/
typedef struct
{
    ble_gatt_uuid_t uuid;    /**< uuid*/
    UINT16 type;            /**< type of uuid*/
    UINT16 start_handle;    /**< start attribute handle*/
    UINT16 end_handle;        /**< end attribute handle*/
}ble_gatt_discover_t;

/**
 * @struct ble_gatt_rw_t
 * @brief read and write params of gatt.
*/
typedef struct
{
    ble_gatt_uuid_t uuid;    /**< uuid*/
    UINT16 att_handle;        /**< attribute value handle*/
    UINT16 len;                /**< read or write length*/
    UINT8 *data;            /**< pointer of read or write data*/
}ble_gatt_rw_t;

/**
 * @struct ble_scan_param_t
 * @brief ble scan params.
*/
typedef struct
{
    UINT8 type;                /**< scan type*/
    UINT16 interval;        /**< scan interval*/
    UINT16 window;            /**< scan window*/
    UINT8 filter;            /**< scan filter*/
    UINT8 addr_type;        /**< scan address type*/
}ble_scan_param_t;

/**
 * @struct ble_scan_param_t
 * @brief ble scan params.
*/
typedef struct
{
    UINT16 adv_min;            /**< minimum adv interval*/
    UINT16 adv_max;            /**< maxmum adv interval*/
    UINT8 adv_type;            /**< adv type*/
    UINT8 own_type;            /**< own address type*/
    UINT8 peer_type;        /**< peer address type*/
    bdaddr_t peer_addr;        /**< peer address*/
    UINT8 map;                /**< adv channel map*/
    UINT8 filter;            /**< adv filter*/
}ble_adv_param_t;

/**
 * @struct ble_adv_report_t
 * @brief adv report data structure used to save the adv report and scan response.
*/
typedef struct
{
    UINT8 event_type;     /**< indicate the adv event or scan response*/
    UINT8 addr_type;
    bdaddr_t addr;
    UINT8 data_length;   /**<  adv data length */
    UINT8 name_length;
    UINT8 name[MGR_MAX_REMOTE_NAME_LEN];
    INT8 rssi;
} ble_adv_report_t;

typedef enum
{
    PAIR_STATE_INIT=0x00,
    PASSKEY_ENTRY_FAILED,
    OOB_NOT_AVAILABLE,
    AUTHENTICATION_REQ,
    CONFIRM_VALUE_FAILED,
    PAIRING_NOT_SUPPORT,
    ENCRYPTION_KEY_SIZE,
    COMMAND_NOT_SUPPORTED,
    UNSPECIFIED_REASON,
    REPEATED_ATTEMPTS,
    INVALID_PARAMETERS,
    DHKEY_CHECK_FAILD,
    NUMERIC_COMPARISON_FAILED,
    BREDR_PAIRING_INPROGRESS,
    CROSS_TRANSPORTKEY_DERIVATION_NOT_ALLOWED,
    PAIR_SUCCESS=0xFF,
}ble_smp_pair_state_t;

/**
 * @struct ble_smp_t
 * @brief ble smp information.
*/
typedef struct
{
    UINT8 ltk[LINKKEY_SIZE];/**< long term key*/
    UINT8 key_type;            /**< key type*/
    UINT16 ediv;            /**< ediv*/
    UINT8 rand[8];            /**< random*/
    UINT8 enc_size;            /**< key size*/
    UINT8 csrk[LINKKEY_SIZE];/**< csrk*/
    UINT32 sign_counter;    /**< sign counter*/
    UINT8 irk[LINKKEY_SIZE];/**< identity key*/
    UINT32 passkey;
    UINT8 user_confgirm_state;
    ble_smp_pair_state_t smp_state;
    BOOL smp_is_pairing;
}ble_smp_t;

/**
 * @struct ble_device_t
 * @brief ble device information.
*/
typedef struct
{
    bdaddr_t addr;                    /**< peer device identity address*/
    bdaddr_t interim_addr;
    bdaddr_t local_rpa;                /**< local resolved address with peer device*/
    bdaddr_t rpa;                    /**< peer device resolved address*/
    UINT8 addr_type;                /**< peer device identity address type*/
    UINT8 interim_addr_type;
    UINT16 device_state;            /**< peer device state*/
    UINT8 features[8];    /**< peer device feature*/
    UINT8 ex_features[8];    /**< peer device extend feature*/
    UINT8 tx_phy;                    /**< peer device tx phy*/
    UINT8 rx_phy;                    /**< peer device rx phy*/
    UINT8 bonding_flag;             /**<bonding enable>*/
    UINT16 tx_octs;                    /**< peer device tx octets*/
    UINT16 tx_time;                    /**< peer device tx time*/
    UINT16 rx_octs;                    /**< peer device rx octets*/
    UINT16 rx_time;                    /**< peer device rx time*/
    INT64 update_date;
}ble_device_t;

/**
 * @struct ble_acl_t
 * @brief ble acl connection information.
*/
typedef struct
{
    UINT8 role;                        /**< acl role*/
    UINT8 acl_state;                /**< acl state*/
    UINT16 acl_handle;                /**< acl handle*/
    UINT16 connect_interval_min;    /**< connection minimum interval*/
    UINT16 connect_interval_max;    /**< connection maxmum interval*/
    UINT16 slave_latency;            /**< connection latency*/
    UINT16 super_timeout;            /**< connection timeout*/
    UINT16 min_ce_len;                /**< minimum connection event length*/
    UINT16 max_ce_len;                /**< maxmum connection event length*/
}ble_acl_t;

/**
 * @struct ble_client_cfg_t
 * @brief client configuration.
*/
typedef struct
{
    UINT16 cfg_bits;                /**< configuration value*/
    UINT16 att_handle;                /**< attribute value handle*/
}ble_client_cfg_t;

/**
 * @struct ble_gatt_t
 * @brief ble gatt information.
*/
typedef struct
{
    UINT16 mtu;                        /**< peer device mtu*/
    UINT16 trust_flag;                /**< peer device trust flag*/
    UINT8 buff[ATT_MAX_LEN];        /**< peer device response buff*/
    UINT16 buff_len;                /**< peer device response length*/
    UINT16 offset;                    /**< peer device response offset*/
    UINT8 write_cnt;                /**< sign write count*/
    UINT16 write_handle;            /**< write attribute value handle*/
    UINT8 lock;                        /**< gatt lock*/
    UINT8 ccb_cnt;                    /**< client configuration count*/
    ble_client_cfg_t *ccb;            /**< pointer of client configuration*/
    UINT8 req_code;                    /**< gatt request code*/
}ble_gatt_t;

/**
 * @struct ble_smp_vars_t
 * @brief smp temporary variable.
*/
typedef struct
{
    UINT8 peer_pair_info[7];        /**< peer pairing information*/
    UINT8 local_pair_info[7];        /**< local pairing information*/
    UINT8 local_rnd[16];            /**< local random number*/
    UINT8 remote_rnd[16];            /**< peer random number*/
    UINT8 confirm[16];                /**< peer confirmation*/
    UINT8 tk[16];                     /**< term key*/
    UINT8 tk_flag;                    /**< term key flag*/
    UINT8 enc_key_size;                /**< key size*/
    UINT8 smp_timerout;                /**< pairing timer*/
    UINT8 local_plublic_key[64];    /**< local public key*/
    UINT8 peer_plublic_key[64];        /**< peer public key*/
    UINT8 dh_key[32];                /**< dh key*/
    UINT8 mackey_f5_result[16];        /**< mac key*/
    UINT8 ltk_f5_result[16];        /**< long term key*/
    UINT8 remote_check_dhkey_info[16];/**< peer dh key*/
    UINT8 pair_method;                /**< pair mode*/
    UINT8 shift_i;                    /**< passkey bit*/
    UINT8 smp_passkey_state;
} ble_smp_vars_t;

/**
 * @struct ble_conn_t
 * @brief ble connection or encryption information.
*/
typedef struct
{
    ble_smp_t smp;                    /**< local smp information*/
    ble_smp_t smp_remote;            /**< peer smp information*/
    ble_smp_vars_t *smp_calc;        /**< smp temporary variable*/
    ble_device_t dev;                /**< peer device information*/
    ble_acl_t acl;                    /**< acl connection information*/
    ble_gatt_t gatt;                /**< gatt information*/
}ble_conn_t;

/**@brief gatt read and write callback function*/
typedef UINT8 (*ble_gatt_write_cb_t)(ble_conn_t *, void *param);
typedef UINT8 (*ble_gatt_read_cb_t)(ble_conn_t *,void *param);

/**
 * @struct ble_gatt_element_t
 * @brief ble attribute element.
*/
typedef struct
{
    UINT16 length;                    /**< attribute length*/
    UINT16 permisssion;                /**< attribute permission*/
    ble_gatt_uuid_t uuid;            /**< attribute uuid*/
    UINT32  fmt;                    /**< attribute format*/
    void *attValue;                    /**< attribute value*/
    ble_gatt_write_cb_t  cb;        /**< attribute write callback function*/
    ble_gatt_read_cb_t  read_cb;    /**< attribute read callback function*/
} ble_gatt_element_t;

/**
 * @struct ble_conn_update_param_t
 * @brief ble connection update param.
*/
typedef struct
{
    UINT16 interval_min;            /**< connection minimum interval*/
    UINT16 interval_max;            /**< connection maxmum interval*/
    UINT16 latency;                    /**< connection latency*/
    UINT16 timeout;                    /**< connection timeout*/
    UINT16 min_ce_len;                /**< minimum connection event length*/
    UINT16 max_ce_len;                /**< maxmum connection event length*/
}ble_conn_update_param_t;

/**
 * @struct ble_ext_adv_param_t
 * @brief ble extended adv param.
*/
typedef struct
{
    UINT8 handle;                    /**< extended adv handle*/
    UINT16 prop;                    /**< extended adv properties*/
    UINT8 interval_min[3];            /**< minimum extended adv interval*/
    UINT8 interval_max[3];            /**< maxmum extended adv interval*/
    UINT8 map;                        /**< extended channel map*/
    UINT8 own_addr_type;            /**< own address type*/
    UINT8 peer_addr_type;            /**< peer address type*/
    bdaddr_t peer_addr;                /**< peer address*/
    UINT8 filter;                    /**< extended adv filter*/
    UINT8 tx_power;                    /**< extended adv tx power*/
    UINT8 phy;                        /**< primary phy*/
    UINT8 secondary_skip;            /**< secondary maxmum skip*/
    UINT8 secondary_phy;            /**< secondary phy*/
    UINT8 sid;                        /**< extended adv sid*/
    UINT8 scan_req_enable;            /**< scan request notification enable*/
}ble_ext_adv_param_t;

/**
 * @struct ble_ext_adv_data_t
 * @brief ble extended adv data.
*/
typedef struct
{
    UINT8 handle;                    /**< extended adv handle*/
    UINT8 operation;                /**< extended adv operation*/
    UINT8 fragment;                    /**< extended adv fragment*/
    UINT8 len;                        /**< extended adv data length*/
    UINT8 *data;                    /**< extended adv data pointer*/
}ble_ext_adv_data_t;

/**
 * @struct ble_ext_adv_enable_t
 * @brief ble extended adv enable.
*/
typedef struct
{
    UINT8 enable;                    /**< extended adv enable*/
    UINT8 *handle;                    /**< extended adv handles*/
    UINT8 num_of_set;                /**< number of sets*/
    UINT16 *duration;                /**< extended adv duration*/
    UINT8 *max_adv_event;            /**< maximumn extended adv events*/
}ble_ext_adv_enable_t;

/**
 * @struct ble_period_adv_param_t
 * @brief ble period adv param.
*/
typedef struct
{
    UINT8 handle;                    /**< period adv handle*/
    UINT16 interval_min;            /**< period adv minimum interval*/
    UINT16 interval_max;            /**< period adv maximum interval*/
    UINT16 prop;                    /**< period adv properties*/
}ble_period_adv_param_t;

/**
 * @struct ble_period_adv_data_t
 * @brief ble period adv data.
*/
typedef struct
{
    UINT8 handle;                    /**< period adv handle*/
    UINT8 operation;                /**< period adv operation*/
    UINT8 len;                        /**< period adv data length*/
    UINT8 *data;                    /**< period adv data pointer*/
}ble_period_adv_data_t;

/**
 * @struct ble_ext_scan_enable_t
 * @brief ble extend adv scan enable.
*/
typedef struct
{
    UINT8 enable;                    /**< extend adv scan enable*/
    UINT8 filter_duplicates;        /**< extend adv scan duplicate filter*/
    UINT16 duration;                /**< extend adv scan duration*/
    UINT16 period;                    /**< extend adv scan period*/
} ble_ext_scan_enable_t;

/**
 * @struct ble_ext_scan_param_t
 * @brief ble extend adv scan param.
*/
typedef struct
{
    UINT8 own_addr_type;            /**< own address type*/
    UINT8 scan_type[2];                /**< extend adv scan type*/
    UINT16 scan_interval[2];        /**< extend adv scan interval*/
    UINT16 scan_window[2];            /**< extend adv scan window*/
    UINT8 filter_policy;            /**< extend adv scan filter policy*/
    UINT8 scan_phy;                    /**< extend adv scan phys*/
} ble_ext_scan_param_t;

/**
 * @struct ble_conn_param_t
 * @brief ble connection param.
*/
typedef struct
{
    UINT16 scan_interval;            /**< host recommend scan interval*/
    UINT16 scan_window;                /**< host recommend scan window*/
    UINT8  filter_policy;            /**< whether or not to use white list*/
    UINT8  peer_addr_type;            /**< peer address type*/
    bdaddr_t peer_addr;                /**< peer address*/
    UINT8 own_addr_type;            /**< local address type*/
    UINT16 conn_interval_min;        /**< minimum connection interval*/
    UINT16 conn_interval_max;        /**< maximum connection interval*/
    UINT16 conn_latency;            /**< connection latency*/
    UINT16 timeout;                    /**< connection timeout*/
    UINT16 min_ce_len;                /**< minimum connection event length*/
    UINT16 max_ce_len;                /**< maximum connection event length*/
}ble_conn_param_t;

/**
 * @struct ble_ext_conn_param_t
 * @brief ble extend connection param.
*/
typedef struct
{
    UINT8 init_filter_policy;        /**< whether or not to use white list*/
    UINT8 own_addr_type;            /**< local address type*/
    UINT8 peer_addr_type;            /**< peer address type*/
    bdaddr_t peer_addr;                /**< peer address*/
    UINT8 init_phys;                /**< which phy should be used*/
    UINT16 scan_interval[3];        /**< host recommend scan interval*/
    UINT16 scan_window[3];            /**< host recommend scan window*/
    UINT16 con_intv_min[3];            /**< minimum connection interval*/
    UINT16 con_intv_max[3];            /**< maximum connection interval*/
    UINT16 con_latency[3];            /**< connection latency*/
    UINT16 superv_to[3];            /**< connection timeout*/
    UINT16 ce_len_min[3];            /**< minimum connection event length*/
    UINT16 ce_len_max[3];            /**< maximum connection event length*/
}ble_ext_conn_param_t;

typedef struct
{
    UINT8 addr_type;
    UINT8 addr[SIZE_OF_BDADDR];
}ble_stack_white_list_t;

typedef struct
{
    UINT32 le_encryption:1; ///0 bit
    UINT32 conn_param_req:1;
    UINT32 extended_rej_ind:1;
    UINT32 slave_init_features_exchange:1;
    UINT32 le_ping:1;
    UINT32 le_data_len_extension:1;
    UINT32 ll_privacy:1;
    UINT32 extended_scan_filter_policies:1;
    UINT32 le_2m_phy:1;
    UINT32 stable_modul_index_transmiter:1;
    UINT32 stable_modul_index_recerver:1;
    UINT32 le_code_phy:1;
    UINT32 le_extended_adv:1;
    UINT32 le_periodic_adv:1;
    UINT32 channel_select_algo_2:1;
    UINT32 le_power_class_1:1;
    UINT32 min_num_of_used_channels_pro:1;
    UINT32 conn_cte_req:1;
    UINT32 conn_cte_resp:1;
    UINT32 conn_cte_transmitter:1;
    UINT32 conn_cte_recerver:1;
    UINT32 antenna_switch_aod:1;
    UINT32 antenna_switch_aoa:1;
    UINT32 receiving_constant_tone_extensions:1;
    UINT32 periodic_adv_sync_trans_sender:1;
    UINT32 periodic_adv_sync_trans_recipient:1;
    UINT32 sleep_clock_accuracy_update:1;
    UINT32 remote_public_key_validation:1;
    UINT32 conn_iso_stream_master:1;
    UINT32 conn_iso_stream_slave:1;
    UINT32 iso_broadcaster:1;
    UINT32 sync_receiver:1; ///31bit
    UINT32 iso_channel_host_support:1;///32 bit
    UINT32 le_power_control_req:1;
    UINT32 le_power_change_ind:1;
    UINT32 le_path_loss_monitor:1;
    UINT32 reserve_for_future:28;

}le_features_data_t;

/**
 * @struct ble_stack_data_t
 * @brief ble stack data.
*/
typedef struct
{
    UINT8 adv_enable;                /**< ble stack adv state */
    UINT8 ext_adv_handle;            /**< ble stack extend adv handle */
    UINT8 ext_adv_enable;            /**< ble stack extend adv state */
    UINT8 ext_adv_sets;                /**< ble stack extend adv set */
    UINT32 ext_adv_size;            /**< maximum extend adv sets */
    bdaddr_t local_addr;            /**< local public address */
    bdaddr_t random_addr;            /**< local random address */
    UINT8 addr_type;                /**< local address type */
    UINT32 acl_buff_size;            /**< local acl buffer size */
    UINT32 acl_buff_num;            /**< local acl buffer number */
    UINT32 white_list_size;            /**< white list size */
    UINT32 resolved_list_size;        /**< resolved list size */
    UINT32 gatt_ccb_cnt;            /**< local client configuration bits */
    le_features_data_t le_features; /**< local ble features */
    le_features_data_t remote_le_features; /**< remote ble features */
    struct _bt_stack_storage_t *p_bt_storage;
} ble_stack_data_t;
/**
 * @struct ble_smp_mode_t
 * @brief ble smp mode.
*/
typedef enum
{
    MODE_PAIR_FAIL,
    MODE_LEGACY_JUSTWORK,
    MODE_LEGACY_PASSKEY_OUTPUT,
    MODE_LEGACY_PASSKEY_INPUT,
    MODE_LEGACY_OOB,
    MODE_SECURE_JUSTWORK,
    MODE_SECURE_NUMBER_COMPARISON,
    MODE_SECURE_PASSKEY_OUTPUT,
    MODE_SECURE_PASSKEY_INPUT,
    MODE_SECURE_OOB,
}ble_smp_mode_t;

typedef struct att_list
{
    UINT16 handle;                    /**< attribute value handle*/
    UINT16 valueLen;                /**< attribute value length*/
    UINT16 proxy_addr;                /**< used for mesh proxy*/
    ble_gatt_element_t *attribute;    /**< point of attribute data*/
    struct att_list *next;            /**< point of next attribute data*/
} att_list_t;

/**
 * @struct ble_stack_server_callbacks_t
 * @brief ble stack server callback function.
*/
typedef struct
{
/**
* @brief Advertising state change callback
* @param [out] state Advertising state
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*adv_state_change_cb)(UINT8 state, UINT8 err_code);    /**< adv state changed callback*/
/**
* @brief Set advertising data callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*set_adv_data_cb)(UINT8 err_code);                    /**< set adv data callback*/
/**
* @brief Set advertising parameters callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*set_adv_param_cb)(UINT8 err_code);                    /**< set adv param callback*/
/**
* @brief Set scan response data callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*set_scan_rsp_data_cb)(UINT8 err_code);                /**< set scan response data callback*/
/**
* @brief Set extend advertising parameters callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*set_ext_adv_param_cb)(UINT8 err_code);                /**< set extend adv param callback*/
/**
* @brief Set extend advertising data callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*set_ext_adv_data_cb)(UINT8 err_code);                    /**< set extend adv data callback*/
/**
* @brief Extend advertising state change callback
* @param [out] handle Extend advertising handle
* @param [out] state Extend advertising state
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*ext_adv_state_change_cb)(UINT8 handle, UINT8 state, UINT8 err_code); /**< extend adv state changed callback*/
/**
* @brief Set extend scan response data callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*set_ext_scan_rsp_data_cb)(UINT8 err_code);            /**< set extend scan response data callback*/
/**
* @brief Set extend advertising random address callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*set_ext_random_addr_cb)(UINT8 err_code);                /**< set extend adv random address callback*/
/**
* @brief Remove an advertising from the Controller callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*delete_ext_adv_sets_cb)(UINT8 err_code);                /**< delete extend adv sets callback*/
/**
* @brief Remove all existing advertising sets from the Controller callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*clear_ext_adv_sets_cb)(UINT8 err_code);                /**< clear extend adv sets callback*/
/**
* @brief Set the parameters for periodic advertising callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*set_period_adv_param_cb)(UINT8 err_code);                /**< set period adv param callback*/
/**
* @brief Set the data used in periodic advertising PDUs callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*set_period_adv_data_cb)(UINT8 err_code);                /**< set period adv data callback*/
/**
* @brief Periodic advertising state change callback
* @param [out] handle Periodic advertising handle
* @param [out] state Periodic advertising state
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*period_adv_state_change_cb)(UINT8 handle, UINT8 state, UINT8 err_code); /**< period adv state changed callback*/

#ifdef CONFIG_FIBOCOM_BASE            //add patch CQ:SPCSS01419072
/**
* @brief Receive gatt data callback
* @param [out] p_conn Ble connection structure
* @return None
*/
    bt_status_t (*gatt_cb)(ble_conn_t *p_conn);                    /**< gatt data callback*/
#endif
} ble_stack_server_callbacks_t;

/**
 * @struct ble_stack_common_callbacks_t
 * @brief ble stack common callback function.
*/
typedef struct
{
/**
* @brief Ble connection state change callback
* @param [out] p_conn Ble connection structure
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*connection_state_change_cb)(ble_conn_t *p_conn, UINT8 err_code);    /**< ble connection change callback*/
/**
* @brief Controller number of complete packets callback
* @param [out] free_packets Number of currently idle packets
* @return None
*/
    void (*num_of_complete_cb)(UINT8 free_packets); /**< hci number of complete event callback*/
/**
* @brief Set ble random address callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*set_random_addr_cb)(UINT8 err_code);        /**< hci set random address callback*/
/**
* @brief Ble connection parameters update callback
* @param [out] p_conn Ble connection structure
* @param [out] param Ble connection parameters
* @return None
*/
    void (*conn_param_update_cb)(ble_conn_t *p_conn, ble_conn_update_param_t *param); /**< ble connection param update callback*/
/**
* @brief Add device to white list callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*add_white_list_cb)(UINT8 err_code);        /**< add device to white list callback*/
/**
* @brief Remove device from white list callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*remove_white_list_cb)(UINT8 err_code);    /**< remove device from white list callback*/
/**
* @brief Clear white list callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*clear_white_list_cb)(UINT8 err_code);        /**< clear white list callback*/
/**
* @brief Add device to resolved list callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*add_resolved_list_cb)(UINT8 err_code);    /**< add device to resolved list callback*/
/**
* @brief Remove device from resolved list callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*remove_resolved_list_cb)(UINT8 err_code); /**< remove device from resolved list callback*/
/**
* @brief Clear resolved list callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*clear_resolved_list_cb)(UINT8 err_code);    /**< clear resolved list callback*/
/**
* @brief Enable resolved list callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*enable_resolved_list_cb)(UINT8 err_code);    /**< enable resolved list callback*/
/**
* @brief Set the length of time the Controller uses a Resolvable Private Address callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*set_resolved_addr_period_cb)(UINT8 err_code); /**< set resolved address period callback*/
/**
* @brief Host suggest maximum transmission packet size and maximum packet transmission time callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*data_length_change_cb)(ble_conn_t *p_conn);    /**< hci set data length callback*/
/**
* @brief Set the PHY preferences for the connection callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    void (*phy_update_cb)(ble_conn_t *p_conn);            /**< hci phy update event callback*/
/**
* @brief mtu change callback
* @param [out] p_conn->gatt.mtu is current mtu
* @return None
*/
    void (*mtu_change_cb)(ble_conn_t *p_conn);
/**
* @brief read_rssi_cb
* @param [out] p_conn current connection
* @param [out] int rssi The value of rssi
* @return None
*/
    void (*read_rssi_cb)(ble_conn_t *p_conn, int rssi);
} ble_stack_common_callbacks_t;

/**
 * @struct ble_stack_client_callbacks_t
 * @brief ble stack client callback function.
*/
typedef struct
{
/**
* @brief Set the scan parameters callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    bt_status_t (*set_scan_param_cb)(UINT8 err_code);            /**< set scan param callback*/
/**
* @brief Broadcast advertisements were received callback
* @param [out] data Advertisements data
* @param [out] len Advertisements data length
* @return None
*/
    bt_status_t (*adv_report_cb)(UINT8 *data, UINT8 len);        /**< adv report callback*/
/**
* @brief Extend broadcast advertisements were received callback
* @param [out] data Advertisements data
* @param [out] len Advertisements data length
* @return None
*/
    bt_status_t (*ext_adv_report_cb)(UINT8 *data, UINT16 len);    /**< extend adv report callback*/
/**
* @brief Period broadcast advertisements were received callback
* @param [out] data Advertisements data
* @param [out] len Advertisements data length
* @return None
*/
    bt_status_t (*period_adv_report_cb)(UINT8 *data, UINT16 len); /**< period adv report callback*/
/**
* @brief Stop scan callback
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    bt_status_t (*stop_scan_cb)(UINT8 err_code);                /**< stop scan callback*/
/**
* @brief Receive gatt data callback
* @param [out] p_conn Ble connection structure
* @return None
*/
    bt_status_t (*gatt_cb)(ble_conn_t *p_conn);                    /**< gatt data callback*/
} ble_stack_client_callbacks_t;

/**
 * @struct ble_stack_smp_callbacks_t
 * @brief ble stack smp callback function.
*/
typedef struct
{
/**
* @brief The change of the encryption mode has been completed callback
* @param [out] p_conn Ble connection structure
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    bt_status_t (*encrypt_change_cb)(ble_conn_t *p_conn, UINT8 err_code);    /**< connection encryption change callback*/
/**
* @brief Smp pairing complete callback
* @param [out] p_conn Ble connection structure
* @param [out] err_code Whether or not it was successfully executed
* @return None
*/
    bt_status_t (*pair_complete_cb)(ble_conn_t *p_conn, UINT8 err_code);    /**< pairing complete callback*/
/**
* @brief Smp user confirm passkey callback
* @param [out] p_conn Ble connection structure
* @param [in] value Ble smp pair displaypasskey
* @return None
*/
    bt_status_t (*user_confirm_cb)(ble_conn_t *p_conn, UINT8 mode, UINT32 value);
} ble_stack_smp_callbacks_t;


/**
 * @struct ble_stack_server_interface_t
 * @brief ble stack server interface.
*/
typedef struct
{
/**
* @brief Add ble service
* @param [in] p_element Ble service pointer
* @param [in] size Number of ble service element
* @return Execute result
*/
    bt_status_t (*add_services)(ble_gatt_element_t *p_element, UINT32 size);    /**< add ble service */
/**
* @brief Clear ble service
* @return Execute result
*/
    bt_status_t (*clear_services)(void);    /**< clear ble service */
/**
* @brief Set advertisement data
* @param [in] data Advertisement data pointer
* @param [in] length Advertisement data length
* @return Execute result
*/
    bt_status_t (*set_adv_data)(UINT8 *data, UINT32 length);                    /**< set adv data */
/**
* @brief Set advertisement parameter
* @param [in] param Advertisement parameter structure pointer
* @return Execute result
*/
    bt_status_t (*set_adv_param)(ble_adv_param_t *param);                        /**< set adv param */
/**
* @brief Set advertisement enable
* @param [in] enable Advertisement enable
* @return Execute result
*/
    bt_status_t (*set_adv_enable)(UINT8 enable);                                /**< set adv enable */
/**
* @brief Send notification
* @param [in] p_conn Ble connection structure
* @param [in] param Ble read & write parameter,you should fill out:att_handle,len,data
* @return Execute result
*/
    bt_status_t (*send_notification)(ble_conn_t *p_conn, ble_gatt_rw_t *param);        /**< send notification */
/**
* @brief Send indication
* @param [in] p_conn Ble connection structure
* @param [in] param Ble read & write parameter,you should fill out:att_handle,len,data
* @return Execute result
*/
    bt_status_t (*send_indication)(ble_conn_t *p_conn, ble_gatt_rw_t *param);        /**< send indication */
/**
* @brief Set scan response data
* @param [in] data Scan response data pointer
* @param [in] data Scan response data length
* @return Execute result
*/
    bt_status_t (*set_scan_rsp_data)(UINT8 *data, UINT32 length);                /**< set scan response data */
/**
* @brief Set extend advertisement parameter
* @param [in] param Extend advertisement parameter pointer
* @return Execute result
*/
    bt_status_t (*set_ext_adv_param)(ble_ext_adv_param_t *param);                /**< set extend adv param */
/**
* @brief Set extend advertisement data
* @param [in] param Extend advertisement data pointer
* @return Execute result
*/
    bt_status_t (*set_ext_adv_data)(ble_ext_adv_data_t *param);                    /**< set extend adv data */
/**
* @brief Set extend advertisement enable
* @param [in] param Extend advertisement enable pointer
* @return Execute result
*/
    bt_status_t (*set_ext_adv_enable)(ble_ext_adv_enable_t *param);                /**< set extend adv enable */
/**
* @brief Set extend scan response data
* @param [in] param Extend scan response data pointer
* @return Execute result
*/
    bt_status_t (*set_ext_scan_rsp_data)(ble_ext_adv_data_t *param);            /**< set extend scan response data */
/**
* @brief Set extend advertisement random address
* @param [in] handle Extend advertisement handle
* @param [in] random_addr Random address pointer
* @return Execute result
*/
    bt_status_t (*set_ext_adv_random_addr)(UINT8 handle, bdaddr_t *random_addr);/**< set extend adv random address */
/**
* @brief Get the maximum length of advertisement data supported by the Controller
* @param None
* @return The maximum length of advertisement data
*/
    UINT32      (*get_ext_adv_size)(void);                                        /**< get maximum extend adv length */
/**
* @brief Get the maximum number of advertising sets supported by the advertising Controller at the same time.
* @param None
* @return The maximum number of advertising sets
*/
    UINT8       (*get_ext_adv_sets_size)(void);                                    /**< get maximum extend adv sets */
/**
* @brief Remove an advertising set from the Controller
* @param [in] handle Extend advertisement handle
* @return Execute result
*/
    bt_status_t (*delete_ext_adv)(UINT8 handle);                                /**< delete extend adv set */
/**
* @brief Remove all existing advertising sets from the Controller
* @param None
* @return Execute result
*/
    bt_status_t (*clear_ext_adv_sets)(void);                                    /**< clear extend adv sets */
/**
* @brief Set period advertisement parameter
* @param [in] param Period advertisement parameter pointer
* @return Execute result
*/
    bt_status_t (*set_period_adv_param)(ble_period_adv_param_t *param);            /**< set period adv param */
/**
* @brief Set period advertisement data
* @param [in] param Period advertisement data pointer
* @return Execute result
*/
    bt_status_t (*set_period_adv_data)(ble_period_adv_data_t *param);            /**< set period adv data */
/**
* @brief Set period advertisement enable
* @param [in] enable Period advertisement enable
* @param [in] handle Period advertisement handle
* @return Execute result
*/
    bt_status_t (*set_period_adv_enable)(UINT8 enable, UINT8 handle);            /**< set period adv enable */
} ble_stack_server_interface_t;

/**
 * @struct ble_stack_client_interface_t
 * @brief ble stack client interface.
*/
typedef struct
{
/**
* @brief Set scan parameter
* @param [in] param Scan patameter pointer
* @return Execute result
*/
    bt_status_t (*set_scan_param)(ble_scan_param_t *param);                        /**< set scan param */
    bt_status_t (*set_ext_scan_param)(ble_ext_scan_param_t *param);
    bt_status_t (*set_ext_scan_enable)(ble_ext_scan_enable_t *param);
/**
* @brief Start scan
* @param [in] filter_enable controls whether the Link Layer should filter out duplicate advertising reports to the Host
* @return Execute result
*/
    bt_status_t (*start_scan)(UINT8 filter_enable);                                /**< start scan */
/**
* @brief Stop scan
* @param None
* @return Execute result
*/
    bt_status_t (*stop_scan)(void);                                                /**< stop scan */
/**
* @brief Create ble connection
* @param [in] param Ble connection parameter pointer
* @return Execute result
*/
    bt_status_t (*connect)(ble_conn_param_t *param);                            /**< create connection */
/**
* @brief Cancel ble connection
* @param None
* @return Execute result
*/
    bt_status_t (*connect_cancel)(void);                            /**< cancel create connection */
/**
* @brief Disconnect ble connection
* @param [in] p_conn Ble connection structure
* @param [in] reason Disconnection reason
* @return Execute result
*/
    bt_status_t (*disconnect)(ble_conn_t *p_conn, UINT8 reason);                /**< disconnect connection */
/**
* @brief Gatt discover all primary service
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt discover param,you should fill out:start_handle,end_handle
* @return Execute result
*/
    bt_status_t (*discover_all_primary_service)(ble_conn_t *p_conn, ble_gatt_discover_t *param);/**< gatt discover all primary service */
/**
* @brief Gatt discover primary service by uuid
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt discover param,you should fill out:start_handle,end_handle,uuid,type
* @return Execute result
*/
    bt_status_t (*discover_primary_service_by_uuid)(ble_conn_t *p_conn, ble_gatt_discover_t *param);/**< gatt discover primary service by uuid */
/**
* @brief Gatt find include service
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt discover param,you should fill out:start_handle,end_handle
* @return Execute result
*/
    bt_status_t (*find_include_service)(ble_conn_t *p_conn, ble_gatt_discover_t *param);/**< gatt find include service */
/**
* @brief Gatt discover all characteristics
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt discover param,you should fill out:start_handle,end_handle
* @return Execute result
*/
    bt_status_t (*discover_all_char)(ble_conn_t *p_conn, ble_gatt_discover_t *param);    /**< gatt discover all characteristics */
/**
* @brief Gatt discover all characteristic by uuid
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt discover param,you should fill out:start_handle,end_handle,uuid,type
* @return Execute result
*/
    bt_status_t (*discover_char_by_uuid)(ble_conn_t *p_conn, ble_gatt_discover_t *param);/**< gatt discover all characteristic by uuid */
/**
* @brief Gatt discover all characteristic description
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt discover param,you should fill out:start_handle,end_handle
* @return Execute result
*/
    bt_status_t (*discover_all_char_desc)(ble_conn_t *p_conn, ble_gatt_discover_t *param);/**< gatt discover all characteristic description */
/**
* @brief Gatt read characteristic
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt read & write param,you should fill out:att_handle
* @return Execute result
*/
    bt_status_t (*read_char)(ble_conn_t *p_conn, ble_gatt_rw_t *param);            /**< gatt read characteristic */
/**
* @brief Gatt read characteristic by uuid
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt discover param,you should fill out:start_handle,end_handle,uuid,type
* @return Execute result
*/
    bt_status_t (*read_char_by_uuid)(ble_conn_t *p_conn, ble_gatt_discover_t *param);/**< gatt read characteristic by uuid */

/**
* @brief Gatt read characteristic by uuid
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt read & write param,you should fill out:att_handle,offset
* @return Execute result
*/
    bt_status_t (*read_long_char)(ble_conn_t *p_conn, ble_gatt_rw_t *param);/**< gatt read long characteristic value */

/**
* @brief Gatt read characteristic multipul
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt read & write param,you should fill out:len,data
* @return Execute result
*/
    bt_status_t (*read_multiple_char)(ble_conn_t *p_conn, ble_gatt_rw_t *param);/**< gatt read characteristic multipul */
/**
* @brief Gatt write characteristic without response
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt read & write param,you should fill out:att_handle,len,data
* @return Execute result
*/
    bt_status_t (*write_char_without_rsp)(ble_conn_t *p_conn, ble_gatt_rw_t *param);/**< gatt write characteristic without response */
/**
* @brief Gatt write characteristic
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt read & write param,you should fill out:att_handle,len,data
* @return Execute result
*/
    bt_status_t (*write_char_req)(ble_conn_t *p_conn, ble_gatt_rw_t *param);        /**< gatt write characteristic */
/**
* @brief Gatt write characteristic reliable
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt read & write param,you should fill out:att_handle,len,data
* @return Execute result
*/
    bt_status_t (*write_char_reliable)(ble_conn_t *p_conn, ble_gatt_rw_t *param);    /**< gatt write characteristic reliable */
/**
* @brief Gatt signed write characteristic without response
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt read & write param,you should fill out:att_handle,len,data
* @return Execute result
*/
    bt_status_t (*write_char_signed)(ble_conn_t *p_conn, ble_gatt_rw_t *param);        /**< gatt signed write characteristic without response */
/**
* @brief Gatt write long characteristic
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt read & write param,you should fill out:att_handle,len,data
* @return Execute result
*/
    bt_status_t (*write_long_char_req)(ble_conn_t *p_conn, ble_gatt_rw_t *param);        /**< gatt write long characteristic */
/**
* @brief Gatt write execute characteristic
* @param [in] p_conn Ble connection structure
* @param [in] param Gatt read & write param,you should fill out:flag
* @return Execute result
*/
    bt_status_t (*write_execute_char_req)(ble_conn_t *p_conn, UINT8 flag);        /**< gatt write execute characteristic */
} ble_stack_client_interface_t;

/**
 * @struct ble_stack_smp_interface_t
 * @brief ble stack smp interface.
*/
typedef struct
{
/**
* @brief Smp start pair
* @param [in] p_conn Ble connection structure
* @return Execute result
*/
    bt_status_t (*start_pair)(ble_conn_t *p_conn);/**< smp start pair */
/**
* @brief Smp remove pairing device
* @param [in] p_conn Ble connection structure
* @return Execute result
*/
    bt_status_t (*remove_pair_dev)(ble_conn_t *p_conn);            /**< delete paired device */
/**
* @brief Smp clear pairing device list
* @param None
* @return Execute result
*/
    bt_status_t (*clear_pair_list)(void);                        /**< clear paired device list */
/**
* @brief Smp user confirm
* @param [in] p_conn Ble smp connection structure
* @param [in] pair_mode Ble smp pair mode
* @param [in] passkey Ble smp pair inputpasskey
* @return Execute result
*/
    bt_status_t (*smp_user_confirm)(ble_conn_t *p_conn,UINT8 pair_mode,UINT32 passkey);
/**
* @brief Smp configpara set
* @param [in] iocap user can set this paramter to change io capability
* @return Execute result
*/
    bt_status_t(*smp_configpara_set)( UINT8 io_cap, UINT8 auth_Req, UINT32 pass_key,UINT32 pairing_timeout);
/**
* @brief Smp configpara get
* @param None
* @return Execute result
*/
    bt_status_t(*smp_configpara_get)( smp_config_t  *config);
} ble_stack_smp_interface_t;

/**
 * @struct ble_stack_common_interface_t
 * @brief ble stack common interface.
*/
typedef struct
{
/**
* @brief Set local ble address
* @param [in] addr Ble address pointer
* @param [in] type Ble address type
* @return Execute result
*/
    bt_status_t (*set_local_addr)(bdaddr_t *addr, UINT8 type);    /**< set local address */
/**
* @brief Get local ble address
* @param [out] addr Ble address pointer
* @param [in] type Ble address type
* @return Execute result
*/
    bt_status_t (*get_local_addr)(bdaddr_t *addr, UINT8 type);    /**< get local address */
/**
* @brief Request to update ble connection parameter
* @param [in] p_conn Ble connection structure
* @param [in] param Ble connection parameter pointer
* @return Execute result
*/
    bt_status_t (*conn_param_update_req)(ble_conn_t *p_conn, ble_conn_update_param_t *param);/**< connection param update request */
/**
* @brief Set the length of time the Controller uses a Resolvable Private Address
* @param [in] sec Seconds
* @return Execute result
*/
    bt_status_t (*set_resolved_addr_period)(UINT16 sec);        /**< set resolved address period */
/**
* @brief Get local acl buffer size
* @param [out] packets_size Acl buffer packets size
* @param [out] packets_num Number of acl buffer packets
* @return Execute result
*/
    bt_status_t (*get_local_buff_size)(UINT32 *packets_size, UINT32 *packets_num);/**< get local acl buffer size */
/**
* @brief Add device to white list
* @param [in] addr Ble device address pointer
* @param [in] addr_type Ble device address type
* @return Execute result
*/
    bt_status_t (*add_white_list)(bdaddr_t *addr, UINT8 addr_type);/**< add device to white list */
/**
* @brief Remove device from white list
* @param [in] addr Ble device address pointer
* @param [in] addr_type Ble device address type
* @return Execute result
*/
    bt_status_t (*remove_white_list)(bdaddr_t *addr, UINT8 addr_type);/**< delete device from white list */
/**
* @brief Clear device from white list
* @param None
* @return Execute result
*/
    bt_status_t (*clear_white_list)(void);                        /**< clear white list */
/**
* @brief Get white list size
* @param None
* @return White list size
*/
    UINT32      (*get_white_list_size)(void);                    /**< get white list size */
/**
* @brief Add device to resolved list
* @param [in] peer_ida_type peer identity address type
* @param [in] peer_ida peer identity address pointer
* @param [in] peer_irk peer irk
* @param [in] local_irk local irk
* @return Execute result
*/
    bt_status_t (*add_resolved_list)(UINT8 peer_ida_type, bdaddr_t *peer_ida,
                UINT8 *peer_irk, UINT8 *local_irk);                /**< add device to resolved list */
/**
* @brief Get resolved list size
* @param None
* @return Resolved list size
*/
    UINT32      (*get_resolved_list_size)(void);                /**< get resolved list size */
/**
* @brief Remove device from resolved list
* @param [in] peer_ida_type peer identity address type
* @param [in] peer_ida peer identity address pointer
* @return Execute result
*/
    bt_status_t (*remove_resolved_list)(UINT8 peer_ida_type, bdaddr_t *peer_ida);/**< delete device from resolved list */
/**
* @brief Clear resolved list
* @param None
* @return Execute result
*/
    bt_status_t (*clear_resolved_list)(void);                    /**< clear resolved list */
/**
* @brief Enable resolved list
* @param [in] enable Resolved list enable
* @return Execute result
*/
    bt_status_t (*enable_resolved_list)(UINT8 enable);            /**< enable resolved list */
/**
* @brief Suggest maximum transmission packet size and maximum packet transmission time to be used for a given connection
* @param [in] p_conn Ble connection structure
* @param [in] tx_oct Preferred maximum number of payload octets
* @param [in] tx_time Preferred maximum number of microseconds
* @return Execute result
*/
    bt_status_t (*set_data_length)(ble_conn_t *p_conn, UINT16 tx_oct, UINT16 tx_time);/**< set controller data length */
/**
* @brief Get ble device list
* @param None
* @return Ble device list pointer
*/
    ble_conn_t **(*get_dev_list)(void);                            /**< get stack device list */
/**
* @brief get ble connection
* @param [in] handle acl handle of ble connection
* @param [in] addr address of ble connection
* @return ble_conn_t pointer of ble connection with handle or addr
*/
    ble_conn_t *(*get_conn)(UINT16 *handle, bdaddr_t *addr);/**< get ble connection*/
/**
* @brief Gatt exchange mtu request
* @param [in] p_conn Ble connection structure
* @param [in] mtu Gatt Client mtu
* @return Execute result
*/
  bt_status_t (*exchange_mtu)(ble_conn_t *p_conn, UINT16 mtu);

  bt_status_t (*read_rssi)(ble_conn_t *p_conn);
} ble_stack_common_interface_t;

/**
 * @struct ble_stack_callbacks_t
 * @brief ble stack callback function.
*/
typedef struct
{
    ble_stack_server_callbacks_t *srv_cb;                /**< ble stack server callback */
    ble_stack_client_callbacks_t *cli_cb;                /**< ble stack client callback */
    ble_stack_common_callbacks_t *comm_cb;                /**< ble stack common callback */
    ble_stack_smp_callbacks_t *smp_cb;                    /**< ble stack smp callback */
} ble_stack_callbacks_t;

/**
 * @struct ble_stack_interface_t
 * @brief ble stack interface.
*/
typedef struct
{
    ble_stack_server_interface_t *srv_interface;        /**< ble stack server interface */
    ble_stack_client_interface_t *cli_interface;        /**< ble stack client interface */
    ble_stack_common_interface_t *comm_interface;        /**< ble stack common interface */
    ble_stack_smp_interface_t *smp_interface;            /**< ble stack smp interface */
    bt_status_t (*init)(ble_stack_callbacks_t *cb);        /**< ble stack initial interface */
    void (*cleanup)(void);                                /**< ble stack cleanup interface */
} ble_stack_interface_t;

ble_stack_callbacks_t *ble_stack_get_callback(void);

ble_stack_interface_t *ble_stack_get_interface(void);

ble_conn_t *ble_stack_find_conn(UINT16 *handle, bdaddr_t *addr);

/*ble paramter defines*/
#define BLE_ADDRESS_LEN  6
typedef enum
{
    BLE_OWN_ADDR_PUBLIC,/*public device address*/
    BLE_OWN_ADDR_RANDOM_STATIC,/*random static address*/
}bt_ble_own_address_type_e;

typedef struct
{
    UINT8 address[BLE_ADDRESS_LEN];
}bt_ble_address_t;

typedef struct
{
    bt_ble_own_address_type_e remote_address_type;
    bt_ble_address_t      remote_address;
}bt_ble_remote_address_t;

typedef enum
{
    BLE_CHANNEL_MAP_37 = 1,
    BLE_CHANNEL_MAP_38,
    BLE_CHANNEL_MAP_37_38,
    BLE_CHANNEL_MAP_39,
    BLE_CHANNEL_MAP_37_39,
    BLE_CHANNEL_MAP_38_39,
    BLE_CHANNEL_MAP_37_38_39,
}bt_ble_adv_channal_map_e;

typedef enum
{
    BLE_NON_DISCOVERABLE_MODE,
    BLE_BROADCASTER_MODE = 4,
    BLE_LIMITED_DISCOVERABLE_MODE,
    BLE_NORMAL_DISCOVERABLE_MODE,
}bt_ble_adv_discover_mode_e;

typedef enum
{
    BLE_ALLOW_SCAN_CONNECT_ANY = 0,/**< Allow both scan and connection requests from anyone. */
    BLE_ALLOW_SCAN_WHITE_CONNECT_ANY, /**< Allow scan req from white-list devices only and connection req from anyone. */
    BLE_ALLOW_SCAN_ANY_CONNECT_WHITE,/**< Allow scan req from anyone and connection req from white-list devices only. */
    BLE_ALLOW_SCAN_WHITE_CONNECT_WHITE,/**< Allow scan and connection requests from white-list devices only. */
}bt_ble_filter_policy_e;

typedef enum
{
    BLE_SUCCESS,
    BLE_UNKOWN_HCI_COMMAND,
    BLE_UNKOWN_CONNECTION_IDENTIFIER,
    BLE_HARDWARE_FAILURE,
    BLE_PAGE_TIMEOUT,
    BLE_AUTHENTICATION_FAILURE,
    BLE_PIN_OR_KEY_MISSING,
    BLE_MEMORY_CAPACITY_EXCEEDED,
    BLE_CONNECTION_TIMEOUT,
    BLE_CONNECTION_LIMIT_EXCEEDED,
    BLE_SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED,
    BLE_CONNECTION_ALREADY_EXISTS,
    BLE_COMMAND_DISALLOWED,
    BLE_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES,
    BLE_CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS,
    BLE_CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR,
    BLE_CONNECTION_ACCEPT_TIMEOUT_EXCEEDED,
    BLE_UNSUPPORTED_FEATURE_OR_PARAMTER_VALUE,
    BLE_INVALID_HCI_COMMAND_PARAMETERS,
    BLE_REMOTE_USER_TERMINATED_CONNECTION,
    BLE_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES,
    BLE_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_POWER_OFF,
    BLE_CONNECION_TERMINATED_BY_LOCAL_HOST,
    BLE_REPEATED_ATTEMPTS,
    BLE_PAIRING_NOT_ALLOWED,
    BLE_UNKNOWN_LMP_PDU,
    BLE_UNSUPPORTED_REMOTE_FEATURE,/*UNSUPPORTED_LMP_FEATURE*/
    BLE_SCO_OFFET_REJECTED,
    BLE_SCO_INTERVAL_REJECTED,
    BLE_SCO_AIR_REJECTED,
    BLE_INVALID_LMP_PARAMETER,/*INVALID_LL_PARAMETER*/
    BLE_UNSPECIFIED_ERROR,
    BLE_UNSUPPORTED_LMP_PARAMETER_VALUE,/*UNSUPPORTED_LL_PARAMETER_VALUE*/
    BLE_ROLE_CHANGE_NOT_ALLOWED,
    BLE_LMP_RESPONSE_TIMEOUT,/*LL_RESPONSE_TIMEOUT*/
    BLE_LMP_ERROR_TRANSACTION_COLLIISION,/*LL_PROCEDURE_COLLISION*/
    BLE_LMP_PDU_NOT_ALLOWED,
    BLE_ENCRYPTION_MODE_NOT_ACCEPTABLE,
    BLE_LINK_KEY_CANNOT_BE_CHANGED,
    BLE_REQUESTED_QOS_NOT_SUPPORTED,
    BLE_INSTANT_PASSED,
    BLE_PAIRING_WITH_UINT_KEY_NOT_SUPPORTED,
    BLE_DIFFERENT_TRANSACTION_COLLISION,
    BLE_RESERVED_FOR_FUTURE_USE_A,
    BLE_QOS_UNACCEPTABLE_PARAMETER,
    BLE_QOS_REJECTED,
    BLE_CHANNAL_CLASSIFICATION_NOT_SUPPORTED,
    BLE_INSUFFICIENT_SECURITY,
    BLE_PARAMETER_OUT_OF_MANDATORY_RANGE,
    BLE_RESERVED_FOR_FUTURE_USED,
    BLE_ROLE_SWITCH_PENDING,
    BLE_RESERVED_FOR_FUTURE_USE_B,
    BLE_RESERVED_SLOT_VIOLATION,
    BLE_ROLE_SWITCH_FAILED,
    BLE_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE,
    BLE_SECURE_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST,
    BLE_HOST_BUSY_PAIRING,
    BLE_CONNECTION_REJECTED_DUE_TO_NO_SUITABLE_CHANNAL_FOUND,
    BLE_CONTROLLER_BUSY,
    BLE_UNACCEPTABLE_CONNECTION_PARAMETERS,
    BLE_ADVERTISING_TIMEOUT,
    BLE_CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE,
    BLE_CONNECTION_FAILED_TO_BE_ESTABLISHED,/*SYNCHRONIZATION_TIMEOUT*/
    BLE_MAC_CONNECTION_FAILED,
    BLE_COARSE_CLOCK_ADJUSTMENT_REJECTED,
    BLE_TYPE0_SUBMAP_NOT_DEFINED,
    BLE_UNKNOWN_ADVERTISING_IDENTIFIER,
    BLE_LIMIT_REACHED,
    BLE_OPERATION_CANCELLED_BY_HOST,
    BLE_PACKET_TOO_LONG,
}bt_ble_error_code_e;

#endif


