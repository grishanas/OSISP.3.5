#include "TCPIPList.h"
#include <Windows.h>

int AddNewIP(PTCPList Connect, PTCTConnect ConnectSock)
{
	
	WaitForSingleObject(Connect->MUTEX, INFINITE);
	{
		__try {
			PTCTConnect temp=Connect->LIST;
			if (temp == NULL)
			{
				temp = ConnectSock;
				__leave;

			}
			while (temp->NextIP != NULL && temp->TCPSocket!=ConnectSock->TCPSocket)
			{
				temp = temp->NextIP;
			}

			if (temp->TCPSocket != ConnectSock->TCPSocket)
			{
				temp->NextIP = ConnectSock;
			}

		}
		__finally {
			ReleaseMutex(Connect->MUTEX);
		}
	}
}

PTCTConnect RemoveIP(PTCPList Connect, PTCTConnect ConnectSock)
{
	WaitForSingleObject(Connect->MUTEX, INFINITE);
	{
		__try {
			PTCTConnect temp = Connect->LIST,tmp=temp;
			while (temp != NULL)
			{
				if (temp->TCPSocket == ConnectSock->TCPSocket)
				{
					tmp->NextIP = temp->NextIP;
					//HeapFree(Connect->heap, 0, temp);
					__leave;
				}
				tmp = temp;
				temp = temp->NextIP;
			}
		}
		__finally {
			ReleaseMutex(Connect->MUTEX);
		}
	}
}

PTCPList CreateList() 
{
	PTCPList TCPList = malloc(sizeof(_TCPList));
	TCPList->MUTEX = CreateMutex(NULL, FALSE, NULL);
	//TCPList->heap= HeapCreate(HEAP_GENERATE_EXCEPTIONS, 0x0f0000, 0);

	return TCPList;
}