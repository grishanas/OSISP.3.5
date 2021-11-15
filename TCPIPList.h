#pragma once
#include <Windows.h>



typedef struct _TCPconnect {
	struct _TCPconnect* NextIP;
	HANDLE TCPThread;
	SOCKET TCPSocket;
} *PTCTConnect,TCPConnect;

typedef struct __List {
	HANDLE MUTEX;
	PTCTConnect LIST;
} *PTCPList,_TCPList;

int AddNewIP(PTCPList, PTCTConnect);
int RemoveIP(PTCPList, PTCTConnect);
PTCPList CreateList();