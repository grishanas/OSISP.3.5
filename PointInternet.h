#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#pragma comment(lib,"Ws2_32.lib")

#include "Queue.h"
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "TCPIPList.h"

#define POINTINTERNET_BUFFERSIZE 128

#define UDPListener_PORT 4343
#define UDPSender_PORT 4344
#define TCP_PORT 4345
#define TCPListenPort 4346

typedef struct _spis{
	struct _spis* NextElement;
	char buffer[256];
	int Lenght;
} *PMessage,spis;


typedef struct _PointInternet
{
	PTCPList TCPList;
	HANDLE ParentThread;
	HANDLE UDPListenerThread;
	HANDLE Thread;
	HANDLE heap;
	SOCKET UDPListenerSocket;
	HWND hwnd;
	PQueue Queue;
	char* PlayerName;

}*PPointInternet, PointInternet;

PPointInternet CreateInternetConnection(HWND hwnd);
