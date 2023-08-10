#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

// User structure type definition
typedef struct 
{
	char	*name;	// 이름
	int		freq;	// 빈도
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tName		*dataPtr;
	struct node	*llink;
	struct node	*rlink;
} NODE;

typedef struct
{
	int		count;
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void);

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList);

// Inserts data into list
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
int addNode( LIST *pList, tName *dataInPtr);

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, tName *keyPtr, tName **dataOutPtr);

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchList( LIST *pList, tName *pArgu, tName **dataOutPtr);

// returns number of nodes in list
int countList( LIST *pList);

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList);

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const tName *));

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const tName *));

// internal insert function
// inserts data into list
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr);

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tName **dataOutPtr);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu);


////////////////////////////////////////////////////////////////////////////////
// Allocates dynamic memory for a name structure, initialize fields(name, freq) and returns its address to caller
//	return	name structure pointer
//			NULL if overflow
tName *createName( char *name, int freq);

// Deletes all data in name structure and recycles memory
void destroyName( tName *pNode);

////////////////////////////////////////////////////////////////////////////////
// gets user's input
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'P':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

// compares two names in name structures
// for createList function
int cmpName( const tName *pName1, const tName *pName2)
{
	return strcmp( pName1->name, pName2->name);
}

// prints contents of name structure
// for traverseList and traverseListR functions
void print_name(const tName *dataPtr)
{
	printf( "%s\t%d\n", dataPtr->name, dataPtr->freq);
}

// increases freq in name structure
// for addNode function
void increase_freq(tName *dataOutPtr, const tName *dataInPtr)
{
	dataOutPtr->freq += dataInPtr->freq;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	
	char name[100];
	int freq;
	
	tName *pName;
	int ret;
	FILE *fp;
	
	if (argc != 2){
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	while(fscanf( fp, "%*d\t%s\t%*c\t%d", name, &freq) != EOF)
	{
		pName = createName( name, freq);
		ret = addNode( list, pName);
		if (ret == 2) // duplicated
		{
			destroyName( pName);
		}
	}
	
	fclose( fp);
	
	fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	
	while (1)
	{
		tName *ptr;
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case FORWARD_PRINT:
				traverseList( list, print_name);
				break;
			
			case BACKWARD_PRINT:
				traverseListR( list, print_name);
				break;
			
			case SEARCH:
				fprintf( stderr, "Input a name to find: ");
				fscanf( stdin, "%s", name);
				
				pName = createName( name, 0);

				if (searchList( list, pName, &ptr)) print_name( ptr);
				else fprintf( stdout, "%s not found\n", name);
				
				destroyName( pName);
				break;
				
			case DELETE:
				fprintf( stderr, "Input a name to delete: ");
				fscanf( stdin, "%s", name);
				
				pName = createName( name, 0);

				if (removeNode( list, pName, &ptr))
				{
					fprintf( stdout, "(%s, %d) deleted\n", ptr->name, ptr->freq);
					destroyName( ptr);
				}
				else fprintf( stdout, "%s not found\n", name);
				
				destroyName( pName);
				break;
			
			case COUNT:
				fprintf( stdout, "%d\n", countList( list));
				break;
		}
		
		if (action) fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}


// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void){
	LIST *names = (LIST *)malloc( sizeof(LIST));
	if (!names) return NULL;
	
	names->count = 0;
	names->head = NULL;
	names->rear = NULL;

	return names;
}

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList){
	NODE *ptr = NULL;
	while (pList->head != NULL){
		ptr = pList->head;
		pList->head = ptr->rlink;
		destroyName(ptr->dataPtr);
		free(ptr);
	}	
	
	pList->count = 0;
	free(pList);
}

// Inserts data into list
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
int addNode( LIST *pList, tName *dataInPtr){
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
	
	pLoc->dataPtr->freq += dataInPtr->freq;
	return 2;
}

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, tName *keyPtr, tName **dataOutPtr){
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
int searchList( LIST *pList, tName *pArgu, tName **dataOutPtr){
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
void traverseList( LIST *pList, void (*callback)(const tName *)){
	NODE *node = pList->head;
	
	while(node != NULL){
		callback(node->dataPtr);
		node = node->rlink;
	}
}

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const tName *)){
	NODE *node = pList->rear;
	
	while(node != NULL){
		callback(node->dataPtr);
		node = node->llink;
	}
}

// internal insert function
// inserts data into list
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr){	
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
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tName **dataOutPtr){
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
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu){
	int flag = 0;
	
	while(*pLoc != NULL){
		flag = cmpName(pArgu, (*pLoc)->dataPtr);
		
		if (!flag) return 1;
		else if (flag < 0) return 0;
		*pPre = *pLoc;
		*pLoc = (*pLoc)->rlink;
	}
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Allocates dynamic memory for a name structure, initialize fields(name, freq) and returns its address to caller
//	return	name structure pointer
//			NULL if overflow
tName *createName( char *name, int freq){
	tName *tname = (tName *)malloc( sizeof(tName));
	if (!tname) return NULL;
	
	tname->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(tname->name, name);
	tname->freq = freq;
	
	return tname;
}	

// Deletes all data in name structure and recycles memory
void destroyName( tName *pNode){
	free(pNode->name);
	free(pNode);
}
