// TCP.h
#ifndef _TCP_H
#define _TCP_H

#include "fibo_opencpu.h"
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

#define RECV_BUFFER_LEN  1024
#define TCP_MAX_SOCKETS 5
#define TCPSOCKET_CONNECT_TIMEOUT 10  // seconds
#define TCPSOCKET_DNS_TIMEOUT 10      // seconds

typedef void (*connectionupdatecb)(int socketno);

typedef enum {
    SOCKET_CLOSED,
    SOCKET_IDLE,
    SOCKET_DNS_PENDING,
    SOCKET_PENDING_OPEN,
    SOCKET_OPEN,
    SOCKET_CONNECTING,
    SOCKET_CONNECTED,
    SOCKET_CLOSING,
    SOCKET_ERROR
} SocketState_e;

typedef struct {
    uint8_t m_ipaddress[5];
    uint8_t dnsresolved;
    uint32_t stateTimestamp;  // Timestamp for timeouts
    uint32_t reconnect_delay_ms;  // Exponential backoff delay in milliseconds
    uint64_t last_ack_number;  // Track last ACK number for send verification
    uint8_t noackcount;   // Counter for consecutive no-ACK events
} runtimeValTypedef;

/**
 * HTTP callback function types (implemented in http.c)
 */
typedef void (*http_connect_cb_t)(int socketNo);
typedef void (*http_data_cb_t)(int socketNo, const char* data, int len);
typedef void (*http_disconnect_cb_t)(int socketNo);

/**
 * TCP Socket Structure 
 */
typedef struct {
    uint8_t isEnabled;
    int SocketIndex;
    int SocketNo;
    SocketState_e SocketState;
    SocketState_e PrevState;
    int Port;
    char DNSorIP[100];
    char *rxBuffer;
    uint16_t rxSizeMAX;
    uint8_t isRXData;
    runtimeValTypedef rval;
    connectionupdatecb OnConnect;
    connectionupdatecb Connected;
    connectionupdatecb OnDisconnect;
    
    // HTTP-specific callbacks (for HTTP socket only, Socket 4)
    http_connect_cb_t http_on_connect;        // Called when HTTP socket connects
    http_data_cb_t http_on_data;              // Called when HTTP data received  
    http_disconnect_cb_t http_on_disconnect;  // Called when HTTP socket disconnects
} TCPSocketTypedef;




extern TCPSocketTypedef ServerSocket[];


/**
 * Register HTTP callbacks for HTTP socket (Socket 4)
 * @param connect_cb    Called when HTTP socket connects
 * @param data_cb       Called when HTTP data is received  
 * @param disconnect_cb Called when HTTP socket disconnects
 */
void TCP_RegisterHttpCallbacks(http_connect_cb_t connect_cb,
                               http_data_cb_t data_cb,
                               http_disconnect_cb_t disconnect_cb);




void CallBack(connectionupdatecb cb, int socketno);
void tcp_thread_entry(void *param);
void tcp_thread_init(uint32_t taskId);
uint8_t TCPSocket_SendString(TCPSocketTypedef* socket, char* str);
uint8_t TCPSocket_WaitAck(TCPSocketTypedef* socket, int sent_len);
void TCP_CloseALLSockets(void);
void TCPSocket_Process(TCPSocketTypedef* socket);
static uint32_t TCPSocket_GetMsSinceBoot(void);


#endif /* _TCP_H */