#include <stdlib.h> // malloc

#include "adt_dlist.h"

// internal insert function
// inserts data into list
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr){	
	NODE *name = (NODE *)malloc(sizeof(NODE));
	if (!name) return 0;	
	name->dataPtr = dataInPtr;
	
	if (pPre == NULL ){
		name->llink = NULL;
		name->rlink = pList->head;
		
		if (pList->head != NULL) 
			pList->head->llink = name;
		pList->head = name;
		
		return 1;
	}
	
	name->llink = pPre;
	
	if (pPre->rlink == NULL){
		name->rlink = NULL;
		pList->rear = name;
		
	}
	else{
		name->rlink = pPre->rlink;
		pPre->rlink->llink = name;
	}
	
	pPre->rlink = name;
	
	return 1;
}

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr){
	*dataOutPtr = pLoc->dataPtr;
	
	if (pPre == NULL ){
		pLoc->rlink->llink = NULL;
		pList->head = pLoc->rlink;
	}
	else if (pLoc->rlink == NULL){
		pLoc->rlink->llink = NULL;
		pList->rear = pPre;
	}
	else{
		pPre->rlink = pLoc->rlink;
		pLoc->rlink->llink = pPre;
	}
	
	free(pLoc);
}


// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu){
	int flag = 0;
	
	while(*pLoc != NULL){
		if (!(pList->compare(pArgu, (*pLoc)->dataPtr))) return 1;
		else if ((pList->compare(pArgu, (*pLoc)->dataPtr)) < 0) return 0;
		
		*pPre = *pLoc;
		*pLoc = (*pLoc)->rlink;
	}
	
	return 0;
}


// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList( int (*compare)(const void *, const void *)){
	LIST *names = (LIST *)malloc( sizeof(LIST));
	if (!names) return NULL;
	
	names->count = 0;
	names->head = NULL;
	names->rear = NULL;
	names->compare = compare;

	return names;
}

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList, void (*callback)(void *)){
	NODE *ptr = NULL;
	
	while (pList->head != NULL){
		ptr = pList->head;
		pList->head = ptr->rlink;
		(*callback)(ptr->dataPtr);
		free(ptr);
	}	
	
	pList->count = 0;
	free(pList);
}

// Inserts data into list
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
int addNode( LIST *pList, void *dataInPtr, void (*callback)(const void *, const void *)){
	NODE *pPre = NULL;
	NODE *pLoc = pList->head;
	
	if (!_search(pList, &pPre, &pLoc, dataInPtr)){
		if (!_insert( pList, pPre, dataInPtr))
			return 0;
		
		if (pPre == NULL) pLoc = pList->head;
		else pLoc = pPre->rlink;
		
		pList->count++;
		return 1;
	}
	
	(*callback)(pLoc->dataPtr, dataInPtr);
	return 2;
}

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, void *keyPtr, void **dataOutPtr){
	NODE *pPre = NULL;
	NODE *pLoc = pList->head;
	
	if (_search(pList, &pPre, &pLoc, keyPtr)){
		_delete(pList, pPre, pLoc, dataOutPtr);
		pList->count--;
		return 1;
	}
	return 0;
}

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchList( LIST *pList, void *pArgu, void **dataOutPtr){
	NODE *pPre = NULL;
	NODE *pLoc = pList->head;
	
	if (_search(pList, &pPre, &pLoc, pArgu)){
		*dataOutPtr = pLoc->dataPtr;
		return 1;
	}
	return 0;
}

// returns number of nodes in list
int countList( LIST *pList){
	return pList->count;
}

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList){
	return !pList->count ? 1 : 0;
}

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const void *)){
	NODE *node = pList->head;
	
	while(node != NULL){
		(*callback)(node->dataPtr);
		node = node->rlink;
	}
}

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const void *)){
	NODE *node = pList->rear;
	
	while(node != NULL){
		(*callback)(node->dataPtr);
		node = node->llink;
	}
}