#pragma once
#include <Windows.h>


typedef struct _TCPconnect {
	struct _TCPconnect* NextIP;
	BYTE IsSendName;
	char* PlayerName;
	HANDLE TCPThread;
	SOCKET TCPSocket;
	wchar_t* MyName;
	void* Internet;
	struct sockaddr_in *TCPSockAdrr;
} *PTCPConnect,TCPConnect;

typedef struct __List {
	HANDLE MUTEX;
	PTCPConnect LIST;
	HANDLE Heap;
} *PTCPList,_TCPList;


int AddNewIP(PTCPList, PTCPConnect);
PTCPConnect RemoveIP(PTCPList Connect, PTCPConnect ConnectSock);
PTCPList CreateList();
PTCPConnect GetIP(PTCPList List, int index);

PTCPList GetUsers(PTCPList Source);
int RemoveUsers(PTCPList);
PTCPConnect GetUserOFIndex(PTCPList List, int index);

int AddNewIP(PTCPList, PTCPConnect);
PTCPConnect RemoveIP(PTCPList Connect, PTCPConnect ConnectSock);
PTCPList CreateList();

