/**
 * @file gps.c
 * @brief GPS/GNSS implementation using Fibocom OpenCPU GNSS API
 *
 * Uses fibo_gnss_* APIs from Fibocom datasheet.
 * GNSS is enabled via fibo_gnss_enable(), NMEA data polled via fibo_gnss_nmea_get().
 */

#include "gps.h"
#include "fibo_opencpu.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* ============================================================
 *  Global Variables (declared extern in gps.h)
 * ============================================================ */

GPS_Typedef GPS = {0};
int         GPSDateTime = 0;

char sLatitude[20]  = {0};
char sLongitude[20] = {0};
char sAltitude[20]  = {0};
char sSpeed[20]     = {0};
char sPDOP[20]      = {0};
char sHDOP[20]      = {0};
char sHeading[20]   = {0};

GPS_CmdStatus gps_cmd = {0};

/* ============================================================
 *  Internal Defines
 * ============================================================ */

#define GNSS_NMEA_BUF_SIZE      2048
#define GNSS_POLL_INTERVAL_MS   1000    /* poll NMEA every 1 second */
#define GNSS_ENABLE_RETRY       3
#define GNSS_ENABLE_RETRY_DELAY 2000

/* Overspeed threshold in km/h  */
#define GPS_OVERSPEED_THRESHOLD 120.0

/* NMEA sentence IDs */
#define NMEA_GGA  "GGA"
#define NMEA_RMC  "RMC"
#define NMEA_GSA  "GSA"
#define NMEA_GSV  "GSV"
#define NMEA_GLL  "GLL"
#define NMEA_VTG  "VTG"

/* ============================================================
 *  Internal Prototypes
 * ============================================================ */

static void  gnss_parse_nmea(const char *nmea_buf, int len);
static void  parse_gga(const char *sentence);
static void  parse_rmc(const char *sentence);
static void  parse_gsa(const char *sentence);
static void  parse_gsv(const char *sentence, const char *talker);
static void  parse_vtg(const char *sentence);
static int   nmea_checksum_ok(const char *sentence);
static int   nmea_split(const char *sentence, char fields[][20], int max_fields);
static double nmea_coord_to_decimal(const char *coord_str, char dir);
static double parse_double(const char *s);
static int    parse_int(const char *s);

/* ============================================================
 *  GPS Init
 * ============================================================ */

int gps_init(void)
{
    int ret;
    int retry = 0;

    fibo_textTrace("GPS: Initializing GNSS");

    /* Configure satellite constellation: GPS + BDS + GALILEO (default=3) */
    ret = fibo_gnss_set_satellite(3);
    if (ret != 0) {
        fibo_textTrace("GPS: Warning - set_satellite failed (%d)", ret);
    }

    /* Disable AGNSS (use standalone positioning by default) */
    ret = fibo_gnss_set_agnss_mode(0);
    if (ret != 0) {
        fibo_textTrace("GPS: Warning - set_agnss_mode failed (%d)", ret);
    }

    /* Configure NMEA sentences: enable GGA, RMC, GSA, GSV, VTG */
    fibo_gnss_nmea_config("GGA,RMC,GSA,GSV,VTG", 1);

    /* Set NMEA report frequency: 1 second */
    ret = fibo_gnss_nmea_freq_set(1);
    if (ret != 0) {
        fibo_textTrace("GPS: Warning - nmea_freq_set failed (%d)", ret);
    }

    /* Cold start GNSS */
    do {
        ret = fibo_gnss_enable(1);  /* 1 = cold start */
        if (ret == 0) {
            fibo_textTrace("GPS: GNSS enabled (cold start)");
            GPS.State = 1;
            return 0;
        }
        fibo_textTrace("GPS: gnss_enable failed, retry %d/%d", retry + 1, GNSS_ENABLE_RETRY);
        fibo_taskSleep(GNSS_ENABLE_RETRY_DELAY);
        retry++;
    } while (retry < GNSS_ENABLE_RETRY);

    fibo_textTrace("GPS: GNSS init failed after %d retries", GNSS_ENABLE_RETRY);
    GPS.State = 0;
    return -1;
}

/* ============================================================
 *  GPS Thread Entry
 * ============================================================ */

void gps_thread_entry(void *param)
{
    fibo_textTrace("GPS: Thread started");

    /* Wait a moment for system to settle before touching GNSS */
    fibo_taskSleep(3000);

    if (gps_init() != 0) {
        fibo_textTrace("GPS: Init failed, thread exiting");
        return;
    }

    /* Give the chip time to settle after cold start */
    fibo_taskSleep(1000);

    static uint8_t nmea_buf[GNSS_NMEA_BUF_SIZE];

    while (1) {
        memset(nmea_buf, 0, sizeof(nmea_buf));

        int len = fibo_gnss_nmea_get(nmea_buf, sizeof(nmea_buf) - 1);

        if (len > 0) {
            nmea_buf[len] = '\0';
            fibo_textTrace("GPS NMEA[%d]: %.120s", len, (char *)nmea_buf);
            gnss_parse_nmea((char *)nmea_buf, len);

            /* Overspeed check after each position update */
            gps_overspeed_check();
        } else if (len == -1) {
            fibo_textTrace("GPS: nmea_get error");
        }
        /* len == 0: no data yet, just keep polling */

        fibo_taskSleep(GNSS_POLL_INTERVAL_MS);
    }
}

/* Legacy thread init wrapper */
void gps_thread_init(int taskId)
{
    gps_thread_entry((void *)(intptr_t)taskId);
}

/* ============================================================
 *  NMEA Parsing - Top Level
 * ============================================================ */

static void gnss_parse_nmea(const char *nmea_buf, int len)
{
    /* NMEA sentences are separated by \r\n.
     * Walk through the buffer, extract each '$' sentence. */
    const char *p = nmea_buf;
    const char *end = nmea_buf + len;

    while (p < end) {
        /* Find start of sentence */
        while (p < end && *p != '$') p++;
        if (p >= end) break;

        /* Find end of sentence (\r or \n) */
        const char *start = p;
        while (p < end && *p != '\r' && *p != '\n') p++;

        /* Copy to local buffer */
        int slen = (int)(p - start);
        if (slen < 6 || slen >= 100) {
            p++;
            continue;
        }

        char sentence[100];
        memcpy(sentence, start, slen);
        sentence[slen] = '\0';

        /* Validate checksum */
        if (!nmea_checksum_ok(sentence)) {
            p++;
            continue;
        }

        /* Identify talker and sentence type.
         * Format: $TTsss,... where TT = talker (GP, GL, GA, GB, GN...)
         *                            sss = sentence type (GGA, RMC, ...)
         * Proprietary sentences start with $P...
         */
        if (slen < 6) { p++; continue; }

        char talker[3] = {0};
        char stype[4]  = {0};

        if (sentence[1] == 'P') {
            /* Proprietary - skip for now */
            p++;
            continue;
        }

        talker[0] = sentence[1];
        talker[1] = sentence[2];
        stype[0]  = sentence[3];
        stype[1]  = sentence[4];
        stype[2]  = sentence[5];

        if (strcmp(stype, "GGA") == 0) {
            parse_gga(sentence);
        } else if (strcmp(stype, "RMC") == 0) {
            parse_rmc(sentence);
        } else if (strcmp(stype, "GSA") == 0) {
            parse_gsa(sentence);
        } else if (strcmp(stype, "GSV") == 0) {
            parse_gsv(sentence, talker);
        } else if (strcmp(stype, "VTG") == 0) {
            parse_vtg(sentence);
        }

        p++;
    }
}

/* ============================================================
 *  NMEA Sentence Parsers
 * ============================================================ */

/**
 * GGA - Global Positioning System Fix Data
 * $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
 * Field 0: Message ID
 * Field 1: UTC time hhmmss.ss
 * Field 2: Latitude  ddmm.mmmm
 * Field 3: N/S
 * Field 4: Longitude dddmm.mmmm
 * Field 5: E/W
 * Field 6: Fix quality (0=invalid, 1=GPS fix, 2=DGPS)
 * Field 7: Number of satellites
 * Field 8: HDOP
 * Field 9: Altitude
 * Field 10: M (altitude unit)
 */
static void parse_gga(const char *sentence)
{
    char fields[15][20];
    int n = nmea_split(sentence, fields, 15);
    if (n < 10) return;

    /* Fix quality */
    int fix_quality = parse_int(fields[6]);
    GPS.GPSFix = (fix_quality > 0) ? 1 : 0;

    if (!GPS.GPSFix) return;

    /* UTC Time: hhmmss.ss */
    if (strlen(fields[1]) >= 6) {
        char tmp[3] = {0};
        tmp[0] = fields[1][0]; tmp[1] = fields[1][1];
        GPS.Time.Hour = (uint8_t)atoi(tmp);
        tmp[0] = fields[1][2]; tmp[1] = fields[1][3];
        GPS.Time.Minute = (uint8_t)atoi(tmp);
        tmp[0] = fields[1][4]; tmp[1] = fields[1][5];
        GPS.Time.Second = (uint8_t)atoi(tmp);
        /* Milliseconds from decimal portion */
        const char *dot = strchr(fields[1], '.');
        if (dot) GPS.Time.Millisecond = (uint16_t)(parse_double(dot + 1) * 10);
    }

    /* Latitude */
    GPS.Latitude = nmea_coord_to_decimal(fields[2], fields[3][0]);
    GPS.LatDir   = fields[3][0];
    snprintf(sLatitude, sizeof(sLatitude), "%.6f", GPS.Latitude);

    /* Longitude */
    GPS.Longitude = nmea_coord_to_decimal(fields[4], fields[5][0]);
    GPS.LngDir    = fields[5][0];
    snprintf(sLongitude, sizeof(sLongitude), "%.6f", GPS.Longitude);

    /* Satellites */
    GPS.NoOfSatalite = (uint8_t)parse_int(fields[7]);

    /* HDOP */
    GPS.HDOP = parse_double(fields[8]);
    snprintf(sHDOP, sizeof(sHDOP), "%.2f", GPS.HDOP);

    /* Altitude */
    GPS.Altitude = parse_double(fields[9]);
    snprintf(sAltitude, sizeof(sAltitude), "%.1f", GPS.Altitude);
}

/**
 * RMC - Recommended Minimum Specific GNSS Data
 * $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
 * Field 1: UTC time
 * Field 2: Status A=active V=void
 * Field 3: Latitude
 * Field 4: N/S
 * Field 5: Longitude
 * Field 6: E/W
 * Field 7: Speed over ground (knots)
 * Field 8: Course over ground
 * Field 9: Date ddmmyy
 */
static void parse_rmc(const char *sentence)
{
    char fields[13][20];
    int n = nmea_split(sentence, fields, 13);
    if (n < 9) return;

    /* Status */
    if (fields[2][0] != 'A') {
        GPS.GPSFix = 0;
        return;
    }
    GPS.GPSFix = 1;

    /* UTC Time */
    if (strlen(fields[1]) >= 6) {
        char tmp[3] = {0};
        tmp[0] = fields[1][0]; tmp[1] = fields[1][1];
        GPS.Time.Hour = (uint8_t)atoi(tmp);
        tmp[0] = fields[1][2]; tmp[1] = fields[1][3];
        GPS.Time.Minute = (uint8_t)atoi(tmp);
        tmp[0] = fields[1][4]; tmp[1] = fields[1][5];
        GPS.Time.Second = (uint8_t)atoi(tmp);
    }

    /* Latitude / Longitude */
    GPS.Latitude  = nmea_coord_to_decimal(fields[3], fields[4][0]);
    GPS.LatDir    = fields[4][0];
    GPS.Longitude = nmea_coord_to_decimal(fields[5], fields[6][0]);
    GPS.LngDir    = fields[6][0];
    snprintf(sLatitude,  sizeof(sLatitude),  "%.6f", GPS.Latitude);
    snprintf(sLongitude, sizeof(sLongitude), "%.6f", GPS.Longitude);

    /* Speed: knots -> km/h */
    double speed_knots = parse_double(fields[7]);
    GPS.Speed = speed_knots * 1.852;
    snprintf(sSpeed, sizeof(sSpeed), "%.1f", GPS.Speed);

    /* Heading / Course */
    GPS.Heading = parse_double(fields[8]);
    snprintf(sHeading, sizeof(sHeading), "%.1f", GPS.Heading);

    /* Date: ddmmyy */
    if (strlen(fields[9]) >= 6) {
        char tmp[5] = {0};
        tmp[0] = fields[9][0]; tmp[1] = fields[9][1];
        GPS.Date.Day = (uint8_t)atoi(tmp);
        tmp[0] = fields[9][2]; tmp[1] = fields[9][3];
        GPS.Date.Month = (uint8_t)atoi(tmp);
        tmp[0] = fields[9][4]; tmp[1] = fields[9][5];
        GPS.Date.Year = (uint16_t)(2000 + atoi(tmp));

        GPSDateTime = 1;
    }
}

/**
 * GSA - GNSS DOP and Active Satellites
 * Field 2: Fix type (1=no fix, 2=2D, 3=3D)
 * Field 15: PDOP
 * Field 16: HDOP
 * Field 17: VDOP
 */
static void parse_gsa(const char *sentence)
{
    char fields[18][20];
    int n = nmea_split(sentence, fields, 18);
    if (n < 17) return;

    GPS.PDOP = parse_double(fields[15]);
    GPS.HDOP = parse_double(fields[16]);
    snprintf(sPDOP, sizeof(sPDOP), "%.2f", GPS.PDOP);
    snprintf(sHDOP, sizeof(sHDOP), "%.2f", GPS.HDOP);
}

/**
 * GSV - GNSS Satellites in View
 * Field 3: Total number of satellites in view
 * We accumulate per-talker counts.
 */
static void parse_gsv(const char *sentence, const char *talker)
{
    char fields[20][20];
    int n = nmea_split(sentence, fields, 20);
    if (n < 4) return;

    /* Field 1: total messages, Field 2: message number, Field 3: sat count */
    int msg_num   = parse_int(fields[2]);
    int total_sat = parse_int(fields[3]);

    /* On first message of a sequence, update the talker count */
    if (msg_num == 1) {
        if      (strcmp(talker, "GP") == 0) GPS.SatGPS     = total_sat;
        else if (strcmp(talker, "GL") == 0) GPS.SatGLONASS = total_sat;
        else if (strcmp(talker, "GA") == 0) GPS.SatGalileo = total_sat;
        else if (strcmp(talker, "GB") == 0 ||
                 strcmp(talker, "BD") == 0) GPS.SatBeiDou  = total_sat;
        else if (strcmp(talker, "GQ") == 0) GPS.SatQZSS    = total_sat;
        else if (strcmp(talker, "GI") == 0) GPS.SatNavIC   = total_sat;
        else if (strcmp(talker, "GN") == 0) GPS.SatMixed   = total_sat;

        /* Recalculate total */
        GPS.SatTotal = GPS.SatGPS + GPS.SatGLONASS + GPS.SatGalileo +
                       GPS.SatBeiDou + GPS.SatQZSS + GPS.SatNavIC;
        if (GPS.SatTotal == 0) GPS.SatTotal = GPS.SatMixed;
    }
}

/**
 * VTG - Course Over Ground and Ground Speed
 * Field 7: Speed in km/h
 */
static void parse_vtg(const char *sentence)
{
    char fields[10][20];
    int n = nmea_split(sentence, fields, 10);
    if (n < 8) return;

    /* Field 1: True heading, Field 7: Speed km/h */
    GPS.Heading = parse_double(fields[1]);
    GPS.Speed   = parse_double(fields[7]);
    snprintf(sSpeed,   sizeof(sSpeed),   "%.1f", GPS.Speed);
    snprintf(sHeading, sizeof(sHeading), "%.1f", GPS.Heading);
}

/* ============================================================
 *  Utility: Overspeed Check
 * ============================================================ */

void gps_overspeed_check(void)
{
    if (!GPS.GPSFix) return;

    if (GPS.Speed > GPS_OVERSPEED_THRESHOLD) {
        fibo_textTrace("GPS: OVERSPEED! %.1f km/h > %.1f km/h threshold",
                       GPS.Speed, GPS_OVERSPEED_THRESHOLD);
        /* TODO: trigger alarm, send alert SMS/GPRS */
    }
}

/* ============================================================
 *  Utility: Update EPO (Extended Prediction Orbit / AGNSS)
 * ============================================================ */

void update_epo(void)
{
    int ret;

    fibo_textTrace("GPS: Enabling AGNSS for EPO update");

    /* Enable AGNSS */
    ret = fibo_gnss_set_agnss_mode(1);
    if (ret != 0) {
        fibo_textTrace("GPS: AGNSS enable failed (%d)", ret);
        return;
    }

    /*
     * For Allystar add-on GNSS, set the AGNSS server.
     * Adjust the URL/port/token to match your account.
     */
    const char *agnss_url  = "http://cagnss.allystar.com/api/v1/eph/rt"
                             "?token=53caacc6821649e696d36b0ab275e328&sys=gpsbds";
    int         agnss_port = 80;

    ret = fibo_gnss_set_agnss_server((char *)agnss_url,
                                     (int)strlen(agnss_url),
                                     agnss_port,
                                     0);
    if (ret != 0) {
        fibo_textTrace("GPS: AGNSS server set failed (%d)", ret);
    } else {
        fibo_textTrace("GPS: AGNSS server configured");
    }
}

/* ============================================================
 *  Utility: Send NMEA string via UART (RS232 forwarding)
 * ============================================================ */

void GPS_UartSendString(const char *str)
{
    if (!str) return;
    /* Forward raw string to trace output.
     * Replace with fibo_uart_write() if UART forwarding is required. */
    fibo_textTrace("GPS_UART >> %s", str);
}

void SendNMEAToRS232(void)
{
    /* Build a simple summary NMEA-like string and send it */
    if (!GPS.GPSFix) {
        GPS_UartSendString("$GPS,NOFIX*00\r\n");
        return;
    }

    char buf[128];
    snprintf(buf, sizeof(buf),
             "$GPS,%.6f,%c,%.6f,%c,%.1f,%.1f,%02d%02d%04d,%02d%02d%02d\r\n",
             GPS.Latitude,  GPS.LatDir,
             GPS.Longitude, GPS.LngDir,
             GPS.Speed,     GPS.Heading,
             GPS.Date.Day,  GPS.Date.Month, GPS.Date.Year,
             GPS.Time.Hour, GPS.Time.Minute, GPS.Time.Second);

    GPS_UartSendString(buf);
}

/* ============================================================
 *  NMEA Helper Functions
 * ============================================================ */

/**
 * Verify NMEA checksum.
 * Format: $sentence*XX  where XX is XOR of all bytes between $ and *
 */
static int nmea_checksum_ok(const char *sentence)
{
    if (!sentence || sentence[0] != '$') return 0;

    const char *p   = sentence + 1;
    const char *ast = strchr(p, '*');
    if (!ast) return 1; /* No checksum field - accept */

    uint8_t calc = 0;
    while (p < ast) {
        calc ^= (uint8_t)*p++;
    }

    /* Parse the two hex digits after '*' */
    char hex[3] = {ast[1], ast[2], 0};
    uint8_t given = (uint8_t)strtol(hex, NULL, 16);

    return (calc == given);
}

/**
 * Split NMEA sentence into fields (comma-separated).
 * Returns number of fields populated.
 */
static int nmea_split(const char *sentence, char fields[][20], int max_fields)
{
    int  fi   = 0;
    int  ci   = 0;
    const char *p = sentence;

    memset(fields[0], 0, 20);

    while (*p && fi < max_fields) {
        if (*p == ',' || *p == '*' || *p == '\r' || *p == '\n') {
            fields[fi][ci] = '\0';
            fi++;
            ci = 0;
            if (fi < max_fields) memset(fields[fi], 0, 20);
            if (*p == '*') break; /* stop at checksum delimiter */
        } else {
            if (ci < 19) {
                fields[fi][ci++] = *p;
            }
        }
        p++;
    }

    if (ci > 0 && fi < max_fields) {
        fields[fi][ci] = '\0';
        fi++;
    }

    return fi;
}

/**
 * Convert NMEA coordinate (ddmm.mmmm or dddmm.mmmm) to decimal degrees.
 * dir: 'N', 'S', 'E', 'W'
 */
static double nmea_coord_to_decimal(const char *coord_str, char dir)
{
    if (!coord_str || strlen(coord_str) < 4) return 0.0;

    double raw = parse_double(coord_str);
    /* Integer portion is degrees * 100, remainder is minutes */
    int    deg = (int)(raw / 100.0);
    double min = raw - (deg * 100.0);
    double dec = deg + (min / 60.0);

    if (dir == 'S' || dir == 'W') dec = -dec;

    return dec;
}

static double parse_double(const char *s)
{
    if (!s || *s == '\0') return 0.0;
    return atof(s);
}

static int parse_int(const char *s)
{
    if (!s || *s == '\0') return 0;
    return atoi(s);
}