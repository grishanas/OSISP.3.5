#include "TCPIPList.h"
#include <Windows.h>


PTCPConnect GetIP(PTCPList List, int index)
{
	PTCPConnect temp = NULL;
	WaitForSingleObject(List->MUTEX, INFINITE);
	{
		__try
		{
			temp = List->LIST;
			for (int i = 0; i < index; i++)
			{
				if (temp == NULL)
					__leave;
				temp = temp->NextIP;
			}

		}
		__finally {
			ReleaseMutex(List->MUTEX);
		}
	}
	return temp;

}


int AddNewIP(PTCPList Connect, PTCPConnect ConnectSock)
{
	WaitForSingleObject(Connect->MUTEX, INFINITE);
	{
		__try {
			PTCPConnect temp=Connect->LIST;
			if (temp == NULL)
			{
				Connect->LIST = ConnectSock;
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

PTCPConnect RemoveIP(PTCPList Connect, PTCPConnect ConnectSock)
{
	int res = 0;
	int i = -1;
	WaitForSingleObject(Connect->MUTEX, INFINITE);
	{
		__try {
			
			PTCPConnect temp = Connect->LIST,tmp=temp;
			
			if (temp->TCPThread == ConnectSock->TCPThread)
			{
				i++;
				__leave;
			}
			


			while (temp->TCPThread != ConnectSock->TCPThread)
			{
				tmp = temp;
				temp = temp->NextIP;
				i++;
			}
			tmp->NextIP = temp->NextIP;
		}
		__finally {

			if (i == 0)
				Connect->LIST = NULL;
			else
			{
				PTCPConnect temp = Connect->LIST;
				for (int j = -1; j < i; j++)
				{
					Connect->LIST = Connect->LIST->NextIP;
				}
				Connect->LIST = NULL;
				Connect->LIST = temp;
			}
			ReleaseMutex(Connect->MUTEX);
		}
	}
	return res;
}

PTCPList CreateList() 
{
	PTCPList TCPList = malloc(sizeof(_TCPList));
	TCPList->MUTEX = CreateMutex(NULL, FALSE, NULL);
	TCPList->LIST = NULL;

	return TCPList;
}


PTCPList GetUsers(PTCPList Source) {
	HANDLE Heap = HeapCreate(HEAP_GENERATE_EXCEPTIONS, 0x0f0000, 0);
	PTCPList TCPList = HeapAlloc(Heap, HEAP_ZERO_MEMORY, sizeof(_TCPList));
	PTCPConnect tempList = TCPList->LIST= HeapAlloc(Heap, HEAP_ZERO_MEMORY, sizeof(TCPConnect));
	TCPList->Heap = Heap;
	WaitForSingleObject(Source->MUTEX, INFINITE);
	{
		__try {

			PTCPConnect temp = Source->LIST;
			if (temp == NULL)
				__leave;

			*tempList = *temp;

			while (temp->NextIP != NULL)
			{

				tempList->NextIP = HeapAlloc(Heap, HEAP_ZERO_MEMORY, sizeof(TCPConnect));
				*tempList->NextIP = *temp->NextIP;
				tempList = tempList->NextIP;
				temp = temp->NextIP;
			}

		}
		__finally {
			ReleaseMutex(Source->MUTEX);
		}
	}
	return TCPList;
}

int RemoveUsers(PTCPList List) {
	HANDLE heap = List->Heap;
	PTCPConnect tmp,temp = List->LIST;
	while (temp != NULL)
	{
		tmp = temp->NextIP;
		HeapFree(heap,0,temp);
		temp = tmp;
	}
	HeapFree(heap, 0, List);
	HeapDestroy(heap);
}

PTCPConnect GetUserOFIndex(PTCPList List,int index)
{
	PTCPConnect tmp = List->LIST;
	if (tmp->TCPThread == 0)
		return NULL;
	for (int i = 0; i < index; i++)
	{
		tmp = tmp->NextIP;
		if (tmp == NULL)
			return NULL;
	}
	return tmp;
}