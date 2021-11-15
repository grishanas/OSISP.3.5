#include "PointInternet.h"


#include <Windows.h>
#include <windowsx.h>
#include "PlayGround.h"


#define	MINIMAL_WIDTH_WINDOW 700
#define	MINIMAL_HEIGHT_WINDOW 600

const wchar_t* const WindowsClass = L"PointGame";

LRESULT WINAPI MainWindowProcc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case(WM_CREATE): 
		{
		PplayGround Play = GetWindowLong(hwnd, 0);
		if (!Play)
		{
			Play = CreatePlayGround(hwnd);
			SetWindowLong(hwnd, 0, Play);
			InvalidateRect(hwnd, NULL, FALSE);
		}
		//тут надо создавать еще одно окошко, со списком всех доступных игроков+ надо пораждать еще один поток для чтения потока
		// еще надо задефайнить сообщение для отправки винде
		break;
		}
	case(WM_PAINT):
		{
		PplayGround Play = GetWindowLong(hwnd, 0);
		PlayGroundRedraw(Play);
			
		break;
		}
	case(WM_LBUTTONDOWN): 
		{

		// тестовый код
		CreateInternetConnection(hwnd);
		//



		PplayGround Play = GetWindowLong(hwnd, 0);
		if (!Play->EndGame)
		{
			PlayGroundAddPoint(Play, lParam);
			InvalidateRect(hwnd, NULL, FALSE);
		}

		break;
		}
	case(WM_SIZING): 
	{
		PplayGround Play = GetWindowLong(hwnd, 0);
		PlayGroundResize(Play);
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	}
	case(WM_DESTROY): 
		{
		PostQuitMessage(0);
		break;
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
	wcex.cbWndExtra = 4;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(0, IDC_HAND);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WindowsClass;
	wcex.hIconSm = 0;
	RegisterClassEx(&wcex);

	Mainhwnd = CreateWindowEx(0, WindowsClass, L"Игра точки", (WS_OVERLAPPEDWINDOW | WS_VISIBLE), 0, 0, MINIMAL_WIDTH_WINDOW, MINIMAL_HEIGHT_WINDOW, 0, 0, hInstance, NULL);

	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;

}