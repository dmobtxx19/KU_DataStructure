#define SHOW_STEP 0// 제출시 0
#define BALANCING 1 // 제출시 1 (used in _insert function)

#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> //strcmp, strdup

#define max(x, y)	(((x) > (y)) ? (x) : (y))

////////////////////////////////////////////////////////////////////////////////
// AVL_TREE type definition
typedef struct node
{
	char		*data;
	struct node	*left;
	struct node	*right;
	int			height;
} NODE;

typedef struct
{
	NODE	*root;
	int		count;  // number of nodes
} AVL_TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create( void);

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy( AVL_TREE *pTree);

static void _destroy( NODE *root);

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, char *data);

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert( NODE *root, NODE *newPtr);

static NODE *_makeNode( char *data);

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char *AVL_Retrieve( AVL_TREE *pTree, char *key);

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, char *key);

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree);
static void _traverse( NODE *root);

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree);
/* internal traversal function
*/
static void _infix_print( NODE *root, int level);

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root);

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	AVL_TREE *tree;
	char str[1024];
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 0;
	}
	// creates a null tree
	tree = AVL_Create();
	if (!tree)
	{
		fprintf( stderr, "Cannot create tree!\n");
		return 100;
	}


	FILE *fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "Cannot open file! [%s]\n", argv[1]);
		return 200;
	}

	while(fscanf( fp, "%s", str) != EOF)
	{

///조건부 컴파일 notation///
#if SHOW_STEP
		fprintf( stdout, "Insert %s>\n", str);
#endif		
		// insert function call
		AVL_Insert( tree, str);

#if SHOW_STEP
		fprintf( stdout, "Tree representation:\n");
		printTree( tree);
#endif
	}
	
	fclose( fp);
	
#if SHOW_STEP
	fprintf( stdout, "\n");

	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	AVL_Traverse( tree);
	fprintf( stdout, "\n");

	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
#endif
///end of conditional compile///

	fprintf( stdout, "Height of tree: %d\n", tree->root->height);
	fprintf( stdout, "# of nodes: %d\n", tree->count);
	
	// retrieval
	char *key;
	fprintf( stdout, "Query: ");
	while( fscanf( stdin, "%s", str) != EOF)
	{
		key = AVL_Retrieve( tree, str);
		
		if (key) fprintf( stdout, "%s found!\n", key);
		else fprintf( stdout, "%s NOT found!\n", str);
		
		fprintf( stdout, "Query: ");
	}
	
	// destroy tree
	AVL_Destroy( tree);

	return 0;
}


/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create( void){
    AVL_TREE* avl = (AVL_TREE*)malloc(sizeof(AVL_TREE));
    avl->root = NULL;
    avl->count = 0;

    return avl;
}

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy( AVL_TREE *pTree){
    if (pTree->root == NULL){
        free (pTree);
    }
    else{
        _destroy(pTree->root);
        free(pTree);
    }
}

static void _destroy( NODE *root){
    if (root == NULL){
        return;
    }
    _destroy(root->left);
    _destroy(root->right);
    free(root->data); //char pointer니까 free 해주고
    free(root);
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, char *data){

    NODE *insertnode = _makeNode(data);

    if (pTree->root == NULL){
        pTree->root = insertnode;
        pTree->count++;
        return 1;
    }

    pTree->root = _insert(pTree->root, insertnode);

    pTree->count++;

    return 1;
}

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
//이거는 node* 리턴이라서 iteration이 아니라 재귀로 짜기 가능.
static NODE *_insert( NODE *root, NODE *newPtr){

    //getheightfucntion을 안쓰면 null 일때 segfault
    if (root == NULL){ //끝에 도달했음 //재귀로 짤때는 어짜피 밖에서 호출해서 들어갈 때 root->left거나 root->right라고 받아줬으니까
        //그냥 newPtr만 리턴하면 자동으로 연결됨.
        root = newPtr;
        return root;
        //처음이거나, 맨 끝에 삽입된 경우 level이 1 기본 설정
    }
    else if ( strcmp(newPtr->data, root->data) < 0){
        root->left = _insert(root->left, newPtr);
        //왼쪽에 insert를 했을 때 left-high가 된다면
    }
    else if ( strcmp( newPtr->data, root->data ) >= 0){
        root->right = _insert(root->right, newPtr);
    }

    root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
    //방금 insert된 node의 parent의 height를 고쳐주고

#if BALANCING

    if(getHeight(root->left) - getHeight(root->right) > 1){ //left high
        if(getHeight(root->left->left) - getHeight(root->left->right) > 0){ //ll
            return rotateRight(root);
        }
        else{ //right of left;
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
    }
    if(getHeight(root->left) - getHeight(root->right) < -1){ //right-high가 되면
        if( getHeight(root->right->left) - getHeight(root->right->right) < 0){ //rr
            return rotateLeft(root);
        }
        else{ //left of right;
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }
    }
#endif

    return root;
}

static NODE *_makeNode( char *data){
    NODE* temp = (NODE*)malloc(sizeof(NODE));
    temp->data = strdup(data);
    temp->left = NULL;
    temp->right = NULL;
    temp->height = 1; //각 node의 height를 integer 값으로 품고 있음

    return temp;
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char *AVL_Retrieve( AVL_TREE *pTree, char *key){
    NODE* retret = _retrieve(pTree->root, key);

    if ( retret == NULL){ //만약 null이 들어온다면, 즉 못찾았으면
        return NULL;
    }
    else{
        return retret->data; //주소값 리턴 //data가 char*
    }
}

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, char *key){

    NODE* temp = root;

    while(temp != NULL){
        if( strcmp(key, temp->data) == 0){
            return temp;
        }
        else if(strcmp(key,temp->data) < 0){
            temp = temp->left;
        }
        else if (strcmp(key,temp->data) > 0){
            temp = temp->right;
        }
    }

    return NULL;
}

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree){
    _traverse(pTree->root);
}

static void _traverse( NODE *root){
    if (root == NULL){
        return;
    }

    _traverse(root->left);
    printf("%s ", root->data);
    _traverse(root->right);

}

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree){
    int level = -1;
    _infix_print(pTree->root, level);
}
/* internal traversal function
*/
static void _infix_print( NODE *root, int level){
    if (root == NULL){
        return;
    }

    level++;

    _infix_print(root->right, level);

    for (int i = 0; i < level; i++){
        printf("\t");
    }
    printf("%s\n", root->data);

    _infix_print(root->left, level);
}

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root){
    if (root == NULL){
        return 0;
    }
    return root->height;
}

//나는 내 자식들보다 높이가 높다. 내 자식들 중 높이가 높은 애의 height + 1 == 나의 높이


/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root){

    NODE* newroot = root->left;
    NODE* leftright = root->left->right;

    root->left = leftright;
    newroot->right = root;

    root->height = max( getHeight(root->right) , getHeight(root->left)) + 1;
    newroot->height = max( getHeight(newroot->right) , getHeight(newroot->left)) + 1;

    return newroot; //새로운 루트 리턴
}

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root){

    NODE* newroot = root->right;
    NODE* rightleft = root->right->left;

    root->right = rightleft;
    newroot->left = root;

    root->height = max( getHeight(root->right) , getHeight(root->left)) + 1;
    newroot->height = max( getHeight(newroot->right) , getHeight(newroot->left)) + 1;

    return newroot;
}


