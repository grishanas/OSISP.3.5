#include "PointInternet.h"
#include <Windows.h>
#include <windowsx.h>
#include "PlayGround.h"
#include <CommCtrl.h>
#include "Main.h"
#include "TCPIPList.h"
#include "stdlib.h"


typedef struct MainWindow {
	HWND HListBox;

} *PMainWindows;

const wchar_t* const WindowsClass = L"PointGame";
const wchar_t* const Child = L"Child";

PPointInternet Internet;

static HWND HListBox;
static HWND HEdit;
static HWND HButton;
static HWND HButtonStartGame;
static char BufferName[16];

LRESULT CALLBACK ChildProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg)
	{
		case(WM_CREATE) :
		{
			HListBox=CreateWindow(WC_LISTBOX, NULL,
				WS_CHILD | WS_VISIBLE | LBS_STANDARD |
				LBS_WANTKEYBOARDINPUT,
				0, 0, 200, 200,
				hwnd, (HMENU)ID_LIST, NULL, NULL);
			HEdit = CreateWindow(WC_EDITW, NULL, WS_CHILD | WS_VISIBLE|WS_BORDER, 210, 0, 120, 20, hwnd, NULL, NULL, NULL);
			HButton = CreateWindow(WC_BUTTON, L"–егистраци€", WS_CHILD | WS_VISIBLE, 210, 20, 120, 20, hwnd, BUTTON_ED_ID, NULL, NULL);
			HButtonStartGame = CreateWindow(WC_BUTTON, L"Ќачать игру", WS_CHILD | WS_VISIBLE, 210, 40, 120, 20, hwnd, BUTTON_START_GAME, NULL, NULL);
			Button_Enable(HButtonStartGame, FALSE);

			memset(BufferName, 0, sizeof(BufferName));
			break;
		}
		case(WM_CLOSE):
		case(WM_DESTROY): 
		{
			ShowWindow(hwnd, SW_HIDE);
			return 0;
		}
		case(WM_KEYDOWN): 
		{


			break;
		}
		case(WM_COMMAND):
		{
			switch(wParam)
			{
			case BUTTON_START_GAME: {
				int ChooseItem = SendMessage(HListBox, LB_GETCURSEL, 0, 0);
				if (ChooseItem >= 0)
				{
					HWND parent = GetWindowLong(hwnd, 0);
					SendMessage(parent, MESSAGE_STARTGAME_OUT, ChooseItem, 0);
					SendMessage(hwnd, WM_DESTROY, 0, 0);
					
				}

				break;
			}
			case(EDIT_ID): {

				break;
			}
			case(BUTTON_ED_ID): 
			{
				
				TCHAR* Name=malloc(sizeof(TCHAR)*MaxNameSize);
				memset(Name, "\0", sizeof(TCHAR) * MaxNameSize);
				//GetDlgItemText();
				Edit_GetText(HEdit, Name, sizeof(TCHAR) * MaxNameSize);
				//GetDlgItemText(hwnd, EDIT_ID, Name, 16);
				int err = GetLastError();
				HWND parent = GetWindowLong(hwnd, 0);
				SendMessage(parent, MESSAGE_NEWNAME, Name,0);
				Edit_Enable(HEdit, FALSE);
				Button_Enable(HButton, FALSE);
				Button_Enable(HButtonStartGame, TRUE);
				break;
			}
				default:
					break;
			}
		break;
		}
		
		case WM_SIZING:
		{

			break;
		}
		case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = 350;
			lpMMI->ptMinTrackSize.y = 235;

			lpMMI->ptMaxTrackSize.x = 350;
			lpMMI->ptMaxTrackSize.y = 235;

			break;
		}
		case MESSAGE_UPDATELIST: {
			SendMessage(HListBox, LB_RESETCONTENT, 0, 0);
			PTCPList UserList = GetUsers(wParam);
			PTCPConnect TCP;
			int i = 0;
			while (TCP= GetUserOFIndex(UserList, i))
			{
				SendMessage(HListBox, LB_ADDSTRING, 0, (TCHAR*)TCP->PlayerName);
				i++;
			}
			RemoveUsers(UserList);

		}

	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

HWND WINAPI CreateChildWindow(HWND hwndParent) {

	WNDCLASSEX w;
	memset(&w, 0, sizeof(WNDCLASSEX));
	
	w.cbSize = sizeof(WNDCLASSEX);
	w.cbWndExtra = 4;
	w.cbClsExtra = 0;
	w.style = 0;
	w.lpfnWndProc = ChildProc;
	w.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	int err = GetLastError();

	w.lpszMenuName = NULL;
	w.lpszClassName = L"ChildWClass";
	w.hCursor = LoadCursor(0, IDC_HAND);

	RegisterClassEx(&w);
	HWND child;
	child = CreateWindowEx(0, L"ChildWClass", (LPCTSTR)NULL,
		WS_OVERLAPPEDWINDOW| WS_MINIMIZEBOX, 0, 0,
		400, 235, NULL, NULL, NULL, NULL);
	SetWindowLong(child, 0, hwndParent);
	ShowWindow(child, SW_SHOWDEFAULT);
	UpdateWindow(child);
	return child;
}


LRESULT WINAPI MainWindowProcc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	

	switch (msg)
	{
	case(WM_CREATE): 
		{
		PplayGround Play = GetWindowLong(hwnd, 0);
		if (!Play)
		{			

			HWND child= CreateChildWindow(hwnd);
			Play = CreatePlayGround(hwnd);
			
			SetWindowLong(hwnd, 0, Play);
			SetWindowLong(hwnd, 4, child);

			InvalidateRect(hwnd, NULL, FALSE);

		}
		SetFocus(GetWindowLong(hwnd, 4));
		//тут надо создавать еще одно окошко, со списком всех доступных игроков+ надо пораждать еще один поток дл€ чтени€ потока
		// еще надо задефайнить сообщение дл€ отправки винде
		break;
		}
	case(WM_PAINT):
		{
		PplayGround Play = GetWindowLong(hwnd, 0);
		PlayGroundRedraw(Play);
		int res=PlayGroundEndGame(Play);
		if (res == 1)
		{
			PPointInternet Internet = GetWindowLong(hwnd, 8);
			InternetSendMessage(Internet->ChooseUser, '3', 1);
			Internet->ChooseUser == NULL;

		}
		break;
		}
	case WM_COMMAND:
		switch (wParam)
		{
		case MAIN_WINDOW_BUTTON_START:
		{
			HWND ChildHandle = GetWindowLong(hwnd, 4);
			ShowWindow(ChildHandle, SW_SHOW);
			int er = GetLastError();
			UpdateWindow(ChildHandle);
			SetFocus(ChildHandle);
			break;
		}
		case MAIN_WINDOW_LEAVE_GAME: {

			HWND ChildHandle = GetWindowLong(hwnd, 4);
			EnableWindow(hwnd, TRUE);
			EnableWindow(ChildHandle, TRUE);
			ShowWindow(ChildHandle, SW_SHOW);
			UpdateWindow(ChildHandle);
			PlayGroundClean(GetWindowLong(hwnd, 0));
			PPointInternet Internet = GetWindowLong(hwnd, 8);
			if ((Internet != NULL) && (Internet->ChooseUser != NULL))
			{
				InternetSendMessage(Internet->ChooseUser, "41", 2);
			}
			break;
		}
		}
		break;
	
	// ќбработка сообщений от нажатий пользовател€ на этом компьютере 
	// 
	//
	case(WM_LBUTTONDOWN): 
		{
		
		InvalidateRect(hwnd, NULL, FALSE);
		PPointInternet Internet = GetWindowLong(hwnd, 8);
		if (Internet == NULL)
			break;
		if (Internet->ChooseUser == NULL)
			break;
		PplayGround Play = GetWindowLong(hwnd, 0);
		if (Play->YouStep == 0)
			break;

		int res;
		if (!Play->EndGame)
		{
			res=PlayGroundAddPoint(Play, lParam);
			InvalidateRect(hwnd, NULL, FALSE);
		}

		char DGram[16] = { 0 };
		DGram[0] = 3;
		DGram[1] = res >> 24;
		DGram[2] = res >> 16;
		DGram[3] = res >> 8;
		DGram[4] = res >> 0;

		InternetSendMessage(Internet->ChooseUser, DGram, 5);

		break;
		}
	case(WM_SIZING): 
	{
		/*PplayGround Play = GetWindowLong(hwnd, 0);
		PlayGroundResize(Play);
		InvalidateRect(hwnd, NULL, FALSE);*/
		break;
	}

	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = MINIMAL_WIDTH_WINDOW;
		lpMMI->ptMinTrackSize.y = MINIMAL_HEIGHT_WINDOW;

		lpMMI->ptMaxTrackSize.x = MINIMAL_WIDTH_WINDOW;
		lpMMI->ptMaxTrackSize.y = MINIMAL_HEIGHT_WINDOW;

		break;
	}
	case WM_SETFOCUS: {
		
		break;
	}
	case(WM_DESTROY): 
		{
		PostQuitMessage(0);
		break;
		}
	case(WM_KEYDOWN): {
		return 0;
		break;
	}

	case MESSAGE_CONNECTION_ADDPOINT: {

		PplayGround Play = GetWindowLong(hwnd, 0);
		PlayGroundAddPointInt(Play, wParam);
		InvalidateRect(hwnd, NULL, FALSE);

		break;
	}
	case MESSAGE_NEWNAME: {
		wchar_t* Name = wParam;
		PPointInternet Internet = CreateInternetConnection(hwnd, Name);
		SetWindowLong(hwnd, 8, Internet);
		UDPSendName(GetWindowLong(hwnd, 8));
		break;
	}
	case MESSAGE_UPDATELIST: {
		HWND child = GetWindowLong(hwnd, 4);
		SendMessage(child, MESSAGE_UPDATELIST, wParam, lParam);
		break;
	}

	/// <summary>
	/// € хз, что здесь придумать,надо как-нибудь сделать нормальное ожидание отклика
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="msg"></param>
	/// <param name="wParam"></param>
	/// <param name="lParam"></param>
	/// <returns></returns>
	case MESSAGE_STARTGAME_OUT: {

		PPointInternet Internet = GetWindowLong(hwnd, 8);
		PTCPConnect Connection = GetIP(Internet->TCPList, wParam);
		Internet->ChooseUser = Connection;
		InternetSendMessage(Connection, "21", 2);
		PplayGround Play = GetWindowLong(hwnd, 0);
		Play->YouStep = 1;
		EnableWindow(hwnd, FALSE);
		EnableWindow(GetWindowLong(hwnd, 4), FALSE);
		int res = MessageBox(hwnd, L"ќжидание ответа пользовател€", L"ќжидание ответа пользовател€", MB_OKCANCEL | MB_ICONINFORMATION);
		if (res == IDOK)
		{
			break;

		}
		else {
			InternetSendMessage(Connection, "20", 2);
			Internet->ChooseUser = NULL;
			Play->YouStep = 0;
		}


		EnableWindow(hwnd, TRUE);
		EnableWindow(GetWindowLong(hwnd, 4), TRUE);
		break;
	}
	// подтверждение начала игры
	// wParam- указатель на структуру PTCPConnection - противник 
	case MESSAGE_CONNECTION_SUCCESS: {

		PPointInternet Internet = GetWindowLong(hwnd, 8);
		MessageBox(hwnd, L"»гра начата", L"игра начата", MB_OK | MB_ICONINFORMATION);
		Internet->ChooseUser = wParam;
		EnableWindow(hwnd, TRUE);
		EnableWindow(GetWindowLong(hwnd, 4), TRUE);
		SendMessageW(GetWindowLong(hwnd, 4), WM_CLOSE, 0, 0);

		break;
	}
	// «акрытие соединение с пользователем
	// wParam- указатель на структуру PTCPConnection - противник 
	// 
	case MESSAGE_CONNECTION_CLOSE: {

		PPointInternet Internet = GetWindowLong(hwnd, 8);
		Internet->ChooseUser = NULL;
		EnableWindow(hwnd, TRUE);
		EnableWindow(GetWindowLong(hwnd, 4), TRUE);
		break;
	}

	case MESSAGE_CONNECTION_END: {
		PPointInternet Internet = GetWindowLong(hwnd, 8);
		Internet->ChooseUser = NULL;
		break;
	}

	// MESSAGE_STARTGAME_IN-код ообщени€ msg
	// wParam- структура типа PTCPConnection - указатель на соединивщегос€ человека
	//
	case MESSAGE_STARTGAME_IN:
	{
		PTCPConnect Connection = wParam;
		PPointInternet Internet = GetWindowLong(hwnd, 8);
		if (Internet->ChooseUser != NULL)
			InternetSendMessage(Connection, "20", 2);

		EnableWindow(hwnd, FALSE);
		EnableWindow(GetWindowLong(hwnd, 4), FALSE);

		int res = MessageBox(hwnd, Connection->PlayerName, L"ѕрин€ть приглашение?", MB_YESNO | MB_ICONINFORMATION);
		if (res == IDYES)
		{
			PplayGround Play = GetWindowLong(hwnd, 0);
			Play->YouStep = 0;
			Internet->ChooseUser = Connection;
			InternetSendMessage(Connection, "22", 2);
			SendMessage(hwnd, MESSAGE_CONNECTION_SUCCESS, wParam, 0);
		}
		else
		{
			InternetSendMessage(Connection, "20", 2);
			EnableWindow(hwnd, TRUE);
			EnableWindow(GetWindowLong(hwnd, 4), TRUE);
		}
		break;
	}
	case MESSAGE_CONNECTION_FAILD:
	{
		PPointInternet Internet = GetWindowLong(hwnd, 8);
		PTCPConnect Connection = (PTCPConnect)wParam;
		if (Internet->ChooseUser!=NULL)
		if (Internet->ChooseUser->TCPThread == Connection->TCPThread)
		{
			Internet->ChooseUser = NULL;
		}
		EnableWindow(hwnd, TRUE);
		EnableWindow(GetWindowLong(hwnd, 4), TRUE);

		break;
	}
	case MESSAGE_CONNECTION_LEAVE: {
		MessageBox(hwnd, L"ѕользователь покинул игру", L"ќтвет пользовател€", MB_OK | MB_ICONINFORMATION);
		PPointInternet Internet = GetWindowLong(hwnd, 8);
		Internet->ChooseUser = NULL;
		PplayGround Play= GetWindowLong(hwnd, 0);
		PlayGroundClean(Play);
		InvalidateRect(hwnd, NULL, FALSE);
	}
	}
	return DefWindowProc(hwnd,msg,wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	WNDCLASSEX wcex;
	MSG msg;
	HWND Mainhwnd;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = MainWindowProcc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 12;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(0, IDC_HAND);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WindowsClass;
	wcex.hIconSm = 0;
	RegisterClassEx(&wcex);

	Mainhwnd = CreateWindowEx(0, WindowsClass, L"»гра точки", (WS_OVERLAPPEDWINDOW | WS_VISIBLE), 0, 0, MINIMAL_WIDTH_WINDOW, MINIMAL_HEIGHT_WINDOW, 0, 0, hInstance, NULL);

	HWND HBStart = CreateWindowEx(0, L"BUTTON", L"—писок игроков", (WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON), MINIMAL_WIDTH_WINDOW - RIGHT_PLACE, 0, 120, 30, Mainhwnd, (HMENU)MAIN_WINDOW_BUTTON_START, 0, NULL);
	HWND HBLeaveGame = CreateWindowEx(0, L"BUTTON", L"ѕокинуть игру", (WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON), MINIMAL_WIDTH_WINDOW - RIGHT_PLACE, 40, 120, 30, Mainhwnd, (HMENU)MAIN_WINDOW_LEAVE_GAME, 0, NULL);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Here need to send a disconnect message to all connected users
	return msg.wParam;

}