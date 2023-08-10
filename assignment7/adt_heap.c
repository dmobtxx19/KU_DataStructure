#include <stdio.h>
#include <stdlib.h> // malloc, realloc

#include "adt_heap.h"


/*
typedef struct
{
	void** heapArr;
	int	last;
	int	capacity;
	int (*compare) (void* arg1, void* arg2);
} HEAP;
*/

/* Reestablishes heap by moving data in child up to correct location heap array
*/
static void _reheapUp(HEAP* heap, int index) {
	void* tmp;
	while (index != 0 && heap->compare(heap->heapArr[index], heap->heapArr[(index - 1) / 2]) > 0) {
		tmp = heap->heapArr[index];
		heap->heapArr[index] = heap->heapArr[(index - 1) / 2];
		heap->heapArr[(index - 1) / 2] = tmp;
		index = (index - 1) / 2;
	}
}


/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
static void _reheapDown(HEAP* heap, int index) {
	void* tmp;
	int child_index;
	while (index * 2 + 1 <= heap->last) {
		if ((index + 1) * 2 <= heap->last){
			if (heap->compare(heap->heapArr[(index + 1) * 2], heap->heapArr[index * 2 + 1]) > 0)
				child_index = (index + 1) * 2;
			else
				child_index = index * 2 + 1;
		}
		else
			child_index = index * 2 + 1;

		if (heap->compare(heap->heapArr[child_index], heap->heapArr[index]) > 0) {
			tmp = heap->heapArr[index];
			heap->heapArr[index] = heap->heapArr[child_index];
			heap->heapArr[child_index] = tmp;
			index = child_index;
		}
		else
			break;
	}

}


/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
*/
HEAP* heap_Create(int capacity, int (*compare) (void* arg1, void* arg2)) {
	HEAP* heap = (HEAP*)malloc(sizeof(HEAP));
	if (heap == NULL)
		return NULL;

	heap->heapArr = (void**)malloc(sizeof(void*) * capacity);
	heap->last = -1;
	heap->capacity = capacity;
	heap->compare = compare;
	return heap;
}

/* Free memory for heap
*/
void heap_Destroy(HEAP* heap) {
	while (!heap_Empty) {
		free(heap->heapArr[heap->last]);
		heap->last--;
	}
	free(heap->heapArr);
	free(heap);
}

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heap_Insert(HEAP* heap, void* dataPtr) {
	heap->last++;
	if (heap->last >= heap->capacity)
	{
		heap->heapArr = (void **)realloc(heap->heapArr, sizeof(void*) * heap->capacity * 2);
		heap->capacity *= 2;
	}
	heap->heapArr[heap->last] = dataPtr;
	_reheapUp(heap, heap->last);
	return 1;
}

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heap_Delete(HEAP* heap, void** dataOutPtr) {
	if (!heap_Empty)
		return 0;

	*dataOutPtr = heap->heapArr[0];
	heap->heapArr[0] = heap->heapArr[heap->last];
	heap->last--;
	_reheapDown(heap, 0);
	return 1;
}

/*
return 1 if the heap is empty; 0 if not
*/
int heap_Empty(HEAP* heap) {
	if (heap->last == -1)
		return 1;
	return 0;
}

/* Print heap array */
void heap_Print(HEAP* heap, void (*print_func) (void* data)) {
	for (int i = 0; i <= heap->last; i++) {
		print_func(heap->heapArr[i]);
	}
	printf("\n");
}

