#define SHOW_STEP 0 // 제출시 0
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
AVL_TREE* AVL_Create(void) {
	AVL_TREE* pTree = (AVL_TREE*)malloc(sizeof(AVL_TREE));
	if (pTree == NULL) return NULL;

	pTree->root = NULL;
	pTree->count = 0;
	return pTree;
}

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy(AVL_TREE* pTree){
	if (pTree->root != NULL)
		_destroy(pTree->root);

	free(pTree);
}
static void _destroy(NODE* root){
	if (root != NULL) {
		_destroy(root->left);
		_destroy(root->right);
		free(root->data);
		free(root);
	}
	return;
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert(AVL_TREE* pTree, char* data) {
	NODE* pNode = _makeNode(data);
	if (pNode == NULL) return 0;

	pTree->root = _insert(pTree->root, pNode);
	pTree->count++;
	return 1;
}

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE* _insert(NODE* root, NODE* newPtr) {
	if (root != NULL) {
		if (strcmp(root->data, newPtr->data) > 0)
			root->left = _insert(root->left, newPtr);
		else
			root->right = _insert(root->right, newPtr);

		root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	}
	else
		return newPtr;

	if (BALANCING) {
		int balance = getHeight(root->left) - getHeight(root->right);
		if (balance < -1) {
			if (getHeight(root->right->left) - getHeight(root->right->right) > 0)
				root->right = rotateRight(root->right);
			
			root = rotateLeft(root);
		}
		else if (balance > 1) {
			if (getHeight(root->left->left) - getHeight(root->left->right) < 0)
				root->left = rotateLeft(root->left);
			
			root = rotateRight(root);
		}
	}

	return root;

}

static NODE* _makeNode(char* data) {
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	if (newNode == NULL) return NULL;

	newNode->data = (char*)malloc(sizeof(char) * (strlen(data) + 1));
	strcpy(newNode->data, data);
	newNode->left = NULL;
	newNode->right = NULL;
	newNode->height = 1;

	return newNode;
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char* AVL_Retrieve(AVL_TREE* pTree, char* key) {
	NODE* find = _retrieve(pTree->root, key);

	if (find == NULL) return NULL;
	else return (find->data);
}

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE* _retrieve(NODE* root, char* key) {
	NODE* cur = root;

	while (cur != NULL) {
		if (!strcmp(cur->data, key)) break;

		if (strcmp(cur->data, key) > 0)
			cur = cur->left;
		else
			cur = cur->right;
	}

	if (cur == NULL) return NULL;
	else return cur;
}

/* Prints tree using inorder traversal
*/
void AVL_Traverse(AVL_TREE* pTree) {
	if (pTree->root != NULL)
		_traverse(pTree->root);
}

static void _traverse(NODE* root) {
	if (root != NULL) {
		printf("%s ", root->data);
		_traverse(root->left);
		_traverse(root->right);
	}
}


/* Prints tree using inorder right-to-left traversal
*/
void printTree(AVL_TREE* pTree) {
	if (pTree->root != NULL)
		_infix_print(pTree->root, 0);
}
/* internal traversal function
*/
static void _infix_print(NODE* root, int level) {
	if (root->right != NULL)
		_infix_print(root->right, level + 1);

	for (int i = 0; i < level; i++) printf("\t");
	printf("%s\n", root->data);

	if (root->left != NULL)
		_infix_print(root->left, level + 1);
}

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight(NODE* root) {
	if (root == NULL) return 0;
	return root->height;
}

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE* rotateRight(NODE* root) {
	NODE* lnode = root->left;

	root->left = root->left->right;
	lnode->right = root;

	root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	lnode->height = max(getHeight(lnode->left), getHeight(lnode->right)) + 1;

	return lnode;
}

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE* rotateLeft(NODE* root) {
	NODE* rnode = root->right;

	root->right = root->right->left;
	rnode->left = root;

	root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	rnode->height = max(getHeight(rnode->left), getHeight(rnode->right)) + 1;

	return rnode;
}
