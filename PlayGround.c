#include "PlayGround.h"
#include <Windows.h>
#include <windowsx.h>
#include "Main.h"

#define BLUE 1
#define RED 2


#define CLEANTEXT "                                           "
#define STEPRED "Ходит красный"
#define STEPBLUE "Ходит синий" 

#define REDCOUNT "Красные точки -"
#define	BLUECOUNT "Синие точки -"

#define RIGHTCOLUMN RIGHT_PLACE

#pragma warning(disable : 4996)


int PlayGroundClean(PplayGround Play)
{
	Play->CountBlue = 0;
	Play->CountRed = 0;
	Play->EndGame = 0;
	for (int i = 0; i < Play->WidthPoint; i++)
	{
		for (int j = 0; j < Play->HeightPoint; j++)
		{
			Play->PointArr[i * Play->HeightPoint + j].brush = 0;
			Play->PointArr[i * Play->HeightPoint + j].color = 0;
			Play->PointArr[i * Play->HeightPoint + j].view = 0;

		}
	}
	Play->YouStep = 0;
	PlayGroundBackDraw(Play);
}

PplayGround CreatePlayGround(HWND hwnd)
{
	PplayGround Play = (PplayGround)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(struct _playGround));
	Play->hwndSelf = hwnd;
	Play->player.blue = CreateSolidBrush(0xff0000);
	Play->player.red = CreateSolidBrush(0x0000ff);
	Play->Heap = HeapCreate(HEAP_GENERATE_EXCEPTIONS, 0x0f0000, 0);
	Play->WidthPoint = MINIMALWIDTH;
	Play->HeightPoint = MINIMALHEIGHT;

	RECT rc;
	GetWindowRect(hwnd, &rc);
	Play->hdcBack = CreateCompatibleDC(GetDC(hwnd));
	Play->hbmBack = CreateBitmap(rc.right- RIGHTCOLUMN, rc.bottom-20, 1, 32, NULL);
	
	SelectObject(Play->hdcBack, Play->hbmBack);

	rc.right -= RIGHTCOLUMN;
	rc.bottom -= 25;
	Play->playGroundsize=rc;
	PlayGroundCalculatePlace(Play);
	PlayGroundBackDraw(Play);
	return Play;
}

int PlayGroundResize(PplayGround Play) 
{
	RECT RC;
	GetWindowRect(Play->hwndSelf, &RC);
	RC.right -= RIGHTCOLUMN;
	RC.bottom -= 25;
	DeleteObject(Play->hbmBack);
	Play->hbmBack = CreateBitmap(RC.right, RC.bottom, 1, 32, NULL);
	SelectObject(Play->hdcBack, Play->hbmBack);
	Play->playGroundsize = RC;
	PlayGroundCalculatePlace(Play);
	PlayGroundBackDraw(Play);


}

int PlayGroundBackDraw(PplayGround Play) 
{
	int r = r = Play->rad;
	//HBRUSH br = CreateHatchBrush(HS_FDIAGONAL, Play->player.red);
	HBRUSH br = CreateSolidBrush(0xFFFFFF);
	SelectObject(Play->hdcBack, br);
	FillRect(Play->hdcBack, &Play->playGroundsize, br);
	HBRUSH brush = CreateSolidBrush(0xFFFFFF);
	HPEN pen= CreatePen(PS_SOLID, 1, 0x000000);
	HPEN lpen = SelectObject(Play->hdcBack, pen);
	int i = 0;
	while ((i) < (Play->WidthPoint))
	{
		MoveToEx(Play->hdcBack, Play->PointArr[i * Play->HeightPoint].point.x, Play->PointArr[i * Play->HeightPoint].point.y, NULL);
		LineTo(Play->hdcBack, Play->PointArr[(i + 1) * Play->HeightPoint - 1].point.x, Play->PointArr[(i + 1) * Play->HeightPoint - 1].point.y);
		i++;
	}
	int j = 0;
	while (j < Play->HeightPoint)
	{
		MoveToEx(Play->hdcBack, Play->PointArr[j].point.x, Play->PointArr[j].point.y, NULL);
		LineTo(Play->hdcBack, Play->PointArr[(i - 1) * Play->HeightPoint + j].point.x, Play->PointArr[(i - 1) * Play->HeightPoint + j].point.y);
		j++;
	}
	i = 0;
	while (i < Play->WidthPoint)
	{
		j = 0;
		while (j < Play->HeightPoint)
		{
			SelectObject(Play->hdcBack, brush);
			if (Play->PointArr[i * Play->HeightPoint + j].color == RED)
				SelectObject(Play->hdcBack, Play->player.red);
			if (Play->PointArr[i * Play->HeightPoint + j].color == BLUE)
				SelectObject(Play->hdcBack, Play->player.blue);
			Ellipse(Play->hdcBack, Play->PointArr[i * Play->HeightPoint + j].point.x - r, Play->PointArr[(i)*Play->HeightPoint + j].point.y - r, Play->PointArr[i * Play->HeightPoint + j].point.x + r, Play->PointArr[(i)*Play->HeightPoint + j].point.y + r);

			j++;
		}
		i++;
	}

	PSPISOK Psp = Play->spis;

	while (Psp != NULL)
	{

		if (Psp->color == RED)
		{
			SelectObject(Play->hdcBack, Play->player.red);
		}
		if (Psp->color == BLUE)
			SelectObject(Play->hdcBack, Play->player.blue);
		Polygon(Play->hdcBack, Psp->ArrOfPoint, Psp->CountPoint);

		Psp = Psp->NextPoint;
	}

	SelectObject(Play->hdcBack, lpen);
	DeleteObject(pen);
	DeleteObject(brush);
}

int PlayGroundCalculatePlace(PplayGround Play)
{

	int i = 0, j = 0;
	int Size_x, Size_y;


	Size_x = (int)(Play->playGroundsize.right / Play->WidthPoint);
	Size_y = (int)(Play->playGroundsize.bottom / Play->HeightPoint);
	if (Size_x > Size_y)
		Play->rad = (int)(Size_x / 5);
	else
		Play->rad = (int)(Size_y / 5);

	while (i < Play->WidthPoint)
	{
		j = 0;
		while (j < Play->HeightPoint)
		{
			// gпоиграться с этими значениями, чтобы не было выхода за границы экрана, а то не учитываются радиусы в конечном итоге
			Play->PointArr[i * Play->HeightPoint + j].point.x = i * Size_x + Play->rad * 2;
			Play->PointArr[i * Play->HeightPoint + j].point.y = j * Size_y + Play->rad * 2;
			//Play->PointArr[i * Play->HeightPoint + j].color = 0;
			//Play->PointArr[i * Play->HeightPoint + j].brush = 0;
			j++;
		}
		i++;

	}
}


int PlayGroundCalculatePoint(PplayGround Play)
{
	int i = 0, j = 0;
	int count = 0;
	Play->CountRed = Play->CountBlue = 0;
	while (i < Play->WidthPoint)
	{
		j = 0;
		while (j < Play->HeightPoint)
		{
			if (Play->PointArr[i * Play->HeightPoint + j].brush == RED)
				Play->CountRed++;
			if (Play->PointArr[i * Play->HeightPoint + j].brush == BLUE)
				Play->CountBlue++;
			j++;
		}
		i++;
	}
}

PSPISOK PlayGroundCleanSpis(PplayGround Play)
{
	PSPISOK temp = Play->spis;
	while (temp != NULL)
	{
		PSPISOK tmp = temp;
		temp = temp->NextPoint;
		HeapFree(Play->Heap, 0, tmp->ArrOfPoint);
		HeapFree(Play->Heap, 0, tmp);

	}
	return NULL;

}

int PlayGroundRedraw(PplayGround Play) 
{
	RECT rc;
	GetWindowRect(Play->hwndSelf, &rc);
	rc.right -= RIGHTCOLUMN;
	PAINTSTRUCT ps;


	HDC windowhdc = BeginPaint(Play->hwndSelf,&ps);
	BitBlt(windowhdc, rc.left, rc.top, rc.right, rc.bottom, Play->hdcBack, 0, 0, SRCCOPY);


	if (Play->player.color == BLUE)
	{
		TextOutA(windowhdc, MINIMAL_WIDTH_WINDOW - 180, MINIMAL_HEIGHT_WINDOW - 70, CLEANTEXT, strlen(CLEANTEXT));
		TextOutA(windowhdc, MINIMAL_WIDTH_WINDOW - 180, MINIMAL_HEIGHT_WINDOW - 70, STEPBLUE, strlen(STEPBLUE));
	}
	if (Play->player.color == RED)
	{
		TextOutA(windowhdc, MINIMAL_WIDTH_WINDOW - 180, MINIMAL_HEIGHT_WINDOW - 70, STEPRED, strlen(STEPRED));
	}
	if (Play->player.color)
	{

		char text1[3] = { '\0' };
		itoa(Play->CountRed, text1, 10);
		TextOutA(windowhdc, MINIMAL_WIDTH_WINDOW - 180, MINIMAL_HEIGHT_WINDOW - 100, REDCOUNT, strlen(REDCOUNT));
		TextOutA(windowhdc, MINIMAL_WIDTH_WINDOW - 50, MINIMAL_HEIGHT_WINDOW - 100, text1, 3);
		char text2[3] = { '\0' };
		itoa(Play->CountBlue, text2, 10);
		TextOutA(windowhdc, MINIMAL_WIDTH_WINDOW - 180, MINIMAL_HEIGHT_WINDOW - 120, BLUECOUNT, strlen(BLUECOUNT));
		TextOutA(windowhdc, MINIMAL_WIDTH_WINDOW - 50, MINIMAL_HEIGHT_WINDOW - 120, text2, 3);
	}

	EndPaint(Play->hwndSelf, &ps);
}


int PlayGroundBrush(PplayGround Play, int color)
{
	int i = 0, j = 0;
	int brush = 0, count = 0, ResultWertical = 0, ResultHorizontal = 0;
	while (i < Play->WidthPoint)
	{
		j = 0;
		count = 0;
		while (j < Play->HeightPoint)
		{

			if (Play->PointArr[i * Play->HeightPoint + j].view == 1)
			{
				count++;
			}

			j++;
		}
		j = 0;
		brush = 0;
		while ((j < Play->HeightPoint) && (count > 0))
		{
			if ((brush) && (Play->PointArr[i * Play->HeightPoint + j].view != 1) && (Play->PointArr[i * Play->HeightPoint + j].brush != color))
			{
				ResultWertical++;
			}
			if ((Play->PointArr[i * Play->HeightPoint + j].view == 1))
			{
				brush = !brush;
				count--;
			}
			j++;
		}
		i++;
	}

	j = 0;
	while (j < Play->HeightPoint)
	{
		i = 0;
		count = 0;
		while (i < Play->WidthPoint)
		{

			if (Play->PointArr[i * Play->HeightPoint + j].view == 1)
			{
				count++;
			}

			i++;
		}
		i = 0;
		brush = 0;
		while ((i < Play->WidthPoint) && (count > 0))
		{
			if ((brush) && (Play->PointArr[i * Play->HeightPoint + j].view != 1) && (Play->PointArr[i * Play->HeightPoint + j].brush != color))
			{
				ResultHorizontal++;
			}
			if ((Play->PointArr[i * Play->HeightPoint + j].view == 1))
			{
				brush = !brush;
				count--;
			}
			i++;
		}
		j++;
	}
	if (ResultWertical == 0)
		return 0;
	if (ResultHorizontal == 0)
		return 0;
	if ((ResultWertical - ResultHorizontal) != 0)
		return 0;
	i = 0;
	while (i < Play->WidthPoint)
	{
		j = 0;
		count = 0;
		while (j < Play->HeightPoint)
		{

			if (Play->PointArr[i * Play->HeightPoint + j].view == 1)
			{
				count++;
			}

			j++;
		}
		j = 0;
		brush = 0;
		while ((j < Play->HeightPoint) && (count > 0))
		{
			if ((brush) && (Play->PointArr[i * Play->HeightPoint + j].view != 1) && (Play->PointArr[i * Play->HeightPoint + j].brush != color))
			{
				Play->PointArr[i * Play->HeightPoint + j].brush = color;
			}
			if ((Play->PointArr[i * Play->HeightPoint + j].view == 1))
			{
				Play->PointArr[i * Play->HeightPoint + j].view = 0;
				brush = !brush;
				count--;
			}
			j++;
		}
		i++;
	}
	return 1;
}

int PlayGroundIsUniq(PplayGround Play, PPOINT point, int count, int color)
{
	PSPISOK psp = Play->spis;
	PPOINT pp;
	int Max_x = 0, Max_y = 0, Min_x = MAXINT, Min_y = MAXINT;
	int i, tmp = 1;
	if (count < 4)
		return 0;
	while (psp != NULL)
	{
		tmp = 0;
		i = 0;
		if (psp->CountPoint != count)
			tmp = 1;
		else
		{
			pp = psp->ArrOfPoint;
			while (((point + i)->x == (pp + i)->x) && ((point + i)->y == (pp + i)->y) && (i <= count))
			{
				i++;
			}
			if (i != count)
				tmp = 1;
		}

		psp = psp->NextPoint;

	}

	if (!tmp)
		return 0;


	return  PlayGroundBrush(Play, color);
}

int PlayGroundTryBrush(PplayGround Play, int i, int j) 
{
	int Direction = 0, d1 = 0, loop1 = 0, d2;
	int Near_x = 0;
	int StartRedraw = 0;
	int res = 0;
	int Near_y = 0;
	PPOINT PArrOfPoint, SavePoint = NULL;
	DWORD err;
	int CountPoint = 0;
	int NewPP;

	while (Direction < 8)
	{

		int index = 0;
		while (index < Play->HeightPoint * Play->WidthPoint)
		{
			Play->PointArr[index].view = 0;
			index++;
		}
		index = 0;


		/*
		тут не хватает проверок на выход за границы массива,в принципе это не вызывает прямо капитальных проблем, но надо пофиксить.
		ошибки в индексации особенно заметны при проставлении точек в первой и последней строке массива		
		*/
		switch (Direction)
		{
		case 0:
			if (Play->PointArr[i * Play->HeightPoint + j].color != Play->PointArr[(i)*Play->HeightPoint + j + 1].color)
			{
				Near_x = i;
				Near_y = j + 1;
				loop1 = 1;
				d1 = Direction;
				d2 = Direction + 1;
			}
			break;
		case 1:
			if (Play->PointArr[i * Play->HeightPoint + j].color != Play->PointArr[(i + 1) * Play->HeightPoint + j + 1].color)
			{
				Near_x = i + 1;
				Near_y = j + 1;
				loop1 = 1;
				d1 = Direction;
				d2 = Direction + 1;
			}
			break;
		case 2:
			if (Play->PointArr[i * Play->HeightPoint + j].color != Play->PointArr[(i + 1) * Play->HeightPoint + j].color)
			{
				Near_x = i + 1;
				Near_y = j;
				loop1 = 1;
				d1 = Direction;
				d2 = Direction + 1;
				loop1 = 1;
			}
			break;
		case 3:
			if (Play->PointArr[i * Play->HeightPoint + j].color != Play->PointArr[(i + 1) * Play->HeightPoint + j - 1].color)
			{
				Near_x = i + 1;
				Near_y = j - 1;
				loop1 = 1;
				d1 = Direction;
				d2 = Direction + 1;
				loop1 = 1;
			}

			break;
		case 4:
			if (Play->PointArr[i * Play->HeightPoint + j].color != Play->PointArr[(i)*Play->HeightPoint + j - 1].color)
			{
				Near_x = i;
				Near_y = j - 1;
				loop1 = 1;
				d1 = Direction;
				d2 = Direction + 1;
				loop1 = 1;
			}

			break;
		case 5:
			if (Play->PointArr[i * Play->HeightPoint + j].color != Play->PointArr[(i - 1) * Play->HeightPoint + j - 1].color)
			{
				Near_x = i - 1;
				Near_y = j - 1;
				loop1 = 1;
				d1 = Direction;
				d2 = Direction + 1;
				loop1 = 1;
			}
			break;
		case 6:
			if (Play->PointArr[i * Play->HeightPoint + j].color != Play->PointArr[(i - 1) * Play->HeightPoint + j].color)
			{
				Near_x = i - 1;
				Near_y = j;
				loop1 = 1;
				d1 = Direction;
				d2 = Direction + 1;
				loop1 = 1;
			}

			break;
		case 7:
			if (Play->PointArr[i * Play->HeightPoint + j].color != Play->PointArr[(i - 1) * Play->HeightPoint + j + 1].color)
			{
				Near_x = i - 1;
				Near_y = j + 1;
				loop1 = 1;
				d1 = Direction;
				d2 = 0;
				loop1 = 1;
			}
			break;
		}

		if (loop1)
		{
			CountPoint = 0;
			CountPoint++;
			NewPP = 1;
			SavePoint = PArrOfPoint = (PPOINT)HeapAlloc(Play->Heap, HEAP_ZERO_MEMORY, CountPoint * sizeof(POINT));
			(PArrOfPoint + CountPoint - 1)->x = Play->PointArr[i * Play->HeightPoint + j].point.x;
			(PArrOfPoint + CountPoint - 1)->y = Play->PointArr[i * Play->HeightPoint + j].point.y;
			Play->PointArr[i * Play->HeightPoint + j].view = 1;
		}
		while (loop1)
		{
			switch (d2)
			{
			case 0:
				//
				if ((Play->PointArr[(Near_x + 1) * Play->HeightPoint + Near_y].color == Play->PointArr[i * Play->HeightPoint + j].color) && (!Play->PointArr[(Near_x + 1) * Play->HeightPoint + Near_y].view) && (Play->PointArr[(Near_x + 1) * Play->HeightPoint + Near_y].brush == 0))
				{
					d1 = d2 = 6;
					Play->PointArr[(Near_x + 1) * Play->HeightPoint + Near_y].view = 1;

					CountPoint++;
					PArrOfPoint = SavePoint = (PPOINT)HeapReAlloc(Play->Heap, 0, SavePoint, CountPoint * sizeof(POINT));
					*(PArrOfPoint + CountPoint - 1) = Play->PointArr[(Near_x + 1) * Play->HeightPoint + Near_y].point;

				}
				else
				{
					if ((Near_x == i) && (Near_y == j))
						StartRedraw = 1;
					else
						Near_x++;
				}
				break;
			case 1:
				//
				if ((Play->PointArr[(Near_x + 1) * Play->HeightPoint + Near_y].color == Play->PointArr[i * Play->HeightPoint + j].color) && (!Play->PointArr[(Near_x + 1) * Play->HeightPoint + Near_y].view) && (Play->PointArr[(Near_x + 1) * Play->HeightPoint + Near_y].brush == 0))
				{
					Play->PointArr[(Near_x + 1) * Play->HeightPoint + Near_y].view = 1;
					d2 = 6;
					d1 = 6;

					CountPoint++;
					SavePoint = (PPOINT)HeapReAlloc(Play->Heap, 0, SavePoint, CountPoint * sizeof(POINT));
					PArrOfPoint = SavePoint;
					*(PArrOfPoint + CountPoint - 1) = Play->PointArr[(Near_x + 1) * Play->HeightPoint + Near_y].point;
				}
				else
				{
					if ((Near_x == i) && (Near_y == j))
						StartRedraw = 1;
					else
						Near_x++;
				}
				break;
			case 2:
				//
				if ((Play->PointArr[(Near_x)*Play->HeightPoint + Near_y - 1].color == Play->PointArr[i * Play->HeightPoint + j].color) && (!Play->PointArr[(Near_x)*Play->HeightPoint + Near_y - 1].view) && (Play->PointArr[(Near_x)*Play->HeightPoint + Near_y - 1].brush == 0))
				{
					Play->PointArr[(Near_x)*Play->HeightPoint + Near_y - 1].view = 1;
					d2 = 0;
					d1 = 0;

					CountPoint++;
					SavePoint = (PPOINT)HeapReAlloc(Play->Heap, 0, SavePoint, CountPoint * sizeof(POINT));
					PArrOfPoint = SavePoint;
					*(PArrOfPoint + CountPoint - 1) = Play->PointArr[(Near_x)*Play->HeightPoint + Near_y - 1].point;
				}
				else
				{
					if ((Near_x == i) && (Near_y == j))
						StartRedraw = 1;
					else
						Near_y--;

				}
				break;
			case 3:
				if ((Play->PointArr[(Near_x)*Play->HeightPoint + Near_y - 1].color == Play->PointArr[i * Play->HeightPoint + j].color) && (!Play->PointArr[(Near_x)*Play->HeightPoint + Near_y - 1].view) && (Play->PointArr[(Near_x)*Play->HeightPoint + Near_y - 1].brush == 0))
				{
					Play->PointArr[(Near_x)*Play->HeightPoint + Near_y - 1].view = 1;
					d1 = d2 = 0;

					CountPoint++;
					SavePoint = (PPOINT)HeapReAlloc(Play->Heap, 0, SavePoint, CountPoint * sizeof(POINT));
					PArrOfPoint = SavePoint;
					*(PArrOfPoint + CountPoint - 1) = Play->PointArr[(Near_x)*Play->HeightPoint + Near_y - 1].point;
				}
				else
				{
					if ((Near_x == i) && (Near_y == j))
						StartRedraw = 1;
					else
						Near_y--;
				}
				break;
			case 4:
				if ((Play->PointArr[(Near_x - 1) * Play->HeightPoint + Near_y].color == Play->PointArr[i * Play->HeightPoint + j].color) && (!Play->PointArr[(Near_x - 1) * Play->HeightPoint + Near_y].view) && (Play->PointArr[(Near_x - 1) * Play->HeightPoint + Near_y].brush == 0))
				{
					Play->PointArr[(Near_x - 1) * Play->HeightPoint + Near_y].view = 1;
					d1 = d2 = 2;
					CountPoint++;
					SavePoint = (PPOINT)HeapReAlloc(Play->Heap, 0, SavePoint, CountPoint * sizeof(POINT));
					PArrOfPoint = SavePoint;
					*(PArrOfPoint + CountPoint - 1) = Play->PointArr[(Near_x - 1) * Play->HeightPoint + Near_y].point;
				}
				else
				{
					if ((Near_x == i) && (Near_y == j))
						StartRedraw = 1;
					else
						Near_x--;
				}
				break;
			case 5:
				if ((Play->PointArr[(Near_x - 1) * Play->HeightPoint + Near_y].color == Play->PointArr[i * Play->HeightPoint + j].color) && (!Play->PointArr[(Near_x - 1) * Play->HeightPoint + Near_y].view) && (Play->PointArr[(Near_x - 1) * Play->HeightPoint + Near_y].brush == 0))
				{
					Play->PointArr[(Near_x - 1) * Play->HeightPoint + Near_y].view = 1;
					d1 = d2 = 2;
					CountPoint++;
					SavePoint = (PPOINT)HeapReAlloc(Play->Heap, 0, SavePoint, CountPoint * sizeof(POINT));
					PArrOfPoint = SavePoint;
					*(PArrOfPoint + CountPoint - 1) = Play->PointArr[(Near_x - 1) * Play->HeightPoint + Near_y].point;
				}
				else
				{
					if ((Near_x == i) && (Near_y == j))
						StartRedraw = 1;
					else
						Near_x--;
				}
				break;
			case 6:
				if ((Play->PointArr[(Near_x)*Play->HeightPoint + Near_y + 1].color == Play->PointArr[i * Play->HeightPoint + j].color) && (!(Play->PointArr[(Near_x)*Play->HeightPoint + Near_y + 1].view)) && (Play->PointArr[(Near_x)*Play->HeightPoint + Near_y + 1].brush == 0))
				{
					Play->PointArr[(Near_x)*Play->HeightPoint + Near_y + 1].view = 1;
					d1 = d2 = 4;

					CountPoint++;
					SavePoint = (PPOINT)HeapReAlloc(Play->Heap, 0, SavePoint, CountPoint * sizeof(POINT));
					PArrOfPoint = SavePoint;
					*(PArrOfPoint + CountPoint - 1) = Play->PointArr[(Near_x)*Play->HeightPoint + Near_y + 1].point;
				}
				else
				{
					if ((Near_x == i) && (Near_y == j))
						StartRedraw = 1;
					else
						Near_y++;
				}
				break;
			case 7:
				if ((Play->PointArr[(Near_x)*Play->HeightPoint + Near_y + 1].color == Play->PointArr[i * Play->HeightPoint + j].color) && (!(Play->PointArr[(Near_x)*Play->HeightPoint + Near_y + 1].view)) && (Play->PointArr[(Near_x)*Play->HeightPoint + Near_y + 1].brush == 0))
				{
					Play->PointArr[(Near_x)*Play->HeightPoint + Near_y + 1].view = 1;
					d1 = d2 = 4;
					CountPoint++;
					SavePoint = (PPOINT)HeapReAlloc(Play->Heap, 0, SavePoint, CountPoint * sizeof(POINT));
					PArrOfPoint = SavePoint;
					*(PArrOfPoint + CountPoint - 1) = Play->PointArr[(Near_x)*Play->HeightPoint + Near_y + 1].point;
				}
				else
				{
					if ((Near_x == i) && (Near_y == j))
						StartRedraw = 1;
					else
						Near_y++;
				}
				break;

			}
			d2++;
			if (d2 == 8)
				d2 = 0;
			if (d2 == d1)
				loop1 = 0;
		}
		if (StartRedraw)
		{
			if (PlayGroundIsUniq(Play, SavePoint, CountPoint, Play->PointArr[i * Play->HeightPoint + j].color))
			{
				NewPP = 0;
				StartRedraw = 0;
				res = 1;
				int op = 0;
				POINT pp[4];
				while ((op < CountPoint) && (op < 4))
				{
					pp[op] = *(SavePoint + op);
					op++;
				}
				if (!Play->spis)
				{
					Play->spis = (PSPISOK)HeapAlloc(Play->Heap, HEAP_ZERO_MEMORY, sizeof(SPISOK));
					Play->spis->ArrOfPoint = SavePoint;
					Play->spis->color = Play->PointArr[i * Play->HeightPoint + j].color;
					Play->spis->CountPoint = CountPoint;
					Play->spis->NextPoint = NULL;
				}
				else
				{
					PSPISOK tmp, temp;
					temp = tmp = Play->spis;
					temp = (PSPISOK)HeapAlloc(Play->Heap, HEAP_ZERO_MEMORY, sizeof(SPISOK));
					temp->ArrOfPoint = SavePoint;
					temp->CountPoint = CountPoint;
					temp->color = Play->PointArr[i * Play->HeightPoint + j].color;
					temp->NextPoint = tmp;
					Play->spis = temp;
				}
			}
		}
		else
		{
			if ((SavePoint != NULL) && (NewPP))
			{
				HeapFree(Play->Heap, 0, SavePoint);
				SavePoint = NULL;
				NewPP = 0;

			}
		}
		Direction++;
	}
	return res;
}


/// <summary>
/// this function search exist loop. if loop under other loop-> first loop delete 
/// </summary>
/// <param name="Play"></param>
void PlayGroundIsPoligonExist(PplayGround Play)
{
	PSPISOK PSp = Play->spis, tmp = Play->spis;
	PPOINT pp;
	int i, j;
	while (PSp != NULL)
	{
		pp = PSp->ArrOfPoint;
		i = (int)(pp->x / (int)(Play->playGroundsize.right / Play->WidthPoint));
		j = (int)(pp->y / (int)(Play->playGroundsize.bottom / Play->HeightPoint));
		if (Play->PointArr[i * Play->HeightPoint + j].brush != 0)
		{
			if (tmp == PSp)
			{
				PSPISOK delete = Play->spis;
				Play->spis = PSp->NextPoint;
				HeapFree(Play->Heap, 0, delete);
				// а вот здесь потенциально(100%) утечка памяти
			}
			else
			{
				PSPISOK delete = PSp;
				tmp->NextPoint = PSp->NextPoint;
				HeapFree(Play->Heap, 0, delete);
			}
		}
		tmp = PSp;
		PSp = PSp->NextPoint;
	}

}

int PlayGroundAddPointInt(PplayGround Play, int Point)
{
	int i = Point >> 16;
	int j = Point & (MAXINT>>16);

	Play->YouStep = Play->YouStep ^ 1;
	if (Play->player.color == RED)
	{
		Play->PointArr[i * Play->HeightPoint + j].color = RED;
		Play->player.color = BLUE;
	}
	else
		if (Play->player.color == BLUE)
		{
			Play->PointArr[i * Play->HeightPoint + j].color = BLUE;
			Play->player.color = RED;
		}
		else
		{
			Play->PointArr[i * Play->HeightPoint + j].color = RED;
			Play->player.color = BLUE;
		}
	PlayGroundTryBrush(Play, i, j);
	PlayGroundCalculatePoint(Play);
	PlayGroundIsPoligonExist(Play);
	PlayGroundBackDraw(Play);

}



int PlayGroundAddPoint(PplayGround Play, LPARAM lParam) 
{
	// получение координат
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);


	// получение значение в массиве
	int i = 0, j = 0;
	i = (int)(x / (int)(Play->playGroundsize.right / Play->WidthPoint));
	j = (int)(y / (int)(Play->playGroundsize.bottom / Play->HeightPoint));

	// проверка на попадание в радиус точки
	if (!((x > Play->PointArr[i * Play->HeightPoint].point.x - Play->rad * 2) && (x < Play->PointArr[i * Play->HeightPoint].point.x + Play->rad * 2)))
		return -1;
	if (!((y > Play->PointArr[i * Play->HeightPoint + j].point.y - Play->rad * 2) && (y < Play->PointArr[i * Play->HeightPoint + j].point.y + Play->rad * 2)))
		return -1;

	// проверка на закрашенность 
	if (!(Play->PointArr[i * Play->HeightPoint + j].color == 0))
		return;
	if (!(Play->PointArr[i * Play->HeightPoint + j].brush == 0))
		return;

	Play->YouStep = Play->YouStep ^ 1;
	if (Play->player.color == RED)
	{
		Play->PointArr[i * Play->HeightPoint + j].color = RED;
		Play->player.color = BLUE;
	}
	else
		if (Play->player.color == BLUE)
		{
			Play->PointArr[i * Play->HeightPoint + j].color = BLUE;
			Play->player.color = RED;
		}
		else
		{
			Play->PointArr[i * Play->HeightPoint + j].color = RED;
			Play->player.color = BLUE;
		}

	int res = 0;
	PlayGroundTryBrush(Play, i, j);
	PlayGroundCalculatePoint(Play);
	PlayGroundIsPoligonExist(Play);
	PlayGroundBackDraw(Play);
	res = (i << 16 ) + j;
	return res;
}

int PlayGroundEndGame(PplayGround Play)
{
	if (Play == NULL)
		return 0;
	if (Play->CountBlue > 25)
	{
		MessageBox(Play->hwndSelf, L"Выйграл синий игрок!", L" ", MB_OK);
		Play->EndGame = 1;
		return 1;
	}

	if (Play->CountRed > 25)
	{
		MessageBox(Play->hwndSelf, L"Выйграл красный игрок!", L" ", MB_OK);
		Play->EndGame = 1;
		return 1;
	}
	int i = 0;
	int j = 0;
	int Clear = 0;
	while (i < Play->WidthPoint)
	{
		j = 0;
		while (j < Play->HeightPoint)
		{
			if (Play->PointArr[i * Play->HeightPoint + j].color == 0)
				Clear++;
			j++;
		}
		i++;
	}
	if (!Clear)
	{
		if (Play->CountBlue < Play->CountRed)
		{
			MessageBox(Play->hwndSelf, L"Выйграл красный игрок!", L" ", MB_OK);
			Play->EndGame = 1;
			return 1;
		}
		if (Play->CountBlue > Play->CountRed)
		{
			MessageBox(Play->hwndSelf, L"Выйграл синий игрок!", L" ", MB_OK);
			Play->EndGame = 1;
			return 1;
		}
		if (Play->CountBlue == Play->CountRed)
		{
			MessageBox(Play->hwndSelf, L"Победтла Дружба!", L" ", MB_OK);
			Play->EndGame = 1;
			return 1;
		}
	}
	return 0;
}