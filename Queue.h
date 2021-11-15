#pragma once
#include <Windows.h>

typedef struct _List {
	PVOID Element;
	struct _List* NextElement;

}_List, * PList;

typedef struct {
	HANDLE Heap;
	HANDLE Mutex;
	PList List;
} _Queue, * PQueue;

void Enqueue(PQueue Queue, PVOID Elem);
PVOID Dequeue(PQueue Queue);
PQueue CreateQueue(); 
