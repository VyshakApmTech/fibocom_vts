#ifndef GPS_H
#define GPS_H

#include "fibo_opencpu.h"
#include <string.h>
#include <stdlib.h>

// GPS UART Configuration
#define GPS_UART_PORT hal_uart_port3  // Check Fibocom's UART port definition
#define GPS_UART_BAUDRATE 9600

#define GPS_UART_BUFFER_SIZE 4096
#define GPS_RCV_TIMEOUT  100 // 100*30ms = 3s

// GPS Reset Pin (check your hardware pin mapping)
#define GPS_RESET_PIN   31  // Example GPIO pin - ADJUST BASED ON YOUR HARDWARE
#define GPS_RESET_ON    fibo_gpio_write(GPS_RESET_PIN, 1)
#define GPS_RESET_OFF   fibo_gpio_write(GPS_RESET_PIN, 0)
#define GPS_RESET_VAL   fibo_gpio_read(GPS_RESET_PIN)
#define GPS_RESET_INIT  fibo_gpio_init(GPS_RESET_PIN, GPIO_OUTPUT, 0)

// NMEA Time Structure
struct NMEA_TIME {
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
    uint16_t Millisecond;
};

// NMEA Date Structure
struct NMEA_DATE {
    uint8_t Day;
    uint8_t Month;
    uint16_t Year;
};

// GPS Main Structure (EXACT match to Quectel M66)
typedef struct
{
    uint8_t State;       
	uint8_t GPSFix;			// 1 or 0
	double Latitude;
	char LatDir;
	double Longitude;
	char LngDir;
	double Speed;
	double Heading;
	uint8_t NoOfSatalite;
	double Altitude;
	double PDOP;
	double HDOP;
	struct NMEA_TIME Time;
	struct NMEA_DATE Date;
    // Satellite counts
    int SatTotal;
    int SatGPS;     // GP
    int SatGLONASS; // GL
    int SatGalileo; // GA
    int SatBeiDou;  // GB
    int SatQZSS;    // GQ
    int SatNavIC;   // GI
    int SatMixed;   // GN (combined multi-constellation sentences)
} GPS_Typedef;

// External variables (EXACT match to Quectel M66)
extern GPS_Typedef GPS;
extern int GPSDateTime;
extern char sLatitude[];
extern char sLongitude[];
extern char sAltitude[];
extern char sSpeed[];
extern char sPDOP[];
extern char sHDOP[];
extern char sHeading[];

// NMEA data structures (from NMeaParser.h - include if needed)
// For now, declare as extern if you have NMeaParser.h
// If not, you'll need to define these structures

// GPS Command types
typedef enum {
    GPS_CMD_NONE = 0,
    GPS_CMD_REBOOT,
    GPS_CMD_UART_CONFIG,
    GPS_CMD_READ_CONFIG,
} GPS_CmdType;

// GPS Command status structure
typedef struct {
    GPS_CmdType type;
    const char* cmd_prefix;    // Command identifier (e.g., "$PQTMCFGUART")
    bool response_pending;
    bool response_received;
    bool response_ok;
    char response[100];
} GPS_CmdStatus;

// External GPS command status
extern GPS_CmdStatus gps_cmd;

// GPS Service Functions
int gps_init(void);
void gps_thread_init(int taskId);
void gps_thread_entry(void *taskId);
void GPS_UartSendString(const char *str);
void SendNMEAToRS232(void);
void gps_overspeed_check(void);
void update_epo(void);

#endif // GPS_H