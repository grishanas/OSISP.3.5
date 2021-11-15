#include "Queue.h"

PQueue CreateQueue() {
	PQueue Queue = malloc(sizeof(_Queue));
	Queue->List = 0;
	Queue->Heap = HeapCreate(HEAP_GENERATE_EXCEPTIONS, 2048, 0);
	Queue->Mutex = CreateMutex(NULL, FALSE, NULL);
	return Queue;
}

void Enqueue(PQueue Queue, PVOID Elem) {

	PList List = HeapAlloc(Queue->Heap, HEAP_ZERO_MEMORY, sizeof(_List));
	List->Element = Elem;
	List->NextElement = NULL;

	WaitForSingleObject(Queue->Mutex, INFINITE);
	{
		__try {
			if (Queue->List == NULL)
			{
				Queue->List = List;
			}
			else {
				PList temp = Queue->List;
				while (temp->NextElement != NULL)
					temp = temp->NextElement;
				temp->NextElement = List;
			}
		}
		__finally
		{
			ReleaseMutex(Queue->Mutex);
		}
	}
}

PVOID Dequeue(PQueue Queue) {
	PList List = NULL;
	PVOID res=NULL;
	WaitForSingleObject(Queue->Mutex, INFINITE);
	{
		__try
		{
			if (Queue->List == NULL)
				return 0;

			List = Queue->List;
			res = List->Element;
			Queue->List = List->NextElement;
			HeapFree(Queue->Heap, 0, List);

		}
		__finally
		{
			ReleaseMutex(Queue->Mutex);
		}
	}
	return res;
}