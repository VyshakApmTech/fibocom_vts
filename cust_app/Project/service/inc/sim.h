#ifndef __SIM_H__
#define __SIM_H__

#include "fibo_opencpu.h"

// PDP state enumeration (internal use only)
typedef enum {
    PDP_STATE_IDLE = 0,
    PDP_STATE_ACTIVATING,
    PDP_STATE_ACTIVE,
    PDP_STATE_FAILED
} pdp_state_t;

// // Operator information structure (if not in fibo_opencpu.h)
// typedef struct {
//     uint16_t mcc;
//     uint16_t mnc;
//     // other fields as needed
// } operator_info_t;

// Neighbour cell structure (matching Quectel)
typedef struct
{
	int mnc;
	int mcc;
	char CellID[8];
	char LAC[8];
	char CellDB[8];
} NeigbourCellTypedef;

// GSM Structure (EXACT match to Quectel)
typedef struct
{
	enum {SIM_NOT_DETECTED=0, SIM_DETECTED, GPRS_INIT, GPRS_ACTIVE} GSMState;
	uint8_t SignalStrength;
	uint16_t IsModemOK;
	uint32_t ModemRespCount;
	uint16_t MCC;
	uint16_t MNC;
	char LAC[6];
	char CellID[8];
	NeigbourCellTypedef NeigbourCell[4];
	uint8_t IsTimeSet;
	uint8_t IsNeighbourCells;
	uint8_t IsRegDenied;  // Flag for registration denied state
} GSM_Typedef;

// Provider enum (matching Quectel)
typedef enum {NONE, VI, BSNL, AIRTEL, JIO} Providertypedef;

// Network Structure (matching Quectel)
typedef struct 
{
	char IMEI[30];
	char IMSI[30];
	char Network[60];
	char SIMNo[30];
	char APN[50];
	Providertypedef Provider;
} NET_Typedef;

// RTC Structure (matching Quectel)
typedef struct _RTC
{
  uint16_t Year;
  uint8_t Month;
  uint8_t Date;
  uint8_t DaysOfWeek;
  uint8_t Hour;
  uint8_t Min;
  uint8_t Sec;
} _RTC;

// External variables (matching Quectel)
extern GSM_Typedef GSM;
extern NET_Typedef NetWork;
extern _RTC CurrentDateTime;
extern Providertypedef prfReq;
extern uint8_t PrfChanged;

// SIM service functions
void sim_init(void);
void sim_process(void);
BOOL sim_is_ready(void);

// GPRS functions
void GprsThreadEntry(void *param);
void InitGPRSThread(int taskId);
int CheckGPRSState(void);
uint8_t GetNextValidProfile(uint8_t currentProfile);
uint8_t SwitchProfile(uint8_t num);
uint8_t SendAtCmd(char* str, char* resp, char* grep);
uint8_t GetNeighbourCells(void);
void GetImei(void);
int SetupAutoTimesync(void);
static void get_apn_from_sim(void);
static void process_register(void);
static void update_signal_strength(void);

#endif
