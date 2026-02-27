#include "tcp.h"
#include <stdio.h>
#include "../common/log.h"

//=============================================================================
// Module Variables
//=============================================================================
TCPSocketTypedef ServerSocket[TCP_MAX_SOCKETS] = {{0}};
static uint8_t tcp_callbacks_registered = 0;
static uint8_t g_tcp_thread_running = 0;












//=============================================================================
// Socket State Processing
//=============================================================================
void TCPSocket_Process(TCPSocketTypedef* socket)
{
    if (!socket || !socket->isEnabled) {
        return;
    }
    
    switch (socket->SocketState) {
        case SOCKET_CLOSED:
            // Socket is closed, nothing to do
            break;
            
        case SOCKET_IDLE:
            // Socket is idle, ready for connection
            if (socket->DNSorIP[0] != '\0' && socket->Port > 0) {
                socket->SocketState = SOCKET_DNS_PENDING;
                LOGD("TCP", "Socket %d moving to DNS_PENDING", socket->SocketIndex);
            }
            break;
            
        default:
            break;
    }
}









void tcp_thread_init(uint32_t taskId)
{
    int i;
    
    for (i = 0; i < TCP_MAX_SOCKETS; i++) {
        ServerSocket[i].SocketIndex = i;
        ServerSocket[i].SocketNo = -1;
        ServerSocket[i].SocketState = SOCKET_CLOSED;
        ServerSocket[i].PrevState = SOCKET_CLOSED;
        ServerSocket[i].isEnabled = 1;
        ServerSocket[i].Port = 0;
        ServerSocket[i].DNSorIP[0] = '\0';
        ServerSocket[i].rxBuffer = NULL;
        ServerSocket[i].rxSizeMAX = 0;
        ServerSocket[i].isRXData = 0;
        ServerSocket[i].rval.reconnect_delay_ms = 1000;
        ServerSocket[i].rval.dnsresolved = 0;
        ServerSocket[i].rval.last_ack_number = 0;
        ServerSocket[i].rval.noackcount = 0;
        ServerSocket[i].OnConnect = NULL;
        ServerSocket[i].Connected = NULL;
        ServerSocket[i].OnDisconnect = NULL;
    }
    
    LOGI("TCP", "TCP module initialized with %d sockets", TCP_MAX_SOCKETS);
}


//=============================================================================
// TCP Thread Function
//=============================================================================
void tcp_thread_entry(void *param)
{
    int i;
    
    g_tcp_thread_running = 1;
    
    while (g_tcp_thread_running) 
    {
        // Process all enabled sockets
        for (i = 0; i < TCP_MAX_SOCKETS; i++) {
            if (ServerSocket[i].isEnabled) {
                TCPSocket_Process(&ServerSocket[i]);  // Call process function
            }
        }
        
        fibo_taskSleep(10);
    }
}