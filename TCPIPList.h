#pragma once
#include <Windows.h>


typedef struct _TCPconnect {
	struct _TCPconnect* NextIP;
	char* PlayerName;
	HANDLE TCPThread;
	SOCKET TCPSocket;
	struct sockaddr *TCPSockAdrr;
} *PTCTConnect,TCPConnect;

typedef struct __List {
	HANDLE MUTEX;
	PTCTConnect LIST;
} *PTCPList,_TCPList;

int AddNewIP(PTCPList, PTCTConnect);
PTCTConnect RemoveIP(PTCPList Connect, PTCTConnect ConnectSock);
PTCPList CreateList();