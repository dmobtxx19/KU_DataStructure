#include <stdlib.h> // malloc, atoi, rand
#include <stdio.h>
#include <assert.h>
#include <time.h> // time

#define RANDOM_INPUT	1
#define FILE_INPUT		2

////////////////////////////////////////////////////////////////////////////////
// TREE type definition
typedef struct node
{
	int			data;
	struct node* left;
	struct node* right;
} NODE;

typedef struct
{
	NODE* root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE* BST_Create(void);

/* Deletes all data in tree and recycles memory
 */
void BST_Destroy(TREE* pTree);

/* internal function (not mandatory)
 */
static void _destroy(NODE* root);

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert(TREE* pTree, int data);

/* internal function (not mandatory)
 */
static void _insert(NODE* root, NODE* newPtr);

NODE* _makeNode(int data);

/* Deletes a node with dltKey from the tree
	return	1 success
			0 not found
*/
int BST_Delete(TREE* pTree, int dltKey);

/* internal function
	success is 1 if deleted; 0 if not
	return	pointer to root
*/
static NODE* _delete(NODE* root, int dltKey, int* success);

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
int* BST_Retrieve(TREE* pTree, int key);

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE* _retrieve(NODE* root, int key);

/* prints tree using inorder traversal
 */
void BST_Traverse(TREE* pTree);
static void _traverse(NODE* root);

/* Print tree using inorder right-to-left traversal
 */
void printTree(TREE* pTree);
/* internal traversal function
 */
static void _inorder_print(NODE* root, int level);

/*
	return 1 if the tree is empty; 0 if not
*/
int BST_Empty(TREE* pTree);


////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	int mode; // input mode
	TREE* tree;
	int data;

	if (argc != 2)
	{
		fprintf(stderr, "usage: %s FILE or %s number\n", argv[0], argv[0]);
		return 1;
	}

	FILE* fp;

	if ((fp = fopen(argv[1], "rt")) == NULL)
	{
		mode = RANDOM_INPUT;
	}
	else mode = FILE_INPUT;

	// creates a null tree
	printf("create\n");
	tree = BST_Create();

	if (!tree)
	{
		printf("Cannot create a tree!\n");
		return 100;
	}

	if (mode == RANDOM_INPUT)
	{
		int numbers;
		numbers = atoi(argv[1]);
		assert(numbers > 0);

		fprintf(stdout, "Inserting: ");

		srand(time(NULL));
		for (int i = 0; i < numbers; i++)
		{
			data = rand() % (numbers * 3) + 1; // random number (1 ~ numbers * 3)

			fprintf(stdout, "%d ", data);

			// insert function call
			printf("insert start\n");
			int ret = BST_Insert(tree, data);
			if (!ret) break;
		}
	}
	else if (mode == FILE_INPUT)
	{
		fprintf(stdout, "Inserting: ");

		while (fscanf(fp, "%d", &data) != EOF)
		{
			fprintf(stdout, "%d ", data);

			// insert function call
			int ret = BST_Insert(tree, data);
			if (!ret) break;
		}
		fclose(fp);
	}

	fprintf(stdout, "\n");

	if (BST_Empty(tree))
	{
		printf("empty\n");
		fprintf(stdout, "Empty tree!\n");
		BST_Destroy(tree);
		return 0;
	}

	// inorder traversal
	printf("traverse\n");
	fprintf(stdout, "Inorder traversal: ");
	BST_Traverse(tree);
	fprintf(stdout, "\n");

	// print tree with right-to-left inorder traversal
	fprintf(stdout, "Tree representation:\n");
	printTree(tree);

	while (1)
	{
		fprintf(stdout, "Input a number to delete: ");
		int num;
		if (scanf("%d", &num) == EOF) break;

		int ret = BST_Delete(tree, num);
		if (!ret)
		{
			fprintf(stdout, "%d not found\n", num);
			continue;
		}

		// print tree with right-to-left inorder traversal
		fprintf(stdout, "Tree representation:\n");
		printTree(tree);

		if (BST_Empty(tree))
		{
			fprintf(stdout, "Empty tree!\n");
			break;
		}
	}

	BST_Destroy(tree);

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE* BST_Create(void) {
	TREE* pTree = (TREE*)malloc(sizeof(TREE));
	if (pTree == NULL) return NULL;

	pTree->root = NULL;
	return pTree;
}

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy(TREE* pTree) {

	if (pTree->root != NULL)
		_destroy(pTree->root);

	free(pTree);
}

/* internal function (not mandatory)
*/
static void _destroy(NODE* root) {

	if (root != NULL) {
		_destroy(root->left);
		_destroy(root->right);
		free(root);
	}
	return;
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert(TREE* pTree, int data) {
	NODE* pNode = _makeNode(data);
	if (pNode == NULL) return 0;

	if (pTree->root == NULL) {
		pTree->root = pNode;
		return 1;
	}

	_insert(pTree->root, pNode);
	return 1;
}

/* internal function (not mandatory)
*/
static void _insert(NODE* root, NODE* newPtr) {
	NODE* cur = root;
	NODE* p = root;

	while (cur != NULL) {
		p = cur;
		if (cur->data > newPtr->data)
			cur = cur->left;
		else
			cur = cur->right;
	}

	if (p->data > newPtr->data) p->left = newPtr;
	else p->right = newPtr;
}

NODE* _makeNode(int data) {
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	if (newNode == NULL) return NULL;

	newNode->data = data;
	newNode->right = NULL;
	newNode->left = NULL;

	return newNode;
}

/* Deletes a node with dltKey from the tree
	return	1 success
			0 not found
*/
int BST_Delete(TREE* pTree, int dltKey) {
	int success;
	NODE* tmp = NULL;
	NODE* tmpp = NULL;

	if (BST_Retrieve(pTree, dltKey) == NULL)
		return 0;

	pTree->root = _delete(pTree->root, dltKey, &success);

	return success;
}

/* internal function
	success is 1 if deleted; 0 if not
	return	pointer to root
*/
static NODE* _delete(NODE* root, int dltKey, int* success) {
	NODE* cur = root;
	NODE* p = NULL;
	NODE* tmp, * tmpp;

	// find dltNode
	while (cur != NULL) {
		if (cur->data == dltKey) break;

		p = cur;
		if (cur->data > dltKey)
			cur = cur->left;
		else
			cur = cur->right;
	}

	if (cur == NULL) {
		*success = 0;
		return root;
	}

	// 0 or 1 child
	if (cur->left == NULL || cur->right == NULL) {
		tmp = NULL;
		if (cur->right != NULL)
			tmp = cur->right;
		else if (cur->left != NULL)
			tmp = cur->left;

		if (cur == root)
			root = tmp;
		else if (p->left == cur)
			p->left = tmp;
		else if (p->right == cur)
			p->right = tmp;
		free(cur);
	}
	// 2 children
	else {
		tmpp = cur;
		tmp = cur->right;
		while (tmp->left != NULL) {
			tmpp = tmp;
			tmp = tmp->left;
		}
		cur->data = tmp->data;

		if (tmpp == cur)
			tmpp->right = tmp->right;
		else
			tmpp->left = tmp->right;
		free(tmp);
	}

	*success = 1;
	return root;
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
int* BST_Retrieve(TREE* pTree, int key) {
	NODE* find = _retrieve(pTree->root, key);

	if (find == NULL) return NULL;
	else return &(find->data);
}

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE* _retrieve(NODE* root, int key) {
	NODE* cur = root;

	while (cur != NULL) {
		if (cur->data == key) break;

		if (cur->data > key)
			cur = cur->left;
		else
			cur = cur->right;
	}

	if (cur == NULL) return NULL;
	else return cur;
}

/* prints tree using inorder traversal
*/
void BST_Traverse(TREE* pTree) {
	if (pTree->root != NULL)
		_traverse(pTree->root);
}
static void _traverse(NODE* root) {
	if (root != NULL) {
		printf("%d ", root->data);
		_traverse(root->left);
		_traverse(root->right);
	}
}

/* Print tree using inorder right-to-left traversal
*/
void printTree(TREE* pTree) {
	if (pTree->root != NULL)
		_inorder_print(pTree->root, 0);
}
/* internal traversal function
*/
static void _inorder_print(NODE* root, int level) {
	if (root->right != NULL)
		_inorder_print(root->right, level + 1);

	for (int i = 0; i < level; i++) printf("\t");
	printf("%d\n", root->data);

	if (root->left != NULL)
		_inorder_print(root->left, level + 1);
}

/*
	return 1 if the tree is empty; 0 if not
*/
int BST_Empty(TREE* pTree) {
	if (pTree->root == NULL)
		return 1;
	else
		return 0;
}
