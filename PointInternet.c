#include "PointInternet.h"
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Main.h"
#include <iphlpapi.h>

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")


// дл€ начала открываем UDPListen отправл€ем сообщение мультикаст сообщение всем и начинаем слушать все IP
// но раньше надо открыть TCP соединение на прослушивани€. как только приходит udp сообщение на наш порт надо пародить новый поток с TCPListener 
// дл€ прослушки нового IP. при создании TCPListener-а надо уже иметь методы синхронизации
//
//


// дл€ подключени€ используетс€ следующее соглашение вызова. ѕри создании порта прослушки
//
//


int CopyStringTCHAR(TCHAR* dest, TCHAR* source)
{
	if (source == NULL)
		return 0;
	int res = 0;
	while (*source != '\0')
	{
		*dest = *source;
		source++;
		dest ++;
		res += sizeof(TCHAR);
	}
	return res;
}



DWORD WINAPI SendAllUsersMessage(PPointInternet Internet,char *Message,int size)
{
	PTCPList List = Internet->TCPList;
	PTCPConnect TCP;
	int i = 0;
	while (TCP = GetIP(List, i))
	{
		InternetSendMessage(TCP, Message, size);
	}

}

DWORD WINAPI UDPSendName(PPointInternet Internet)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return;
	}
	SOCKET UDP;
	if ((UDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		return;
		// тут код обработки ошибки, надо посылать скорее всего сообщение с ошибкой рабочему окну, либо return отрицательные знач
	}
	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);
	if (setsockopt(UDP, SOL_SOCKET, SO_BROADCAST, (char*)&bOptVal, bOptLen) == SOCKET_ERROR) {
		return;
	}

	struct sockaddr_in server;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = UDPSender_PORT;
	server.sin_family = AF_INET;
	if (bind(UDP, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		return;
	}
	struct sockaddr_in Send_other;
	Send_other.sin_family = AF_INET;
	Send_other.sin_port = UDPListener_PORT;
	Send_other.sin_addr.s_addr = INADDR_BROADCAST;
	char mess[POINTINTERNET_BUFFERSIZE];
	mess[0] = 1;
	int res=CopyStringTCHAR(mess + 1, Internet->PlayerName);

	int length = sendto(UDP, mess, res+1,0, &Send_other, sizeof(Send_other));
	int er = WSAGetLastError();
	closesocket(UDP);

}

DWORD WINAPI InternetSendMessage(PTCPConnect TCPConnection,char *DGram,int size)
{
	int Size = sizeof(struct sockaddr_in);
	//connect(TCPConnection->TCPSocket, (struct sockaddr*)TCPConnection->TCPSockAdrr, Size);
	int err = GetLastError();
	if (send(TCPConnection->TCPSocket,DGram,size,0)==-1);
	{
		err = GetLastError();
		return -1;
	}
	return 1;
	
}


DWORD WINAPI TCPConnection(PTCPConnect Connect)
{
	char buffer[256];
	memset(buffer, '\0', 256);

	buffer[0] = 1;
	int size = CopyStringTCHAR(buffer + 1, Connect->MyName);

	send(Connect->TCPSocket, buffer, size + 1, 0);

	while (1)
	{
		memset(buffer, '\0', 256);
		int size=recv(Connect->TCPSocket, buffer, 256, 0);
		if (size < 0)
		{
			PPointInternet Internet = Connect->Internet;
			RemoveIP(Internet->TCPList, Connect);
			
			
			closesocket(Connect->TCPSocket);
			HeapFree(Internet->heap, 0, Connect->MyName);
			HeapFree(Internet->heap, 0, Connect->PlayerName);
			HeapFree(Internet->heap, 0, Connect->TCPSockAdrr);
			HeapFree(Internet->heap, 0, Connect);
			SendMessage(Internet->hwnd, MESSAGE_UPDATELIST, Internet->TCPList, 0);
			return;
		}
		switch (buffer[0])
		{
			// прием имени пользовател€
		case(1):
		{
			PPointInternet Internet = Connect->Internet;
			Connect->PlayerName = HeapAlloc(Internet->heap, HEAP_ZERO_MEMORY, sizeof(wchar_t) * MaxNameSize);
			CopyStringTCHAR(Connect->PlayerName, buffer + 1);
			AddNewIP(Internet->TCPList, Connect);
			SendMessage(Internet->hwnd, MESSAGE_UPDATELIST, Internet->TCPList, 0);

			break;
		}
		// —ообщение о начале игры
		case('2'):
		{
			switch (buffer[1])
			{
			// отказ в подключении
			case '0':{
				PPointInternet Internet = Connect->Internet;
				SendMessage(Internet->hwnd, MESSAGE_CONNECTION_FAILD, Connect, 0);

				break;
			}
				// «апрос к данному устройству на начало игры
			case('1'): {
				PPointInternet Internet = Connect->Internet;
				SendMessage(Internet->hwnd, MESSAGE_STARTGAME_IN, Connect, 0);

				break;
			}
				   // ќтвет от пользовател€ на подключение
			case '2': {
				PPointInternet Internet = Connect->Internet;
				SendMessage(Internet->hwnd, MESSAGE_CONNECTION_SUCCESS, Connect, 0);
				break;
			}
			default:
				break;
			}
			break;
		}
		case 3: {
			int Point = buffer[2] << 16;
			Point += buffer[4];
			PPointInternet Internet = Connect->Internet;
			SendMessage(Internet->hwnd, MESSAGE_CONNECTION_ADDPOINT, Point, 0);
			break;
		}
		// окончание игры
		case 4: 
		{
			break;
		}
		default:
			break;
		}
	}
}



// ѕорождение нового соединени€ при подключении с помощью UDP
// ћожно сделать потом просто вызов TCPConnection
// 
DWORD WINAPI TCPListener(PTCPConnect TCPconnection)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return;
	}


	if ((TCPconnection->TCPSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		return;
	}
	struct sockaddr_in server;
	server.sin_addr = TCPconnection->TCPSockAdrr->sin_addr;
	server.sin_family = AF_INET;
	server.sin_port = TCP_PORT;
	if (connect(TCPconnection->TCPSocket, (struct sockaddr*)&server, sizeof(server)) < 0)
		return -1;
	
	TCPConnection(TCPconnection);


}



// тут прием TCP сообщений от любых IP
// простое пораждение потока на каждый новый сокет
// Ќ”∆Ќќ сделать проверку на существование таких сокетов/IP
//
DWORD WINAPI NormTCPListener(PPointInternet Internet)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return;
	}
	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in NewUser;
	NewUser.sin_addr.s_addr = INADDR_ANY;
	NewUser.sin_port = (TCP_PORT);
	NewUser.sin_family = AF_INET;
	if (bind(ServerSocket, (struct sockaddr*)&NewUser, sizeof(NewUser)) == SOCKET_ERROR)
	{
		return;
	}
	
	listen(ServerSocket, SOMAXCONN);
	
	while (1)
	{
		
		SOCKET recive = accept(ServerSocket, NULL, NULL);
		
		PTCPConnect TCPClient = HeapAlloc(Internet->heap, HEAP_ZERO_MEMORY, sizeof(TCPConnect));
		TCPClient->MyName = HeapAlloc(Internet->heap, HEAP_ZERO_MEMORY, sizeof(WCHAR)*MaxNameSize);
		CopyStringTCHAR(TCPClient->MyName, Internet->PlayerName);
		TCPClient->TCPSocket=recive;
		TCPClient->Internet = Internet;
		TCPClient->TCPThread = CreateThread(NULL, 0, &TCPConnection, TCPClient, 0, NULL);

	}
}

BOOL ISMYIP(struct sockaddr_in* SockAddr)
{
	struct in_addr sa;

	DWORD rv, size;
	PIP_ADAPTER_ADDRESSES adapter_addresses, aa;
	rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size);
	if (rv != ERROR_BUFFER_OVERFLOW) {
		return FALSE;
	}
	adapter_addresses = (PIP_ADAPTER_ADDRESSES)malloc(size);
	rv = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, adapter_addresses, &size);
	if (rv != ERROR_SUCCESS) {
		return FALSE;
	}
	char Buffer[256];
	for (aa = adapter_addresses; aa != NULL; aa = aa->Next) {
		memset(Buffer, 0, 256);
		getnameinfo(aa->FirstUnicastAddress->Address.lpSockaddr, aa->FirstUnicastAddress->Address.iSockaddrLength, Buffer, 256, 0, 0, NI_NUMERICHOST);
		int r;
		if (Buffer[0] == 0)
			continue;
		r= inet_pton(AF_INET, Buffer, &sa);
		
		int er = GetLastError();

		if (sa.S_un.S_addr == SockAddr->sin_addr.s_addr)
		{
			return TRUE;
		}
	}
	return FALSE;
}



// тут прием только udp сообщений
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



	struct sockaddr_in server;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = UDPListener_PORT;
	server.sin_family = AF_INET;
	if (bind(UDPListenerSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		return;
	}
	int sendip = 0;
	char buffer[POINTINTERNET_BUFFERSIZE];
	int InputLength = 0;
	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);
	if (setsockopt(UDPListenerSocket, SOL_SOCKET, SO_BROADCAST, (char*)&bOptVal, bOptLen) == SOCKET_ERROR) {
		return;
	}
	
	InputLength = 0;
	memset(buffer, '\0', POINTINTERNET_BUFFERSIZE);
	while (1)
	{
		// удалить ненужные байты в приход€щем сообщении
		struct sockaddr_in* SocketInput = HeapAlloc(Internet->heap, HEAP_ZERO_MEMORY, sizeof(server));
		int SocketLength = sizeof(server);
		int err=recvfrom(UDPListenerSocket, buffer, POINTINTERNET_BUFFERSIZE, 0, SocketInput, &SocketLength);
		if(err== SOCKET_ERROR)
			err= WSAGetLastError();	
		if (ISMYIP(SocketInput))
			continue;
		switch (buffer[0])
		{
		case(1): 
			{
			PTCPConnect TCPClient = HeapAlloc(Internet->heap, HEAP_ZERO_MEMORY, sizeof(TCPConnect));
			TCPClient->TCPSockAdrr= SocketInput;
			TCPClient->MyName = HeapAlloc(Internet->heap, HEAP_ZERO_MEMORY, sizeof(wchar_t) * (MaxNameSize));
			CopyStringTCHAR(TCPClient->MyName,Internet->PlayerName);
			TCPClient->Internet = Internet;
			TCPClient->TCPThread = CreateThread(NULL, 0, &TCPListener, TCPClient, 0, NULL);

			}
		default:
			break;
		}

	}
	closesocket(UDPListenerSocket);
	WSACleanup();
}


PPointInternet CreateInternetConnection(HWND hwnd,wchar_t* Name)
{
	HANDLE heap = HeapCreate(HEAP_GENERATE_EXCEPTIONS, 0x0f0000, 0);
	PPointInternet Internet = HeapAlloc(heap, HEAP_ZERO_MEMORY, sizeof(PointInternet));
	Internet->PlayerName = HeapAlloc(heap, HEAP_ZERO_MEMORY, sizeof(WCHAR) * MaxNameSize);
	CopyStringTCHAR(Internet->PlayerName, Name);
	Internet->heap = heap;
	Internet->hwnd = hwnd;
	Internet->ParentThread = GetCurrentThread();
	Internet->TCPList = CreateList();
	PQueue Queue = CreateQueue();

	Internet->UDPListenerThread = CreateThread(NULL, 0, &UDPListenerFunction, Internet, 0, NULL);
	Internet->TCPListenerThread = CreateThread(NULL, 0, &NormTCPListener, Internet, 0, NULL);

	return Internet;

}