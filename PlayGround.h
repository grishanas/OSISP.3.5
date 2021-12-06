#pragma once
#include <Windows.h>


#define MINIMALWIDTH 19
#define MINIMALHEIGHT 32

typedef struct _spisokPointArray {
	PPOINT ArrOfPoint;
	int CountPoint;
	int color;
	struct _spisokPointArray* NextPoint;
}SPISOK, * PSPISOK;

typedef struct _point {
	POINT point;
	int color;
	int brush;
	int  view;
}_POINT;

typedef struct _plaeyr {
	int color;
	HBRUSH red;
	HBRUSH blue;
} PLAEYR;

typedef struct _playGround {

	HANDLE Heap;
	int rad;
	int WidthPoint, HeightPoint;
	int CountBlue;
	int CountRed;
	int EndGame;
	int Redrow;
	HWND hwndSelf;
	RECT playGroundsize;
	PLAEYR player;
	_POINT PointArr[MINIMALWIDTH* MINIMALHEIGHT];
	PSPISOK spis;

	HDC hdcBack;
	HBITMAP hbmBack;

	char YouStep;
} *PplayGround;

PplayGround CreatePlayGround(HWND hwnd);
int PlayGroundAddPoint(PplayGround Play, LPARAM lParam);
int PlayGroundAddPointInt(PplayGround Play, int Point);
int PlayGroundRedraw(PplayGround Play);
int PlayGroundResize(PplayGround Play);


