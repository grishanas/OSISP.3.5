#include "PointInternet.h"
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")


// для начала открываем UDPListen отправляем сообщение мультикаст сообщение всем и начинаем слушать все IP
// но раньше надо открыть TCP соединение на прослушивания. как только приходит udp сообщение на наш порт надо пародить новый поток с TCPListener 
// для прослушки нового IP. при создании TCPListener-а надо уже иметь методы синхронизации
//
//


DWORD WINAPI InternetSendMessage(PTCTConnect TCPConnection,char *DGram,int size)
{
	int size = sizeof(struct sockaddr_in);
	if (sendto(TCPConnection->TCPSocket, DGram, size, 0, TCPConnection->TCPSockAdrr, size)==-1)
	{
		return -1;
	}
	return 1;
	
}


DWORD WINAPI TCPListener(PTCTConnect TCPConnection)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return;
	}

	if ((TCPConnection->TCPSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		return;
	}
	struct sockaddr_in server;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(TCP_PORT);
	server.sin_family = AF_INET;
	if (bind(TCPConnection->TCPSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		return;
	}
	char buffer[256];
	int size = sizeof(server);
	while (1)
	{
		memset(buffer, '\0', 256);
		while (recvfrom(TCPConnection->TCPSocket, buffer, POINTINTERNET_BUFFERSIZE, 0, TCPConnection->TCPSockAdrr, &size) == -1);
		switch (buffer[0])
		{
		case(1): 
		{

		}
		default:
			break;
		}
	}


}

DWORD WINAPI UDPListenerFunction(PPointInternet Internet)
{
	// это потоко безопасно

	SOCKET UDPListenerSocket;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return;
	}

	if ((UDPListenerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		return;
		// тут код обработки ошибки, надо посылать скорее всего сообщение с ошибкой рабочему окну, либо return отрицательные знач
	}
	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);
	if (setsockopt(UDPListenerSocket, SOL_SOCKET, SO_BROADCAST, (char*)&bOptVal, bOptLen)==SOCKET_ERROR) {
		return;
	}

	struct sockaddr_in server;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(UDPListener_PORT);
	server.sin_family = AF_INET;
	if (bind(UDPListenerSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		return;
	}
	int sendip = 0;
	char buffer[256];
	int InputLength = 0;
	
	InputLength = 0;
	memset(buffer, '\0', 256);
	if (!sendip)
	{
		struct sockaddr_in Send_other;
		Send_other.sin_family = AF_INET;
		Send_other.sin_port = htons(UDPListener_PORT);
		Send_other.sin_addr.s_addr = INADDR_BROADCAST;

		char mess[10] = { 1 };

		int length = sendto(UDPListenerSocket, mess, 10,0, &Send_other, sizeof(Send_other));
		int er= WSAGetLastError();
		sendip = 0;
	}
	while (1)
	{
		
		struct sockaddr_in* SocketInput = HeapAlloc(Internet->heap, 0, sizeof(server));
		int SocketLength = sizeof(SocketInput);
		while(recvfrom(UDPListenerSocket, buffer, POINTINTERNET_BUFFERSIZE, 0, (struct sockaddr*)&SocketInput,&SocketLength)==-1);
		switch (buffer[0])
		{
		case(1): 
			{
			PTCTConnect TCPClient = HeapAlloc(Internet->heap, HEAP_ZERO_MEMORY, sizeof(TCPConnect));
			TCPClient->TCPSockAdrr = SocketInput;
			TCPClient->TCPThread = CreateThread(NULL, 0, &TCPListener, TCPClient, 0, NULL);


			// нужно ли вообще заносить это сообщение. Лучше наверное сделать счетчик поток в структурке

			/*PMessage list = HeapAlloc(Internet->heap, HEAP_ZERO_MEMORY, sizeof(struct _spis));
			list->buffer[0] = (char)SocketInput->sin_addr.S_un.S_addr;
			list->Lenght = sizeof(SocketInput->sin_addr.S_un.S_addr);
			Enqueue(Internet->Queue, list);*/
			}
		default:
			break;
		}

	}
	closesocket(UDPListenerSocket);
	WSACleanup();
}


PPointInternet CreateInternetConnection(HWND hwnd)
{
	HANDLE heap = HeapCreate(HEAP_GENERATE_EXCEPTIONS, 0x0f0000, 0);
	PPointInternet Internet = HeapAlloc(heap, HEAP_ZERO_MEMORY, sizeof(PointInternet));
	Internet->heap = heap;
	Internet->hwnd = hwnd;
	Internet->ParentThread = GetCurrentThread();
	Internet->TCPList = CreateList();
	PQueue Queue = CreateQueue();

	Internet->UDPListenerThread = CreateThread(NULL, 0, &UDPListenerFunction, Internet, 0, NULL);

	return Internet;

}