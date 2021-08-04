#ifndef ZBLUENET_NET_PLATFORM_H
#define ZBLUENET_NET_PLATFORM_H

#ifdef _WIN32
#define FD_SETSIZE 2048
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#else // LINUX

#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <netdb.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)

#endif // linux

#ifndef RECV_BUFFER_SIZE
#define RECV_BUFFER_SIZE 8192
#define SEND_BUFFER_SIZE 10240
#endif // RECV_BUFFER_SIZE


#endif // ZBLUENET_NET_PLATFORM_H
