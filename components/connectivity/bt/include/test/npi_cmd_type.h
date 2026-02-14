/*
 *  <blueu file> - <bluetooth stack implementation>
 *
 *  Copyright (C) 2019 Unisoc Communications Inc.
 *  History:
 *      <2019.10.11> <wcn bt>
 *      Description
 */

#ifndef _NPI_CMD_TYPE_H_
#define _NPI_CMD_TYPE_H_
//#include "sci_types.h"
//#include "bt_app.h"

#define BT_NONSIG_SUPPORT

/* Default Value TX */
#define BT_EUT_TX_PATTERN_DEAFULT_VALUE (4) /*PRBS9*/
#define BT_EUT_TX_CHANNEL_DEAFULT_VALUE (5)
#define BT_EUT_TX_PKTTYPE_DEAFULT_VALUE (4) /*DH1*/
#define BT_EUT_PKTLEN_DEAFULT_VALUE (37)
#define BT_EUT_POWER_TYPE_DEAFULT_VALUE (0)
#define BT_EUT_POWER_VALUE_DEAFULT_VALUE (0)
#define BT_EUT_PAC_CNT_DEAFULT_VALUE (0)

/* Default Value RX */
#define BT_EUT_RX_PATTERN_DEAFULT_VALUE (7) /* RX Pattern is only 7 */
#define BT_EUT_RX_CHANNEL_DEAFULT_VALUE (5)
#define BT_EUT_RX_PKTTYPE_DEAFULT_VALUE (4) /*DH1*/
#define BT_EUT_RX_RXGAIN_DEAFULT_VALUE (0)  /* Auto */
#define BT_EUT_RX_ADDR_DEAFULT_VALUE ("00:00:88:C0:FF:EE")

#define BT_EUT_COMMAND_RSP_MAX_LEN (128)

#define EUT_BT_OK ("+SPBTTEST:OK")
#define EUT_BT_ERROR ("+SPBTTEST:ERR=")
#define EUT_BT_REQ ("+SPBTTEST:EUT=")

#define EUT_BLE_OK ("+SPBLETEST:OK")
#define EUT_BLE_ERROR ("+SPBLETEST:ERR=")
#define EUT_BLE_REQ ("+SPBLETEST:EUT=")

#define BLE_TESTMODE_REQ_RET ("+SPBLETEST:TESTMODE=")
#define BLE_ADDRESS_REQ_RET ("+SPBLETEST:TESTADDRESS=")
#define BLE_TX_CHANNEL_REQ_RET ("+SPBLETEST:TXCH=")
#define BLE_RX_CHANNEL_REQ_RET ("+SPBLETEST:RXCH=")
#define BLE_TX_PATTERN_REQ_RET ("+SPBLETEST:TXPATTERN=")
#define BLE_RX_PATTERN_REQ_RET ("+SPBLETEST:RXPATTERN=")
#define BLE_TX_PKTTYPE_REQ_RET ("+SPBLETEST:TXPKTTYPE=")
#define BLE_RX_PKTTYPE_REQ_RET ("+SPBLETEST:RXPKTTYPE=")
#define BLE_TXPKTLEN_REQ_RET ("+SPBLETEST:TXPKTLEN=")
#define BLE_TXPWR_REQ_RET ("+SPBLETEST:TXPWR=")
#define BLE_RXGAIN_REQ_RET ("+SPBLETEST:RXGAIN=")
#define BLE_TX_REQ_RET ("+SPBLETEST:TX=")
#define BLE_RX_REQ_RET ("+SPBLETEST:RX=")
#define BLE_RXDATA_REQ_RET ("+SPBLETEST:RXDATA=")
#define BLE_TX_PHY_REQ_RET ("+SPBLETEST:TXPHYTYPE=")
#define BLE_RX_PHY_REQ_RET ("+SPBLETEST:RXPHYTYPE=")

#define BT_TESTMODE_REQ_RET ("+SPBTTEST:TESTMODE=")
#define BT_ADDRESS_REQ_RET ("+SPBTTEST:TESTADDRESS=")
#define BT_TX_CHANNEL_REQ_RET ("+SPBTTEST:TXCH=")
#define BT_RX_CHANNEL_REQ_RET ("+SPBTTEST:RXCH=")
#define BT_TX_PATTERN_REQ_RET ("+SPBTTEST:TXPATTERN=")
#define BT_RX_PATTERN_REQ_RET ("+SPBTTEST:RXPATTERN=")
#define BT_TX_PKTTYPE_REQ_RET ("+SPBTTEST:TXPKTTYPE=")
#define BT_RX_PKTTYPE_REQ_RET ("+SPBTTEST:RXPKTTYPE=")
#define BT_TXPKTLEN_REQ_RET ("+SPBTTEST:TXPKTLEN=")
#define BT_TXPWR_REQ_RET ("+SPBTTEST:TXPWR=")
#define BT_RXGAIN_REQ_RET ("+SPBTTEST:RXGAIN=")
#define BT_TX_REQ_RET ("+SPBTTEST:TX=")
#define BT_RX_REQ_RET ("+SPBTTEST:RX=")
#define BT_RXDATA_REQ_RET ("+SPBTTEST:RXDATA=")
#define BT_MAC_STR_MAX_LEN (12 + 5)


typedef enum
{
    BT_TESTMODE_NONE   = 0,  // Leave Test mode
    BT_TESTMODE_SIG    = 1,  // Enter EUT signal mode
    BT_TESTMODE_NONSIG = 2   // Enter Nonsignal mode
}BT_TEST_MODE_T;

/* Command Type */
typedef enum {
    BTEUT_CMD_TYPE_TX = 0,
    BTEUT_CMD_TYPE_RX,
    BTEUT_CMD_TYPE_INVALID = 0xff
} bteut_cmd_type;

typedef enum {
    BTEUT_BT_MODE_OFF = 0,
    BTEUT_BT_MODE_CLASSIC,
    BTEUT_BT_MODE_BLE,
    BTEUT_BT_MODE_INVALID = 0xff
} bteut_bt_mode;

/* Test Mode */
typedef enum {
    BTEUT_TESTMODE_LEAVE = 0,
    BTEUT_TESTMODE_ENTER_EUT,
    BTEUT_TESTMODE_ENTER_NONSIG,
    BTEUT_TESTMODE_INVALID = 0xff
} bteut_testmode;

typedef enum {
    BTEUT_EUT_RUNNING_OFF = 0,
    BTEUT_EUT_RUNNING_ON,
    BTEUT_EUT_RUNNING_INVALID = 0xff
} bteut_eut_running;

typedef enum {
    BTEUT_TXRX_STATUS_OFF = 0,
    BTEUT_TXRX_STATUS_TXING,
    BTEUT_TXRX_STATUS_RXING,
    BTEUT_TXRX_STATUS_INVALID = 0xff
} bteut_txrx_status;

/* TXPWR Type */
typedef enum {
    BTEUT_TXPWR_TYPE_POWER_LEVEL = 0,
    BTEUT_TXPWR_TYPE_GAIN_VALUE,
    BTEUT_TXPWR_TYPE_INVALID = 0xff
} bteut_txpwr_type;

typedef enum {
    BTEUT_LE_LEGACY_PHY,
    BTEUT_LE_1M_PHY,
    BTEUT_LE_2M_PHY,
    BTEUT_LE_Coded_Both_PHY,
    BTEUT_LE_Coded_S8_PHY,
    BTEUT_LE_Coded_S2_PHY,
} bteut_phy_type;

/* power */
typedef struct {
    bteut_txpwr_type power_type;
    unsigned int power_value;
} BT_EUT_POWER;

/* TX pamater */
typedef struct {
    int pattern;
    int channel;
    int pkttype;
    int pktlen;
    UINT8 phy;
    BT_EUT_POWER txpwr;
} BTEUT_TX_ELEMENT;

typedef enum
{
    BT_CHIP_ID_NULL = 0,
    BT_CHIP_ID_RDA  = 1,
    BT_CHIP_ID_BEKEN  = 2,
    BT_CHIP_ID_UNISOC_NEMOL  = 3,
    BT_CHIP_ID_UNISOC_MARLIN2  = 4,
    BT_CHIP_ID_UNISOC_W317 = 5,
    BT_CHIP_ID_MAX
}BT_CHIP_ID_T;


typedef enum {
    BTEUT_STANDARD_INDEX = 0,
    BTEUT_STABLE_INDEX,
} bteut_modulation_index;

/* Gain */
typedef enum {
    BTEUT_GAIN_MODE_AUTO = 0,
    BTEUT_GAIN_MODE_FIX,
    BTEUT_GAIN_MODE_INVALID = 0xff
} bteut_gain_mode;

/* Gain */
typedef struct {
    bteut_gain_mode mode;
    unsigned int value;
} BT_EUT_GAIN;

/* RX pamater */
typedef struct {
    int pattern;
    int channel;
    int pkttype;
    UINT8 phy;
    UINT8 modulation_index;
    BT_EUT_GAIN rxgain;
    char addr[BT_MAC_STR_MAX_LEN + 1];
} BTEUT_RX_ELEMENT;
#if 0
typedef struct _BT_NONSIG_DATA
{
    UINT8 rssi;
    BT_STATUS status;
    UINT32 pkt_cnt;
    UINT32 pkt_err_cnt;
    UINT32 bit_cnt;
    UINT32 bit_err_cnt;
}BT_NONSIG_DATA;
#endif
typedef struct _BT_NONSIG_PACKET
{
    UINT16 pac_type;    //enum{NULLpkt, POLLpkt, FHSpkt, DM1, DH1, HV1, HV2, HV3, DV, AUX1, DM3, DH3, EV4, EV5, DM5, DH5, IDpkt, INVALIDpkt, EDR_2DH1, EV3, EDR_2EV3, EDR_3EV3, EDR_3DH1, EDR_AUX1, EDR_2DH3, EDR_3DH3, EDR_2EV5, EDR_3EV5, EDR_2DH5, EDR_3DH5};
                                 //for NemoL BLE Range: 0x00 - 0x07, Display: {11111111100000111101, 11110000, 10101010, PRBS15, 11111111, 00000000, 00001111, 01010101};
    UINT16 pac_len;     //according to packet type, max length is {0, 0, 18, 17, 27, 10, 20, 30, 9, 29, 121, 183, 120, 180, 224, 339, 0, 0, 54, 30, 60, 90, 83, 29, 367, 552, 360, 540, 679, 1021}
    UINT16 pac_cnt;     //0~65536
}BT_NONSIG_PACKET;

typedef struct _BT_NONSIG_POWER
{
    UINT8 power_type;     //0 or 1
    UINT16 power_value;  //0~33
} BT_NONSIG_POWER;

typedef struct _BT_NONSIG_PARAM
{
    UINT8 pattern;    //00000000 ---1, 11111111 ---2, 10101010 ---3, PRBS9 ---4, 11110000 ---9
    UINT8 channel;    //255 or 0~78
    union
    {
        UINT8 rx_gain;    //0~32
        BT_NONSIG_POWER power;
    }item;
    BT_NONSIG_PACKET pac;
    UINT8 le_phy;// 1:LE1M, 2:LE2M, 3:LE Coded S8, 4:LE Coded S2 |  3:LE Coded in RX
    UINT8 le_rx_mod_index; // 0:Standard;  1:Stable
} BT_NONSIG_PARAM;

//typedef void (*BT_NONSIGCALLBACK)(const BT_NONSIG_DATA* data);

extern BOOL npi_bt_enable(void);
extern BOOL npi_bt_disable(void);
extern BOOL npi_ble_enable(void);
extern BOOL npi_ble_disable(void);

void bt_test_mode_set(BT_TEST_MODE_T mode);
BT_TEST_MODE_T bt_test_mode_get(void);

void bt_npi_init(void);
BOOL bt_npi_enter_eut_mode(void);
int bt_npi_test_address_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int bt_npi_test_tx_ch_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int bt_npi_test_rx_ch_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int bt_npi_test_tx_pattern_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int bt_npi_test_rx_pattern_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int bt_npi_test_tx_pkttype_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int bt_npi_test_rx_pkttype_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int bt_npi_test_tx_pktlen_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int bt_npi_test_tx_pwr_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int bt_npi_test_rx_gain_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int bt_npi_test_tx_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int bt_npi_test_rx_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int bt_npi_test_rx_data_handle(char *argv[],int argc,char *rsp,int * rsp_len);


int ble_npi_test_address_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int ble_npi_txch_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int ble_npi_rxch_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int ble_npi_tx_pattern_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int ble_npi_rx_pattern_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int ble_npi_tx_pkttype_handle(char * argv [ ], int argc, char * rsp, int * rsp_len);
int ble_npi_rx_pkttype_handle(char * argv [ ], int argc, char * rsp, int * rsp_len);
int ble_npi_tx_pktlen_handle(char * argv [ ], int argc, char * rsp, int * rsp_len);
int ble_npi_tx_pwr_handle(char * argv [ ], int argc, char * rsp, int * rsp_len);
int ble_npi_rx_gain_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int ble_npi_tx_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int ble_npi_rx_handle(char *argv[],int argc,char *rsp,int * rsp_len);
int ble_npi_rxdata_handle(char * argv [ ], int argc, char * rsp, int * rsp_len);

#endif //_NPI_CMD_TYPE_
